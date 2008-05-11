/** @file
 * GameCatapult シーン要素クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/entity.hpp>
#include <gctp/scene/graphfile.hpp>
#include <gctp/scene/flesh.hpp>
#include <gctp/scene/body.hpp>
#include <gctp/scene/stage.hpp>
#include <gctp/scene/motion.hpp>
#include <gctp/scene/motionmixer.hpp>
#include <gctp/graphic/model.hpp>
#include <gctp/app.hpp>
#include <gctp/context.hpp>
#include <gctp/db.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	Entity::Entity(int16_t update_pri) : update_slot(update_pri)
	{
		 update_slot.bind(this);
	}

	void Entity::setUp(Pointer<Body> src)
	{
		target_ = src->dup();
	}

	void Entity::setUp(const _TCHAR *filename)
	{
		if(filename) {
			Pointer<GraphFile> file = context()[filename].lock();
			if(file) {
				Pointer<Motion> pmtn;
				Pointer<Body> pbody;
				for(GraphFile::iterator i = file->begin(); i != file->end(); ++i) {
					pmtn = *i;
					if(pmtn) {
						if(!mixer_) mixer_ = new MotionMixer;
						mixer_->add(pmtn);
					}
					else {
						pbody = *i;
						if(pbody) setUp(pbody);
					}
				}
			}
		}
	}

	void Entity::enter(Stage &stage)
	{
		enter(stage, 0);
	}

	void Entity::enter(Stage &stage, int16_t update_pri)
	{
		update_slot.setPriority(update_pri);
		stage.update_signal.connectOnce(update_slot);
		if(target_) {
			stage.body_list.push_back(target_);
			stage.strutum_tree.root()->push(*target_);
		}
	}

	void Entity::leave(Stage &stage)
	{
		stage.update_signal.disconnect(update_slot);
		if(target_) {
			stage.body_list.erase(remove(stage.body_list.begin(), stage.body_list.end(), target_), stage.body_list.end());
			stage.strutum_tree.root()->erase(target_->root());
		}
	}

	bool Entity::onUpdate(float delta)
	{
		return doOnUpdate(delta);
	}

	bool Entity::doOnUpdate(float delta)
	{
		if(mixer_) {
			mixer_->update(delta);
			if(target_) mixer_->apply(*target_);
		}
		return true;
	}

	bool Entity::setUp(luapp::Stack &L)
	{
		// Stage:newNodeで製作する
		return false;
	}

	void Entity::load(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<Context> _context = tuki_cast<Context>(L[1]);
			if(_context) {
				if(L.top() >= 2) {
#ifdef UNICODE
					WCStr srcfilename(L[2].toCStr());
					_context->load(srcfilename.c_str());
					setUp(srcfilename.c_str());
#else
					const char *srcfilename = L[2].toCStr();
					_context->load(srcfilename);
					setUp(srcfilename);
#endif
				}
			}
			else {
#ifdef UNICODE
				WCStr srcfilename(L[1].toCStr());
				context().load(srcfilename.c_str());
				setUp(srcfilename.c_str());
#else
				const char *srcfilename = L[1].toCStr();
				context().load(srcfilename);
				setUp(srcfilename);
#endif
			}
		}
	}

	void Entity::enter(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<Stage> stage = tuki_cast<Stage>(L[1]);
			if(stage) {
				if(L.top() >= 2) enter(*stage, L[2].toInteger());
				else enter(*stage);
			}
		}
	}

	void Entity::leave(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<Stage> stage = tuki_cast<Stage>(L[1]);
			if(stage) leave(*stage);
		}
	}

	void Entity::setPosition(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			if(target_ && *target_) (*target_)->val.getLCM().setPos(VectorC((float)L[1].toNumber(),(float)L[1].toNumber(),(float)L[1].toNumber()));
		}
	}

	int Entity::getPosition(luapp::Stack &L)
	{
		if(target_ && *target_) {
			Vector v = (*target_)->val.lcm().position();
			L << v.x << v.y << v.z;
			return 3;
		}
		return 0;
	}

	void Entity::setPosture(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			if(target_ && *target_) {
				Coord c = (*target_)->val.lcm();
				c.posture = QuatC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber());
				(*target_)->val.getLCM() = c.toMatrix();
			}
		}
	}

	int Entity::getPosture(luapp::Stack &L)
	{
		if(target_ && *target_) {
			Coord c = (*target_)->val.lcm();
			L << c.posture.yaw() << c.posture.pitch() << c.posture.roll();
			return 3;
		}
		return 0;
	}

	void Entity::setScale(luapp::Stack &L)
	{
		if(target_ && *target_) {
			if(L.top() >= 3) {
				Coord c = (*target_)->val.lcm();
				c.scale.x = (float)L[1].toNumber();
				c.scale.y = (float)L[2].toNumber();
				c.scale.z = (float)L[3].toNumber();
				(*target_)->val.getLCM() = c.toMatrix();
			}
			else if(L.top() >= 1) {
				Coord c = (*target_)->val.lcm();
				c.scale.x = c.scale.y = c.scale.z = (float)L[1].toNumber();
				(*target_)->val.getLCM() = c.toMatrix();
			}
		}
	}

	int Entity::getScale(luapp::Stack &L)
	{
		if(target_ && *target_) {
			Vector v = (*target_)->val.lcm().getScale();
			L << v.x << v.y << v.z;
			return 3;
		}
		return 0;
	}

	int Entity::getMotionMixer(luapp::Stack &L)
	{
		if(mixer_) {
			TukiRegister::newUserData(L, Hndl(mixer_.get()));
			return 1;
		}
		return 0;
	}

	GCTP_IMPLEMENT_CLASS_NS(gctp, Entity, Object);
	TUKI_IMPLEMENT_BEGIN_NS(Scene, Entity)
		TUKI_METHOD(Entity, load)
		TUKI_METHOD(Entity, enter)
		TUKI_METHOD(Entity, leave)
		TUKI_METHOD(Entity, setPosition)
		TUKI_METHOD(Entity, getPosition)
		TUKI_METHOD(Entity, setPosture)
		TUKI_METHOD(Entity, getPosture)
		TUKI_METHOD(Entity, setScale)
		TUKI_METHOD(Entity, getScale)
		TUKI_METHOD(Entity, getMotionMixer)
	TUKI_IMPLEMENT_END(Entity)
	
	Handle<Entity> newEntity(Context &context, Stage &stage, const char *classname, const _TCHAR *name, const _TCHAR *srcfilename)
	{
		if(srcfilename) {
			if(!context.load(srcfilename)) return Handle<Entity>();
		}
		Pointer<Entity> ret = context.create(classname, name).lock();
		if(ret) {
			if(srcfilename) ret->setUp(srcfilename);
			ret->enter(stage);
		}
		return ret;
	}

	Handle<Entity> newEntity(Context &context, Stage &stage, const GCTP_TYPEINFO &typeinfo, const _TCHAR *name, const _TCHAR *srcfilename)
	{
		if(srcfilename) context.load(srcfilename);
		Pointer<Entity> ret = context.create(typeinfo, name).lock();
		if(ret) {
			if(srcfilename) ret->setUp(srcfilename);
			ret->enter(stage);
		}
		return ret.get();
	}

}} // namespace gctp::scene
