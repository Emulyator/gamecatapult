#include "stdafx.h"
#include <gctp/dbgout.hpp>
#include <gctp/app.hpp>
#include <gctp/tuki.hpp>
#include <gctp/fileserver.hpp>
#include <gctp/graphic.hpp>
#include <gctp/audio.hpp>
#include <gctp/context.hpp>
#include <gctp/font.hpp>
#include <gctp/input.hpp>
#include <gctp/color.hpp>
#include <gctp/skeleton.hpp>
#include <gctp/profiler.hpp>
#include <gctp/graphic/fonttexture.hpp>
#include <gctp/graphic/spritebuffer.hpp>
#include <gctp/graphic/particlebuffer.hpp>
#include <gctp/graphic/text.hpp>
#include <gctp/graphic/model.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/scene/flesh.hpp>
#include <gctp/scene/camera.hpp>
#include <gctp/scene/quakecamera.hpp>
#include <gctp/scene/world.hpp>
#include <gctp/scene/entity.hpp>
#include <gctp/scene/motion.hpp>
#include <gctp/scene/motionmixer.hpp>
#include <gctp/scene/light.hpp>
#include <gctp/scene/graphfile.hpp>
#include <gctp/scene/rendertree.hpp>
#include <gctp/scene/worldrenderer.hpp>

//#define MOVIETEST
#define PHYSICSTEST

#ifdef MOVIETEST
# include <gctp/movie/player.hpp>
#endif

#ifdef PHYSICSTEST
# include <btBulletDynamicsCommon.h> // for Bullet
#endif

using namespace gctp;
using namespace std;

namespace {

	bool test(Point2 p, uint8_t button, uint8_t opt)
	{
		PRNN("clicked "<<p<<","<<(int)button<<","<<(int)opt);
		return true;
	}
#ifdef PHYSICSTEST
	// 箱追加
	void addBox(btDynamicsWorld *phy_world, btAlignedObjectArray<btCollisionShape *> &phy_collision_shapes
		, HandleList<scene::Entity> &boxlist, core::Context &context, scene::World &world, const btVector3 &v)
	{
		if(phy_world->getCollisionObjectArray().size() < 100) {
			Handle<scene::Entity> entity = newEntity(context, world, "gctp.scene.Entity", 0, _T("gradriel.x"));
			if(entity) {
				boxlist.push_back(entity);
				AABox aabb = entity->target()->fleshies().front()->model()->getAABB();

				btCollisionShape *col_shape = new btBoxShape(btVector3((aabb.upper.x-aabb.lower.x)/4,(aabb.upper.y-aabb.lower.y)/2,(aabb.upper.z-aabb.lower.z)/4));
				phy_collision_shapes.push_back(col_shape);
				
				btTransform start_transform;
				start_transform.setIdentity();

				btScalar mass(10);
				
				bool is_dynamic = (mass != 0.f);
				btVector3 local_inertia(0, 0, 0);
				
				if(is_dynamic) col_shape->calculateLocalInertia(mass, local_inertia);

				start_transform.setOrigin(v);

				//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
				btDefaultMotionState *my_motion_state = new btDefaultMotionState(start_transform);
				btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, my_motion_state, col_shape, local_inertia);
				btRigidBody *body = new btRigidBody(rbinfo);

				phy_world->addRigidBody(body);
			}
		}
	}
#endif

} // anonymous namespace


extern "C" int main(int argc, char *argv[])
{
	GCTP_USE_CLASS(graphic::Texture);
	GCTP_USE_CLASS(scene::Camera);
	GCTP_USE_CLASS(scene::GraphFile);
	GCTP_USE_CLASS(scene::QuakeCamera);
	GCTP_USE_CLASS(scene::World);
	GCTP_USE_CLASS(scene::Entity);
	GCTP_USE_CLASS(scene::RenderTree);
	GCTP_USE_CLASS(scene::WorldRenderer);
	const char *mesh_mode = "Vertex Shader";
	HRslt hr;

	CoInitialize(0);
	fileserver().mount(_T("../../../media"));
	core::Context context;

	FnSlot3<Point2, uint8_t, uint8_t, test> test_slot;
	app().guievents().dblclick_signal.connect(test_slot);

	//audio::Player bgm = gctp::audio::device().ready(_T("../../../media/hugeraw.wav"));
	//bgm.play(true);
	audio::Player se = gctp::audio::device().ready(_T("../../../media/pang.wav"));
#ifdef MOVIETEST
	movie::Player movie;
	hr = movie.openForTexture(_T("../../../../alpha/alpha/odata/onegoshu.mpg"));
	if(!hr) GCTP_TRACE(hr);
	hr = movie.play();
	if(!hr) GCTP_TRACE(hr);
#endif

#ifdef PHYSICSTEST
	btDefaultCollisionConfiguration				*phy_collision_configuration;
	btCollisionDispatcher						*phy_dispatcher;
	btConstraintSolver							*phy_solver;
	btBroadphaseInterface						*phy_overlapping_pair_cache;
	btDynamicsWorld								*phy_world;			// 物理エンジンを適用するシーン
	btAlignedObjectArray<btCollisionShape *>	phy_collision_shapes;

	btCollisionShape							*phy_ground;		// 地平面
	HandleList<scene::Entity> box_list;

	phy_collision_configuration = new btDefaultCollisionConfiguration();

	phy_dispatcher = new btCollisionDispatcher(phy_collision_configuration);
	
	// 物理シミュレーションする空間のサイズの定義
	btVector3 worldAabbMin(-10000,-10000,-10000);
	btVector3 worldAabbMax(10000,10000,10000);
	// シミュレーションするShapeの限度
	phy_overlapping_pair_cache = new btAxisSweep3(worldAabbMin,worldAabbMax,100);

	// ソルバー
	btSequentialImpulseConstraintSolver *sol = new btSequentialImpulseConstraintSolver;
	phy_solver = sol;

	// シミュレーションするワールドの生成
	phy_world = new btDiscreteDynamicsWorld(phy_dispatcher, phy_overlapping_pair_cache, phy_solver, phy_collision_configuration);

	// 地平面の生成
	{
		btCollisionShape *ground_shape = new btStaticPlaneShape(btVector3(0,1,0),0);
		phy_collision_shapes.push_back(ground_shape);

		btTransform ground_transform;
		ground_transform.setIdentity();
		ground_transform.setOrigin(btVector3(0, -3, 0));

		btScalar mass(0.);

		// rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool is_dynamic = (mass != 0.f);

		btVector3 local_inertia(0, 0, 0);
		if(is_dynamic) ground_shape->calculateLocalInertia(mass, local_inertia);

		// using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState *my_motion_state = new btDefaultMotionState(ground_transform);
		btRigidBody::btRigidBodyConstructionInfo rbinfo(mass, my_motion_state, ground_shape, local_inertia);
		btRigidBody *body = new btRigidBody(rbinfo);

		// add the body to the dynamics world
		phy_world->addRigidBody(body);
	}
#endif

	graphic::Text text;

	bool qcam_on = false;
	graphic::SpriteBuffer spr;
	spr.setUp();
	Pointer<graphic::FontTexture> fonttex = new graphic::FontTexture; // どうすっかな。。。
	fonttex->setUp(512, 512);
	Pointer<Font> font = new gctp::Font;
	font->setUp(_T(",10,BOLD|FIXEDPITCH"));
	Pointer<Font> font2 = new gctp::Font;
	font2->setUp(_T(",12,NORMAL"));
	graphic::ParticleBuffer pbuf;
	pbuf.setUp();
	context.load(_T("BitmapSet4.bmp")/*_T("particle.bmp")*/);
	context.load(_T("Reflect.tga"));
	Pointer<scene::RenderTree> rtree = context.create("gctp.scene.RenderTree", _T("rt")).lock();
	if(rtree) {
		app().draw_signal.connectOnce(rtree->draw_slot);
		Pointer<scene::Camera> camera = context.create("gctp.scene.Camera", _T("camera")).lock();
		if(camera) {
			rtree->setUp(camera);
			camera->setStance(Stance(VectorC(0.0f, 0.5f, -2.0f)));
			Handle<scene::WorldRenderer> wr = context.create("gctp.scene.WorldRenderer");
			if(wr) {
				rtree->root()->push(wr);
				Handle<scene::World> world = context.create("gctp.scene.World", _T("world"));
				if(world) {
					wr->add(world);
					Pointer<scene::QuakeCamera> qcam = context.create("gctp.scene.QuakeCamera", _T("qcam")).lock();
					if(qcam) qcam->target() = camera;
					app().update_signal.connectOnce(world->update_slot);
					Pointer<scene::Entity> entity;
					entity = newEntity(context, *world, "gctp.scene.Entity", _T("chara"), _T("gradriel.x")).lock();
					if(entity) {
						//entity->skeleton().setPosType(MotionChannel::LINEAR);
						//entity->skeleton().setIsOpen(MotionChannel::CLOSE);
						//entity->do_loop_ = true;
						if(entity->mixer().isExist(0)) {
							entity->mixer().tracks()[0].setWeight(1.0f);
							entity->mixer().tracks()[1].setWeight(1.0f);
							entity->mixer().tracks()[0].setLoop(true);
							entity->mixer().tracks()[1].setLoop(true);
							entity->mixer().tracks()[2].setLoop(true);
						}
					}

					entity = newEntity(context, *world, "gctp.scene.Entity", NULL, _T("wire_test.x")).lock();

					for(int i = 0; i < 20; i++) {
						entity = newEntity(context, *world, "gctp.scene.Entity", NULL, _T("tiny.x")).lock();
						if(entity) {
							if(entity->mixer().isExist(0)) {
								entity->mixer().tracks()[0].setWeight(1.0f);
								entity->mixer().tracks()[0].setLoop(true);
								entity->getLCM() = Matrix().scale(0.01f, 0.01f, 0.01f).setPos(((float)rand()/(float)RAND_MAX)*30.0f, 0, ((float)rand()/(float)RAND_MAX)*30.0f);
								//entity->getLpos() = VectorC(((float)rand()/(float)RAND_MAX)*30.0f, 0, ((float)rand()/(float)RAND_MAX)*30.0f);
							}
						}
					}
					{
						AABox aabb = entity->target()->fleshies().front()->model()->getAABB();
						PRNN(aabb.upper << std::endl << aabb.lower);
					}

					entity = newEntity(context, *world, "gctp.scene.Entity", NULL, _T("gctp_gun.x")).lock();
					if(entity) {
						if(entity->mixer().isExist(0)) {
							entity->mixer().tracks()[0].setWeight(1.0f);
							entity->mixer().tracks()[0].setLoop(true);
						}
						//entity->speed_ = 30.0f;
						entity->getLpos().x += 2.0f;
						//entity->skeleton().setIsOpen(MotionChannel::CLOSE);
						//entity->do_loop_ = true;
					}

					entity = newEntity(context, *world, "gctp.scene.Entity", NULL, _T("gctp_base.x")).lock();
					if(entity) {
						if(entity->mixer().isExist(0)) {
							entity->mixer().tracks()[0].setWeight(1.0f);
							entity->mixer().tracks()[0].setLoop(true);
						}
						//entity->speed_ = 30.0f;
						entity->getLpos().x -= 2.0f;
						//entity->skeleton().setIsOpen(MotionChannel::CLOSE);
					}

					entity = newEntity(context, *world, "gctp.scene.Entity", NULL, _T("cell.x")).lock();
					//entity = newEntity(context, *world, "gctp.Entity", NULL, _T("room1.x")).lock();

					{
						graphic::setAmbient(Color(0.5f,0.5f,0.5f));

						graphic::DirectionalLight light;
						light.ambient = Color(0.3f, 0.3f, 0.3f);
						light.diffuse = Color(1.0f, 1.0f, 1.0f);
						light.specular = Color(0.6f, 0.6f, 0.6f);
						light.dir = VectorC(0.0f, -1.0f, 1.0f).normal();
						Pointer<scene::ParallelLight> pl = context.create("gctp.scene.ParallelLight").lock();
						if(pl) {
							pl->set(light);
							//pl->enter(*world);
						}

						light.ambient = Color(0.2f, 0.2f, 0.2f);
						light.diffuse = Color(0.5f, 0.5f, 0.5f);
						light.specular = Color(0.0f, 0.0f, 0.0f);
						light.dir = VectorC(1.0f, -1.0f, 0.0f).normal();
						pl = context.create("gctp.scene.ParallelLight").lock();
						if(pl) {
							pl->set(light);
							//pl->enter(*world);
						}
					}
				}
			}
		}
	}

	while(app().canContinue()) {
		//if(input().kbd().press(DIK_ESCAPE)) break;
		Pointer<scene::World> world = context[_T("world")].lock();
		Pointer<scene::Camera> camera = context[_T("camera")].lock();
		Pointer<scene::QuakeCamera> qcam = context[_T("qcam")].lock();
		if(camera && qcam) {
			if(input().kbd().push(DIK_TAB)) {
				qcam_on = !qcam_on;
				qcam->activate(qcam_on);
				app().showCursor(!qcam_on);
				app().holdCursor(qcam_on);
			}
			if(input().kbd().press(DIK_HOME)) {
				camera->setStance(Stance(VectorC(0.0f, 0.5f, -2.0f)));
				camera->fov() = g_pi/4;
			}
		}
		Pointer<scene::Entity> chr = context[_T("chara")].lock();
		if(chr) {
			if(input().kbd().push(DIK_NUMPADPLUS)||input().kbd().push(DIK_COMMA)) chr->mixer().setSpeed(chr->mixer().speed()+0.1f);
			if(input().kbd().push(DIK_NUMPADMINUS)||input().kbd().push(DIK_PERIOD)) chr->mixer().setSpeed(chr->mixer().speed()-0.1f);
//			if(input().kbd().push(DIK_NUMPAD1)||input().kbd().push(DIK_U)) chr->skeleton().setPosType(MotionChannel::NONE);
//			if(input().kbd().push(DIK_NUMPAD2)||input().kbd().push(DIK_I)) chr->skeleton().setPosType(MotionChannel::LINEAR);
//			if(input().kbd().push(DIK_NUMPAD3)||input().kbd().push(DIK_O)) chr->skeleton().setPosType(MotionChannel::SPLINE);
//			float weight = chr->mixer().speed()>0?1.0f:-1.0f;
			float weight = 1.0f;
			if(input().kbd().push(DIK_NUMPAD4)||input().kbd().push(DIK_J)) {
				chr->mixer().tracks()[0].setWeightDelta( weight);
				chr->mixer().tracks()[1].setWeightDelta(-weight);
				chr->mixer().tracks()[2].setWeightDelta(-weight);
				chr->mixer().tracks()[0].setSpeed(1.0f);
				chr->mixer().tracks()[1].setSpeed(0.0f);
				chr->mixer().tracks()[2].setSpeed(0.0f);
			}
			if(input().kbd().push(DIK_NUMPAD5)||input().kbd().push(DIK_K)) {
				chr->mixer().tracks()[0].setWeightDelta(-weight);
				chr->mixer().tracks()[1].setWeightDelta( weight);
				chr->mixer().tracks()[2].setWeightDelta(-weight);
				chr->mixer().tracks()[0].setSpeed(0.0f);
				chr->mixer().tracks()[1].setSpeed(1.0f);
				chr->mixer().tracks()[2].setSpeed(0.0f);
			}
			if(input().kbd().push(DIK_NUMPAD6)||input().kbd().push(DIK_L)&&chr->mixer().tracks()[2].weightDelta()<0) {
				chr->mixer().tracks()[0].setWeightDelta(-weight);
				chr->mixer().tracks()[1].setWeightDelta(-weight);
				chr->mixer().tracks()[2].setWeightDelta( weight);
				chr->mixer().tracks()[0].setSpeed(0.0f);
				chr->mixer().tracks()[1].setSpeed(0.0f);
				chr->mixer().tracks()[2].setSpeed(1.0f);
				chr->mixer().tracks()[2].setKeytime(0.0f);
			}
			if(input().kbd().push(DIK_1)) { mesh_mode = "Software"; hr = chr->target().lock()->fleshies().front()->model().lock()->useSoftware(); }
			else if(input().kbd().push(DIK_2)) { mesh_mode = "Indexed"; hr = chr->target().lock()->fleshies().front()->model().lock()->useIndexed(); }
			else if(input().kbd().push(DIK_3)) { mesh_mode = "Blended"; hr = chr->target().lock()->fleshies().front()->model().lock()->useBlended(); }
			else if(input().kbd().push(DIK_4)) { mesh_mode = "Vertex Shader"; hr = chr->target().lock()->fleshies().front()->model().lock()->useVS(); }
			else if(input().kbd().push(DIK_5)) { mesh_mode = "HLSL"; hr = chr->target().lock()->fleshies().front()->model().lock()->useShader(); }
			if(!hr) GCTP_TRACE(hr);
		}
//		if(input().mouse().push[0]) se.play();

#ifdef PHYSICSTEST
		if(phy_world) {
			if(input().kbd().push(DIK_B)) {
				addBox(phy_world, phy_collision_shapes, box_list, context, *world, btVector3(0, 2, 0));
			}
			// シミュレーションを進める
			phy_world->stepSimulation(app().lap);
			// 結果を適用
			int ii = 1;
			for(HandleList<scene::Entity>::iterator i = box_list.begin(); i != box_list.end() && ii < phy_world->getCollisionObjectArray().size(); ++i, ++ii)
			{
				float mat[16];
				phy_world->getCollisionObjectArray()[ii]->getWorldTransform().getOpenGLMatrix(mat);
				memcpy(&(*i)->getLCM(), mat, sizeof(float)*16);
				AABox aabb = (*i)->target()->fleshies().front()->model()->getAABB();
				(*i)->getLCM() = Matrix().trans(-aabb.center())*(*i)->lcm();
			}
		}
#endif
		app().update_signal(app().lap);

		if(app().canDraw()) {
			graphic::clear();
			graphic::begin();

			app().draw_signal(app().lap);

			graphic::LineParticleDesc pdesc;
			Vector pos[2];
			pos[0] = VectorC(0.5f, 0.5f, 1.0f);
			pos[1] = VectorC(1.5f, 1.5f, 1.0f);
			pdesc.num = 2;
			pdesc.pos = pos;
			pdesc.size = Vector2C(0.5f, 0.5f);
			pdesc.setUV(RectfC(0.0f, 0.0f, 1.0f, 1.0f));
			pdesc.setColor(Color32(255, 255, 255));
			pdesc.setHilight(Color32(0, 0, 0));
			graphic::setWorld(MatrixC(true));

#ifdef MOVIETEST
			pbuf.begin(*movie.getTexture());
			pbuf.draw(pdesc);
			pbuf.end();
#else
			Pointer<graphic::Texture> ptex = context[_T("BitmapSet4.bmp")].lock();
			pbuf.begin(*ptex);
			pbuf.draw(pdesc);
			pbuf.end();
#endif
			text.reset();
			text.setFont(font).setPos(10, 10).setColor(Color32(200, 200, 127)).out()
				<< "(" << graphic::getScreenSize().x << "," << graphic::getScreenSize().y << ")" << endl
				<< "FPS:" << app().fps.latestave << endl << endl;
			if(chr) text.out()
				<< "track 0 " << chr->mixer().tracks()[0].weight() << endl
				<< "	" << chr->mixer().tracks()[0].keytime() << endl
				<< "track 1 " << chr->mixer().tracks()[1].weight() << endl
				<< "	" << chr->mixer().tracks()[1].keytime() << endl
				<< "track 2 " << chr->mixer().tracks()[2].weight() << endl
				<< "	" << chr->mixer().tracks()[2].keytime() << endl << endl;
			text.out()
#ifdef PHYSICSTEST
				<< _T("箱　  :") << box_list.size() << endl
#endif
				<< _T("ヨー  :") << qcam->yaw_ << endl
				<< _T("ピッチ:") << qcam->pitch_ << endl
				<< _T("速度  :") << qcam->speed_ << endl
				<< _T("視野角:") << toDeg(camera->fov()) << _T("°") << endl
				<< _T("位置  :") << camera->stance().position << endl << endl
				<< _T("モード: ") << mesh_mode << endl << endl
				<< _T("マウス: ") << input().mouse().x << "," << input().mouse().y << " : " << input().mouse().dx << "," << input().mouse().dy;

			text.setPos(10, 500).setColor(Color32(127, 200, 127)).setBackColor(Color32(0, 0, 32)).out() << app().profile();

			if(qcam_on) text.setFont(font2).setBackColor(Color32(0,0,0,0)).setPos(10, graphic::device().getScreenSize().y-20).setColor(Color32(200, 127, 200)).out()
				<< _T("ウォークスルー中");

			text.draw(spr, *fonttex);

			graphic::end();
			app().present();
		}
	}

#ifdef PHYSICSTEST
	for(int i = phy_world->getNumCollisionObjects()-1; i >= 0; i--)
	{
		btCollisionObject *obj = phy_world->getCollisionObjectArray()[i];
		btRigidBody *body = btRigidBody::upcast(obj);
		if(body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		phy_world->removeCollisionObject( obj );
		delete obj;
	}

	for(int i = 0; i < phy_collision_shapes.size(); i++)
	{
		delete phy_collision_shapes[i];
	}

	delete phy_overlapping_pair_cache;
	delete phy_dispatcher;
	delete phy_solver;
	delete phy_world;
	delete phy_collision_configuration;
#endif
	CoUninitialize();
	return 0;
}

#ifdef _DEBUG
# pragma comment(lib, "zlibd.lib")
# ifdef MOVIETEST
#  pragma comment(lib, "strmbasd.lib")
#  pragma comment(lib, "asynbasd.lib")
# endif
# ifdef PHYSICSTEST
#  pragma comment(lib, "libbulletdynamics_d.lib")
#  pragma comment(lib, "libbulletcollision_d.lib")
#  pragma comment(lib, "libbulletmath_d.lib")
# endif
#else
# pragma comment(lib, "zlib.lib")
# ifdef MOVIETEST
#  pragma comment(lib, "strmbase.lib")
#  pragma comment(lib, "asynbase.lib")
# endif
# ifdef PHYSICSTEST
#  pragma comment(lib, "libbulletdynamics.lib")
#  pragma comment(lib, "libbulletcollision.lib")
#  pragma comment(lib, "libbulletmath.lib")
# endif
#endif
