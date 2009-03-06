/** @file
 * GameCatapult スピーカー（３Ｄサウンド音源）オブジェクトクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/speaker.hpp>
#include <gctp/scene/world.hpp>
#include <gctp/audio.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	Speaker::Speaker() : vol_delta(0)
	{
	}

	void Speaker::set(const audio::Player &ambient)
	{
		player_ = ambient;
		speaker_ = audio::Speaker();
	}

	void Speaker::set(const audio::Speaker &omni_or_spot)
	{
		player_ = audio::Player();
		speaker_ = omni_or_spot;
	}

	void Speaker::newNode()
	{
		own_node_ = StrutumNode::create();
		node_ = own_node_;
	}

	void Speaker::attach(Handle<StrutumNode> node)
	{
		if(own_node_) {
			own_node_->remove();
			own_node_ = 0;
		}
		node_ = node;
	}

	void Speaker::enter(World &world)
	{
		world.speaker_list.push_back(this);
		world.speaker_list.unique();
		if(own_node_) world.strutum_tree.root()->push(own_node_);
	}

	void Speaker::exit(World &world)
	{
		world.speaker_list.remove(this);
		if(own_node_) own_node_->remove();
	}

	void Speaker::apply(float delta) const
	{
		Speaker *self = const_cast<Speaker *>(this);
		switch(type_) {
		case AMBIENT:	{
						}
			break;
		case OMNI:		{
			self->speaker_.setPosition(node_->val.wtm().position());
			if(delta != 0) self->speaker_.setVelocity((node_->val.wtm().position()-node_->val.prevWTM().position())/delta);
			if(vol_delta != 0) {
				self->cur_vol += vol_delta*delta;
				if(vol_delta > 0 && cur_vol > 1) {
					self->vol_delta = 0;
					self->cur_vol = 1;
				}
				if(vol_delta < 0 && cur_vol < 0) {
					self->vol_delta = 0;
					self->cur_vol = 0;
				}
				if(cur_vol > 0 && !self->speaker_.isPlaying()) {
					//dbgout << "PLAY!" << range_coeff << endl;
					self->speaker_.play(0);
				}
				if(cur_vol == 0 && self->speaker_.isPlaying()) {
					//dbgout << "STOP!" << range_coeff << endl;
					self->speaker_.stop();
				}
				self->speaker_.setVolume(cur_vol);
			}
			self->speaker_.commit();
						}
			break;
		case SPOT:		{
						}
			break;
		}
	}

	void Speaker::update()
	{
		switch(type_) {
		case AMBIENT:
		case OMNI:
			/*bs_.r = range;
			bs_.c = node_->val.wtm().position();*/
			break;
		case SPOT:
			// 円錐が内接する球を求める
			/*bs_.r = range*tanf(0.5f*phi);
			if(bs_.r < range) {
				float cos_phi_half = cosf(0.5f*phi);
				bs_.r = range/(cos_phi_half*cos_phi_half*2);
				bs_.c = node_->val.wtm().position()+node_->val.wtm().at().normal()*bs_.r;
			}
			else {
				bs_.c = node_->val.wtm().position()+node_->val.wtm().at().normal()*range;
			}*/
			break;
		}
	}

	bool Speaker::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void Speaker::newNode(luapp::Stack &L)
	{
		newNode();
	}

	void Speaker::attach(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			attach(tuki_cast<StrutumNode>(L[1]));
		}
	}

	void Speaker::enter(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) enter(*world);
		}
	}

	void Speaker::exit(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) exit(*world);
		}
	}

	void Speaker::setSource(luapp::Stack &L)
	{
		if(L.top()>=1) {
			bool streaming = true;
			if(L.top()>=2) streaming = L[2].toBoolean();
			type_ = OMNI;
#ifdef _UNICODE
			gctp::WCStr str = L[1].toCStr();
			set(gctp::audio::device().newSpeaker(str.c_str(), streaming));
#else
			set(gctp::audio::device().newSpeaker(L[1].toCStr(), streaming));
#endif
			/*const char *s = L[1].toCStr();
			if(s) {
				switch(*s) {
				case 'a':
				case 'A':
					type_ = AMBIENT;
					break;
				case 'o':
				case 'O':
					type_ = OMNI;
					break;
				case 's':
				case 'S':
					type_ = SPOT;
					break;
				}
			}*/
		}
	}

	int Speaker::getType(luapp::Stack &L)
	{
		switch(type_) {
		case AMBIENT:
			L << "AMBIENT";
			return 1;
		case OMNI:
			L << "OMNI";
			return 1;
		case SPOT:
			L << "SPOT";
			return 1;
		}
		return 0;
	}

	int Speaker::isPlaying(luapp::Stack &L)
	{
		L << speaker_.isPlaying();
		return 1;
	}

	int Speaker::play(luapp::Stack &L)
	{
		//gctp::app().update_signal.connectOnce(on_update);
		if(L.top() >= 1) speaker_.play(L[1].toInteger());
		else speaker_.play();
		return 0;
	}

	int Speaker::stop(luapp::Stack &L)
	{
		speaker_.stop();
		//gctp::app().update_signal.disconnect(on_update);
		return 0;
	}

	void Speaker::setPosition(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			if(node_) node_->val.getLCM().setPos(VectorC((float)L[1].toNumber(),(float)L[2].toNumber(),(float)L[3].toNumber()));
		}
	}

	int Speaker::getPosition(luapp::Stack &L)
	{
		if(node_) {
			Vector v = node_->val.lcm().position();
			L << v.x << v.y << v.z;
			return 3;
		}
		return 0;
	}

	void Speaker::setPosture(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			if(node_) {
				Coord c = node_->val.lcm();
				c.posture = QuatC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber());
				node_->val.getLCM() = c.toMatrix();
			}
		}
	}

	/*int Speaker::getPosture(luapp::Stack &L)
	{
		if(node_) {
			Coord c = node_->val.lcm();
			L << c.posture.yaw() << c.posture.pitch() << c.posture.roll();
			return 3;
		}
		return 0;
	}*/

	void Speaker::setRange(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			speaker_.setMaxDistance((float)L[1].toNumber());
			speaker_.setMinDistance((float)L[2].toNumber());
		}
	}

	int Speaker::getRange(luapp::Stack &L)
	{
		L << speaker_.getMaxDistance() << speaker_.getMinDistance();
		return 2;
	}

	void Speaker::setVolume(luapp::Stack &L)
	{
		if(L.top() >= 1) speaker_.setVolume((float)L[1].toNumber());
	}

	int Speaker::getVolume(luapp::Stack &L)
	{
		L << speaker_.getVolume();
		return 1;
	}

	int Speaker::getFrequency(luapp::Stack &L)
	{
		L << (int)speaker_.getFrequency();
		return 1;
	}

	void Speaker::setFrequency(luapp::Stack &L)
	{
		if(L.top() >= 1) speaker_.setFrequency(L[1].toInteger());
	}

	void Speaker::fadein(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			vol_delta = 1.0f/(float)L[1].toNumber();
		}
	}

	void Speaker::fadeout(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			vol_delta = -1.0f/(float)L[1].toNumber();
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, Speaker, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, Speaker)
		TUKI_METHOD(Speaker, newNode)
		TUKI_METHOD(Speaker, attach)
		TUKI_METHOD(Speaker, enter)
		TUKI_METHOD(Speaker, exit)
		TUKI_METHOD(Speaker, setSource)
		TUKI_METHOD(Speaker, getType)
		TUKI_METHOD(Speaker, isPlaying)
		TUKI_METHOD(Speaker, play)
		TUKI_METHOD(Speaker, stop)
		TUKI_METHOD(Speaker, setPosition)
		TUKI_METHOD(Speaker, getPosition)
		TUKI_METHOD(Speaker, setPosture)
		//TUKI_METHOD(Speaker, getPosture)
		TUKI_METHOD(Speaker, setRange)
		TUKI_METHOD(Speaker, getRange)
		TUKI_METHOD(Speaker, setVolume)
		TUKI_METHOD(Speaker, getVolume)
		TUKI_METHOD(Speaker, setFrequency)
		TUKI_METHOD(Speaker, getFrequency)
		TUKI_METHOD(Speaker, fadein)
		TUKI_METHOD(Speaker, fadeout)
	TUKI_IMPLEMENT_END(Speaker)

}} // namespace gctp::scene
