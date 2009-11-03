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
#include <gctp/graphic/dx/skinbrush.hpp>
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

using namespace gctp;
using namespace std;


Pointer<core::Context> main_context;


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
	const char *mesh_mode = "HLSL";
	HRslt hr;

	CoInitialize(0);
	core::Context context;
	fileserver().mount(_T(""));

	graphic::Text text;

	bool qcam_on = false;
	graphic::SpriteBuffer spr;
	spr.setUp();
	Pointer<graphic::FontTexture> fonttex = new graphic::FontTexture; // どうすっかな。。。
	fonttex->setUp(512, 512);
	app().draw_signal.connectOnce(fonttex->draw_slot); // ガベージ用
	Pointer<Font> font = new gctp::Font;
	font->setUp(_T(",8,BOLD|FIXEDPITCH|OUTLINE"));
	Pointer<Font> font2 = new gctp::Font;
	font2->setUp(_T(",12,NORMAL"));
	Pointer<scene::RenderTree> rtree = context.create("gctp.scene.RenderTree", _T("rt")).lock();
	if(rtree) {
		app().draw_signal.connectOnce(rtree->draw_slot);
		Pointer<scene::Camera> camera = context.create("gctp.scene.Camera", _T("camera")).lock();
		if(camera) {
			camera->newNode();
			camera->node()->val.wtm() = Stance(VectorC(0.0f, 0.5f, -2.0f)).toMatrix();
			rtree->setUp(camera);
			Handle<scene::WorldRenderer> wr = context.create("gctp.scene.WorldRenderer");
			if(wr) {
				rtree->root()->push(wr);
				Handle<scene::World> world = context.create("gctp.scene.World", _T("world"));
				if(world) {
					wr->attach(world);
					Pointer<scene::QuakeCamera> qcam = context.create("gctp.scene.QuakeCamera", _T("qcam")).lock();
					if(qcam) qcam->target() = camera;
					app().update_signal.connectOnce(world->update_slot);

					graphic::device().setAmbientColor(Color32(Color(0.5f,0.5f,0.5f)));

					graphic::DirectionalLight light;
					light.ambient = Color(0.3f, 0.3f, 0.3f);
					light.diffuse = Color(1.0f, 1.0f, 1.0f);
					light.specular = Color(0.6f, 0.6f, 0.6f);
					light.dir = VectorC(0.0f, -1.0f, 1.0f).normal();
					Pointer<scene::Light> pl = context.create("gctp.scene.Light").lock();
					if(pl) {
						pl->newNode();
						pl->set(light);
						pl->enter(*world);
					}

					light.ambient = Color(0.2f, 0.2f, 0.2f);
					light.diffuse = Color(0.5f, 0.5f, 0.5f);
					light.specular = Color(0.0f, 0.0f, 0.0f);
					light.dir = VectorC(1.0f, -1.0f, 0.0f).normal();
					pl = context.create("gctp.scene.Light").lock();
					if(pl) {
						pl->newNode();
						pl->set(light);
						pl->enter(*world);
					}

					if(argc > 0) {
						main_context = context.newChild();
						gctp::Pointer<gctp::scene::Entity> entity;
						entity = gctp::scene::newEntity(*main_context, *world, "gctp.scene.Entity", _T("chara"), WCStr(argv[0]).c_str()).lock();
						if(entity && entity->hasMotionMixer()) {
							for(std::size_t i = 0; i < entity->mixer().tracks().size(); i++) {
								entity->mixer().tracks()[i].setWeight(i == 0 ? 1.0f : 0.0f);
								entity->mixer().tracks()[i].setSpeed(i == 0 ? 1.0f : 0.0f);
								entity->mixer().tracks()[i].setLoop(true);
							}
						}
					}
				}
			}
		}
	}

	bool hud_on = true;

	while(app().canContinue()) {
		gctp::core::Context &context = gctp::core::Context::current();
		gctp::Profiler &update_profile = app().profiler().begin("update");
		input().update(app().lap);

		app().update_signal(app().lap);

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
				camera->node()->val.wtm() = Stance(VectorC(0.0f, 0.5f, -2.0f)).toMatrix();
				camera->fov() = g_pi/4;
			}
		}
		if(input().kbd().push(DIK_ESCAPE)) hud_on = !hud_on;
		Pointer<scene::Entity> chr = context[_T("chara")].lock();
		if(chr) {
			if(input().kbd().push(DIK_NUMPADPLUS)||input().kbd().push(DIK_COMMA)) chr->mixer().setSpeed(chr->mixer().speed()+0.1f);
			if(input().kbd().push(DIK_NUMPADMINUS)||input().kbd().push(DIK_PERIOD)) chr->mixer().setSpeed(chr->mixer().speed()-0.1f);
//			if(input().kbd().push(DIK_NUMPAD1)||input().kbd().push(DIK_U)) chr->skeleton().setPosType(MotionChannel::NONE);
//			if(input().kbd().push(DIK_NUMPAD2)||input().kbd().push(DIK_I)) chr->skeleton().setPosType(MotionChannel::LINEAR);
//			if(input().kbd().push(DIK_NUMPAD3)||input().kbd().push(DIK_O)) chr->skeleton().setPosType(MotionChannel::SPLINE);
			if(chr->hasMotionMixer() && chr->mixer().tracks().size() > 0) {
				if(input().kbd().push(DIK_DOWN)) {
					for(std::size_t i = 0; i < chr->mixer().tracks().size(); i++) {
						if(chr->mixer().tracks()[i].weight() > 0) {
							chr->mixer().tracks()[i].setWeight(0.0f);
							chr->mixer().tracks()[i].setSpeed(0.0f);
							if(i+1 < chr->mixer().tracks().size()) {
								chr->mixer().tracks()[i+1].setWeight(1.0f);
								chr->mixer().tracks()[i+1].setKeytime(0.0f);
								chr->mixer().tracks()[i+1].setSpeed(1.0f);
							}
							else {
								chr->mixer().tracks()[0].setWeight(1.0f);
								chr->mixer().tracks()[0].setKeytime(0.0f);
								chr->mixer().tracks()[0].setSpeed(1.0f);
							}
							break;
						}
					}
				}
				else if(input().kbd().push(DIK_UP)) {
					for(std::size_t i = 0; i < chr->mixer().tracks().size(); i++) {
						if(chr->mixer().tracks()[i].weight() > 0) {
							chr->mixer().tracks()[i].setWeight(0.0f);
							chr->mixer().tracks()[i].setSpeed(0.0f);
							if(i > 0) {
								chr->mixer().tracks()[i-1].setWeight(1.0f);
								chr->mixer().tracks()[i-1].setKeytime(0.0f);
								chr->mixer().tracks()[i-1].setSpeed(1.0f);
							}
							else {
								chr->mixer().tracks()[chr->mixer().tracks().size()-1].setWeight(1.0f);
								chr->mixer().tracks()[chr->mixer().tracks().size()-1].setKeytime(0.0f);
								chr->mixer().tracks()[chr->mixer().tracks().size()-1].setSpeed(1.0f);
							}
							break;
						}
					}
				}
			}
			if(input().kbd().push(DIK_1)) {
				mesh_mode = "Software";
				Pointer<graphic::dx::SoftwareSkinBrush> brush = new graphic::dx::SoftwareSkinBrush;
				if(brush && (hr = brush->setUp(chr->target().lock()->fleshies().front()->model()))) chr->target().lock()->fleshies().front()->model().lock()->brush() = brush;
			}
			else if(input().kbd().push(DIK_2)) {
				mesh_mode = "Indexed";
				Pointer<graphic::dx::BlendedSkinBrush> brush = new graphic::dx::BlendedSkinBrush;
				if(brush && (hr = brush->setUp(chr->target().lock()->fleshies().front()->model()))) chr->target().lock()->fleshies().front()->model().lock()->brush() = brush;
			}
			else if(input().kbd().push(DIK_3)) {
				mesh_mode = "Blended";
				Pointer<graphic::dx::IndexedSkinBrush> brush = new graphic::dx::IndexedSkinBrush;
				if(brush && (hr = brush->setUp(chr->target().lock()->fleshies().front()->model()))) chr->target().lock()->fleshies().front()->model().lock()->brush() = brush;
			}
			else if(input().kbd().push(DIK_4)) {
				mesh_mode = "Vertex Shader";
				Pointer<graphic::dx::VertexShaderSkinBrush> brush = new graphic::dx::VertexShaderSkinBrush;
				if(brush && (hr = brush->setUp(chr->target().lock()->fleshies().front()->model()))) chr->target().lock()->fleshies().front()->model().lock()->brush() = brush;
			}
			else if(input().kbd().push(DIK_5)) {
				mesh_mode = "HLSL";
				Pointer<graphic::dx::ShaderSkinBrush> brush = new graphic::dx::ShaderSkinBrush;
				if(brush && (hr = brush->setUp(chr->target().lock()->fleshies().front()->model()))) chr->target().lock()->fleshies().front()->model().lock()->brush() = brush;
			}
			if(!hr) GCTP_TRACE(hr);
		}
		update_profile.end();

		if(app().canDraw()) {
			Profiling draw_profile("draw");

			graphic::device().clear();
			graphic::device().begin();

			app().draw_signal(app().lap);

			text.reset();
			text.setFont(font).setPos(10, 10).setColor(Color32(200, 200, 127)).out()
				<< "FPS:" << app().fps.latestave << endl;
			if(hud_on) {
				text.out() << "(" << graphic::device().getScreenSize().x << "," << graphic::device().getScreenSize().y << ")" << endl << endl;
				if(chr && chr->hasMotionMixer()) {
					for(std::size_t i = 0; i < chr->mixer().tracks().size(); i++) {
						text.out() << "track "<< i << " " << chr->mixer().tracks()[i].keytime() << endl;
					}
				}
				text.out() << endl
					<< _T("ヨー  :") << qcam->yaw << endl
					<< _T("ピッチ:") << qcam->pitch << endl
					<< _T("速度  :") << qcam->speed << endl
					<< _T("視野角:") << toDeg(camera->fov()) << _T("°") << endl
					<< _T("位置  :") << camera->node()->val.wtm().position() << endl << endl
					<< _T("モード: ") << mesh_mode << endl << endl
					//<< _T("マウス: ") << input().mouse().x << "," << input().mouse().y << " : " << input().mouse().dx << "," << input().mouse().dy << endl
					;

				//text.setColor(Color32(127, 200, 127)).out() << app().profiler();

				if(qcam_on) text.setFont(font2).setPos(10, graphic::device().getScreenSize().y-20).setColor(Color32(200, 127, 200)).out()
					<< _T("ウォークスルー中");
			}

			text.draw(spr, *fonttex);

			graphic::device().end();
			{
				Profiling present_profile("present");
				app().present();
			}
		}
	}

	main_context = 0;

	CoUninitialize();
	return 0;
}
