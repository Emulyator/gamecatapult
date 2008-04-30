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
#include <gctp/scene/stage.hpp>
#include <gctp/scene/entity.hpp>
#include <gctp/scene/motion.hpp>
#include <gctp/scene/motionmixer.hpp>
#include <gctp/scene/light.hpp>
#include <gctp/scene/graphfile.hpp>
//#include <ode/ode.h>

#define MOVIETEST

#ifdef MOVIETEST
# include <gctp/movie/player.hpp>
#endif

using namespace gctp;
using namespace std;

namespace {

	bool test(Point2 p, uint8_t button, uint8_t opt)
	{
		PRNN("clicked "<<p<<","<<(int)button<<","<<(int)opt);
		return true;
	}

} // anonymous namespace

extern "C" int main(int argc, char *argv[])
{
	GCTP_USE_CLASS(scene::Camera);
	GCTP_USE_CLASS(scene::GraphFile);
	GCTP_USE_CLASS(graphic::Texture);
	GCTP_USE_CLASS(scene::QuakeCamera);
	GCTP_USE_CLASS(scene::Entity);
	const char *mesh_mode = "Vertex Shader";
	HRslt hr;

	CoInitialize(0);
	fileserver().mount(_T("../../../media"));
	Context context;

	FnSlot3<Point2, uint8_t, uint8_t, test> test_slot;
	app().guievents().dblclick_signal.connect(test_slot);

	audio::Player bgm = gctp::audio::device().ready(_T("../../../media/hugeraw.wav"));
	bgm.play(true);
	audio::Player se = gctp::audio::device().ready(_T("../../../media/pang.wav"));
#ifdef MOVIETEST
	movie::Player movie;
	hr = movie.openForTexture(_T("../../../../alpha/alpha/odata/onegoshu.mpg"));
	if(!hr) GCTP_TRACE(hr);
	hr = movie.play();
	if(!hr) GCTP_TRACE(hr);
#endif
	// ルートスクリプト実行
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
	{
		Pointer<scene::Camera> camera = context.create("gctp.Camera", _T("camera")).lock();
		if(camera) {
			camera->setStance(Stance(VectorC(0.0f, 0.5f, -2.0f)));
			app().draw_signal.connectOnce(camera->draw_slot);
			Pointer<scene::Stage> stage = context.create("gctp.Stage", _T("stage")).lock();
			if(stage) {
				Pointer<scene::QuakeCamera> qcam = context.create("gctp.QuakeCamera", _T("qcam")).lock();
				if(qcam) qcam->target() = camera;
				camera->draw_signal.connectOnce(stage->draw_slot);
				app().update_signal.connectOnce(stage->update_slot);
				Pointer<scene::Entity> entity;
				entity = newEntity(context, *stage, "gctp.Entity", _T("chara"), _T("gradriel.x")).lock();
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

				entity = newEntity(context, *stage, "gctp.Entity", NULL, _T("wire_test.x")).lock();

				for(int i = 0; i < 20; i++) {
					entity = newEntity(context, *stage, "gctp.Entity", NULL, _T("gradriel.x")).lock();
					if(entity) {
						entity->mixer().tracks()[0].setWeight(1.0f);
						entity->mixer().tracks()[0].setLoop(true);
						entity->getLpos().x = ((float)rand()/(float)RAND_MAX)*30.0f;
						entity->getLpos().z = ((float)rand()/(float)RAND_MAX)*30.0f;
					}
				}

				entity = newEntity(context, *stage, "gctp.Entity", NULL, _T("gctp_gun.x")).lock();
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

				entity = newEntity(context, *stage, "gctp.Entity", NULL, _T("gctp_base.x")).lock();
				if(entity) {
					if(entity->mixer().isExist(0)) {
						entity->mixer().tracks()[0].setWeight(1.0f);
						entity->mixer().tracks()[0].setLoop(true);
					}
					//entity->speed_ = 30.0f;
					entity->getLpos().x -= 2.0f;
					//entity->skeleton().setIsOpen(MotionChannel::CLOSE);
				}

				entity = newEntity(context, *stage, "gctp.Entity", NULL, _T("cell.x")).lock();
				//entity = newEntity(context, *stage, "gctp.Entity", NULL, _T("room1.x")).lock();

				{
					graphic::setAmbient(Color(0.5f,0.5f,0.5f));

					graphic::DirectionalLight light;
					light.ambient = Color(0.3f, 0.3f, 0.3f);
					light.diffuse = Color(1.0f, 1.0f, 1.0f);
					light.specular = Color(0.6f, 0.6f, 0.6f);
					light.dir = VectorC(0.0f, -1.0f, 1.0f).normal();
					Pointer<scene::ParallelLight> pl = context.create("gctp.ParallelLight").lock();
					if(pl) {
						pl->set(light);
						//pl->enter(*stage);
					}

					light.ambient = Color(0.2f, 0.2f, 0.2f);
					light.diffuse = Color(0.5f, 0.5f, 0.5f);
					light.specular = Color(0.0f, 0.0f, 0.0f);
					light.dir = VectorC(1.0f, -1.0f, 0.0f).normal();
					pl = context.create("gctp.ParallelLight").lock();
					if(pl) {
						pl->set(light);
						//pl->enter(*stage);
					}
				}
			}
		}
	}

	while(app().canContinue()) {
		//if(input().kbd().press(DIK_ESCAPE)) break;
		Pointer<scene::Stage> stage = context[_T("stage")].lock();
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
		Pointer<scene::Entity> chr = (*stage)[_T("chara")].lock();
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
			else if(input().kbd().push(DIK_5)) { mesh_mode = "HLSL"; hr = chr->target().lock()->fleshies().front()->model().lock()->useBrush(); }
			if(!hr) GCTP_TRACE(hr);
		}
//		if(input().mouse().push[0]) se.play();

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
	CoUninitialize();
	return 0;
}

#ifdef _DEBUG
# pragma comment(lib, "zlibd.lib")
# ifdef MOVIETEST
#  pragma comment(lib, "strmbasd.lib")
#  pragma comment(lib, "asynbasd.lib")
# endif
#else
# pragma comment(lib, "zlib.lib")
# ifdef MOVIETEST
#  pragma comment(lib, "strmbase.lib")
#  pragma comment(lib, "asynbase.lib")
# endif
#endif
