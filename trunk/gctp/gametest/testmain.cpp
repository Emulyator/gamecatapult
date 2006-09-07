#include "stdafx.h"
#include <gctp/dbgout.hpp>
#include <gctp/app.hpp>
#include <gctp/tuki.hpp>
#include <gctp/graphic.hpp>
#include <gctp/audio.hpp>
#include <gctp/context.hpp>
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

#if 0

class Game : public gctp::Object {
public:
	gctp::Context context;
	
	static luapp::State &lua()
	{
		return lua__;
	}

	static int run(const char *filename)
	{
		lua().open();
		lua().registerEnv(gctp::TukiInitializer());
		gctp::TukiRegister::registerIt(lua(), "Game");
		gctp::Tuki<Game>::newUserData(lua(), new Game);
		lua().global().declare("game");
#ifndef NDEBUG
		lua().global().declare("_DEBUG", true);
#endif
		int ret = 0;
		if(!lua().run(filename)) ret = 1;
		lua().close();
		return ret;
	}

protected:
	bool setUp(luapp::Stack &)
	{
		return false;
	}

	int run(luapp::Stack &L)
	{
		GCTP_TRACE("RUNNING");
		gctp::graphic::Text text;
		while(canContinue(L)) {
			{
				gctp::Profiling update_prof("update");
				gctp::app().update_signal(gctp::app().lap);
				if(L.top() > 0 && L[1].isTable() && L[1].toTable()["update"].isFunction()) {
					luapp::Argument1<lua_Number> arg(gctp::app().lap);
					L[1].toTable()["update"](luapp::Callback::dummy(), arg);
				}
			}
			if(gctp::app().canDraw()) {
				gctp::graphic::clear();
				gctp::graphic::begin();
				gctp::app().draw_signal();
				text.setPos(10, 10).setColor(gctp::Color32(200, 200, 127)).setFixedPitch(true).out()
					<< gctp::graphic::getScreenSize() << std::endl
					<< "ＦＰＳ:" << gctp::app().fps.latestave << std::endl << std::endl
					<< "マウス: " << gctp::input().mouse().x << "," << gctp::input().mouse().y << " : " << gctp::input().mouse().dx << "," << gctp::input().mouse().dy;
				text.setPos(10, 500).setColor(gctp::Color32(127, 200, 127)).setBackColor(gctp::Color32(0, 0, 32)).setFixedPitch(true).out()
					<< gctp::app().profile();
				text.draw();
				gctp::graphic::end();
				gctp::app().present();
			}
		}
		return 0;
	}

	static int print(luapp::Stack &L)
	{
		int n = lua_gettop(L);  /* number of arguments */
		int i;
		lua_getglobal(L, "tostring");
		for (i=1; i<=n; i++) {
			const char *s;
			lua_pushvalue(L, -1);  /* function to be called */
			lua_pushvalue(L, i);   /* value to print */
			lua_call(L, 1, 1);
			s = lua_tostring(L, -1);  /* get result */
			if (s == NULL)
			return luaL_error(L, LUA_QL("tostring") " must return a string to " LUA_QL("print"));
			if (i>1) gctp::dbgout << "\t";
			gctp::dbgout << s;
			lua_pop(L, 1);  /* pop result */
		}
		gctp::dbgout << std::endl;
		return 0;
	}

	int init(luapp::Stack &L)
	{
		gctp::graphic::setAmbient(gctp::Color(0.5f,0.5f,0.5f));

		gctp::graphic::DirectionalLight light;
		light.ambient = gctp::Color(0.3f, 0.3f, 0.3f);
		light.diffuse = gctp::Color(1.0f, 1.0f, 1.0f);
		light.specular = gctp::Color(0.6f, 0.6f, 0.6f);
		light.dir = gctp::VectorC(0.0f, -1.0f, 1.0f).normal();
		gctp::Pointer<gctp::scene::ParallelLight> pl = context.create("gctp.ParallelLight").lock();
		if(pl) {
			pl->set(light);
			//pl->enter(*stage);
		}

		light.ambient = gctp::Color(0.2f, 0.2f, 0.2f);
		light.diffuse = gctp::Color(0.5f, 0.5f, 0.5f);
		light.specular = gctp::Color(0.0f, 0.0f, 0.0f);
		light.dir = gctp::VectorC(1.0f, -1.0f, 0.0f).normal();
		pl = context.create("gctp.ParallelLight").lock();
		if(pl) {
			pl->set(light);
			//pl->enter(*stage);
		}

		return 0;
	}

private:
	Game()
	{
	}

	bool canContinue(luapp::Stack &L)
	{
		if(!gctp::app().canContinue()) return false;
		if(L.top() > 0 && L[1].isTable() && L[1].toTable()["continue"].isFunction()) {
			luapp::Return1<bool> ret;
			L[1].toTable()["continue"](ret);
			if(!ret.ret1) {
				PRNN("countinue false");
			}
			return ret.ret1;
		}
		return true;
	}

	static luapp::State lua__;

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(Game)
};
luapp::State Game::lua__;
GCTP_IMPLEMENT_CLASS(Game, Object)
TUKI_IMPLEMENT_BEGIN(Game)
	TUKI_METHOD(Game, run)
	TUKI_METHOD(Game, print)
	TUKI_METHOD(Game, init)
TUKI_IMPLEMENT_END(Game)


class TextPrinter : public gctp::Object {
public:
	TextPrinter()
	{
		draw_slot.bind(this);
	}

	bool onDraw() const
	{
		text_.draw();
		return true;
	}
	gctp::MemberSlot0<const TextPrinter, &TextPrinter::onDraw> draw_slot;

protected:
	bool setUp(luapp::Stack &L)
	{
		gctp::app().draw_signal.connect(draw_slot);
		return true;
	}
	void setPos(luapp::Stack &L)
	{
		if(L.top()==3) {
			text_.setPos(L[2].toNumber(), L[3].toNumber());
		}
	}
	void setClumpPos(luapp::Stack &L)
	{
		if(L.top()==3) {
			text_.setClumpPos(L[2].toNumber(), L[3].toNumber());
		}
	}
	void setColor(luapp::Stack &L)
	{
		if(L.top()>=5) {
			text_.setColor(gctp::Color32(L[2].toInteger(), L[3].toInteger(), L[4].toInteger(), L[5].toInteger()));
		}
		else if(L.top()>=4) {
			text_.setColor(gctp::Color32(L[2].toInteger(), L[3].toInteger(), L[4].toInteger()));
		}
		else if(L.top()>=2) {
			text_.setColor(gctp::Color32(L[2].toCStr()));
		}
	}
	void print(luapp::Stack &L)
	{
		for(int i = 2; i <= L.top(); i++) {
			const char *s = L[i].toCStr();
			text_.out() << (s ? s : "?");
		}
	}

private:
	gctp::graphic::Text text_;

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(TextPrinter)
};
GCTP_IMPLEMENT_CLASS(TextPrinter, gctp::Object)
TUKI_IMPLEMENT_BEGIN_NS(Scene, TextPrinter)
	TUKI_METHOD(TextPrinter, setPos)
	TUKI_METHOD(TextPrinter, setClumpPos)
	TUKI_METHOD(TextPrinter, setColor)
	TUKI_METHOD(TextPrinter, print)
TUKI_IMPLEMENT_END(TextPrinter)

using namespace gctp;
using namespace std;

extern "C" int main(int argc, char *argv[])
{
	GCTP_USE_CLASS(graphic::Texture);
	GCTP_USE_CLASS(scene::GraphFile);
	GCTP_USE_CLASS(scene::Camera);
	GCTP_USE_CLASS(scene::QuakeLikeCamera);
	GCTP_USE_CLASS(scene::Entity);
	return Game::run("../../../media/ini.lua");
}

#else

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
	GCTP_USE_CLASS(scene::QuakeLikeCamera);
	GCTP_USE_CLASS(scene::Entity);
	const char *mesh_mode = "Vertex Shader";
	HRslt hr;

	FnSlot3<Point2, uint8_t, uint8_t, test> test_slot;
	app().guievents().dblclick_signal.connect(test_slot);

//	audio::Player bgm = audio::ready("../media/hugeraw.wav");
//	bgm.play(true);
//	audio::Player se = audio::ready("../media/pang.wav");

	graphic::Text text;

	graphic::ParticleBuffer pbuf;
	pbuf.setUp();
	Pointer<graphic::Texture> ptex = createOnDB<graphic::Texture>("../../../media/BitmapSet4.bmp"/*"particle.bmp"*/);

	// ルートスクリプト実行
	Context context;
	context.load("../../../media/Reflect.tga");
	Pointer<scene::Stage> stage = context.create("gctp.Stage").lock();
	if(stage) {
		Pointer<scene::Camera> camera = stage->newNode(context, "gctp.Camera", "camera").lock();
		if(camera) {
			scene::Camera *c = camera.get();
			camera->setUp();
			camera->enter(*stage);
			camera->node()->val.getLCM().right() = VectorC(1.0f, 0.0f, 0.0f);
			camera->node()->val.getLCM().up() = VectorC(0.0f, 1.0f, 0.0f);
			camera->node()->val.getLCM().at() = VectorC(0.0f, 0.0f, 1.0f);
			camera->node()->val.getLCM().position() = VectorC(0.0f, 0.5f, -2.0f);
		}
		Pointer<scene::Entity> entity;
		entity = newEntity(context, *stage, "gctp.Entity", "chara", "../../../media/gradriel.x").lock();
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

		entity = newEntity(context, *stage, "gctp.Entity", NULL, "../../../media/wire_test.x").lock();

		for(int i = 0; i < 20; i++) {
			entity = newEntity(context, *stage, "gctp.Entity", NULL, "../../../media/gradriel.x").lock();
			if(entity) {
				entity->mixer().tracks()[0].setWeight(1.0f);
				entity->mixer().tracks()[0].setLoop(true);
				entity->getLpos().x = ((float)rand()/(float)RAND_MAX)*30.0f;
				entity->getLpos().z = ((float)rand()/(float)RAND_MAX)*30.0f;
			}
		}
		entity = newEntity(context, *stage, "gctp.Entity", NULL, "../../../media/gctp_gun.x").lock();
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
		entity = newEntity(context, *stage, "gctp.Entity", NULL, "../../../media/gctp_base.x").lock();
		if(entity) {
			if(entity->mixer().isExist(0)) {
				entity->mixer().tracks()[0].setWeight(1.0f);
				entity->mixer().tracks()[0].setLoop(true);
			}
			//entity->speed_ = 30.0f;
			entity->getLpos().x -= 2.0f;
			//entity->skeleton().setIsOpen(MotionChannel::CLOSE);
		}

		entity = newEntity(context, *stage, "gctp.Entity", NULL, "../../../media/cell.x").lock();
		//entity = newEntity(context, *stage, "gctp.Entity", NULL, "../../../media/room1.x").lock();

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
		Pointer<scene::QuakeLikeCamera> qcam = stage->newNode(context, "gctp.QuakeLikeCamera", "qcamera").lock();
		if(qcam) {
			qcam->target() = camera;
			qcam->enter(*stage);
		}
	}

	while(app().canContinue()) {
		//if(input().kbd().press(DIK_ESCAPE)) break;
		Pointer<scene::Camera> camera = (*stage)["camera"].lock();
		Pointer<scene::QuakeLikeCamera> qcam = (*stage)["qcamera"].lock();
		if(camera && qcam) {
			if(input().kbd().push(DIK_TAB)) {
				qcam->setEnable(!qcam->enable());
				app().showCursor(!qcam->enable());
				app().holdCursor(qcam->enable());
			}
			if(input().kbd().press(DIK_HOME)) {
				camera->node()->val.getLCM().right() = VectorC(1.0f, 0.0f, 0.0f);
				camera->node()->val.getLCM().up() = VectorC(0.0f, 1.0f, 0.0f);
				camera->node()->val.getLCM().at() = VectorC(0.0f, 0.0f, 1.0f);
				camera->node()->val.getLCM().position() = VectorC(0.0f, 0.5f, -2.0f);
				camera->fov() = g_pi/4;
			}
		}
		Pointer<scene::Entity> chr = (*stage)["chara"].lock();
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

		stage->onUpdate(app().lap);

		if(app().canDraw()) {
			graphic::clear();
			graphic::begin();

			stage->onDraw();

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
			pbuf.begin(*ptex);
			pbuf.draw(pdesc);
			pbuf.end();

			text.setPos(10, 10).setColor(Color32(200, 200, 127)).setFixedPitch(true).out()
				<< "(" << graphic::getScreenSize().x << "," << graphic::getScreenSize().y << ")" << endl
				<< "ＦＰＳ:" << app().fps.latestave << endl << endl
				<< "track 0 " << chr->mixer().tracks()[0].weight() << endl
				<< "	" << chr->mixer().tracks()[0].keytime() << endl
				<< "track 1 " << chr->mixer().tracks()[1].weight() << endl
				<< "	" << chr->mixer().tracks()[1].keytime() << endl
				<< "track 2 " << chr->mixer().tracks()[2].weight() << endl
				<< "	" << chr->mixer().tracks()[2].keytime() << endl << endl
				<< "ヨー  :" << qcam->yaw_ << endl
				<< "ピッチ:" << qcam->pitch_ << endl
				<< "速度  :" << qcam->speed_ << endl
				<< "視野角:" << toDeg(camera->fov()) << "°" << endl
				<< "位置  :" << camera->node()->val.wtm().position() << endl << endl
				<< "モード: " << mesh_mode << endl << endl
				<< "マウス: " << input().mouse().x << "," << input().mouse().y << " : " << input().mouse().dx << "," << input().mouse().dy;

			text.setPos(10, 500).setColor(Color32(127, 200, 127)).setBackColor(Color32(0, 0, 32)).setFixedPitch(true).out() << app().profile();

			if(qcam->enable()) text.setFixedPitch(false).setClumpPos(10, -20).setColor(Color32(200, 127, 200)).out()
				<< "ウォークスルー中";

			text.draw();

			graphic::end();
			app().present();
		}
	}
	return 0;
}
#endif
