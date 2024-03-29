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
#include <gctp/scene/world.hpp>
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
		source_ = src;
		target_ = src->dup();
		if(target_ && target_->getName(*target_->root())) {
			// ルートに名前がある場合、モーションが適用される可能性があるので、
			// 下駄を履かせる
			Body::NodePtr n = target_->root();
			target_->setUp();
			target_->root()->push(n);
		}
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
				if(!target_) { // メッシュのみの場合
					for(GraphFile::iterator i = file->begin(); i != file->end(); ++i) {
						Pointer<graphic::Model> pmodel;
						pmodel = *i;
						if(pmodel) {
							if(!target_) {
								target_ = new Body;
								target_->setUp(0);
								source_ = target_;
							}
							Pointer<Flesh> flesh = new Flesh;
							if(flesh) {
								flesh->setUp(pmodel, target_, target_->root());
								target_->fleshies().push_back(flesh);
							}
						}
					}
				}
			}
		}
	}

	void Entity::enter(World &world)
	{
		enter(world, 0);
	}

	void Entity::enter(World &world, int16_t update_pri)
	{
		update_slot.setPriority(update_pri);
		world.update_signal.connectOnce(update_slot);
		if(target_) {
			world.body_list.push_back(target_);
			world.strutum_tree.root()->push(target_->root());
		}
	}

	void Entity::exit(World &world)
	{
		world.update_signal.disconnect(update_slot);
		if(target_) {
			world.body_list.erase(remove(world.body_list.begin(), world.body_list.end(), target_), world.body_list.end());
			world.strutum_tree.root()->erase(target_->root().get());
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
			if(target_ && source_) mixer_->apply(*target_, *source_);
		}
		return true;
	}

	bool Entity::LuaCtor(luapp::Stack &L)
	{
		// World:newNodeで製作する
		return false;
	}

	void Entity::load(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<core::Context> _context = tuki_cast<core::Context>(L[1]);
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
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) {
				if(L.top() >= 2) enter(*world, L[2].toInteger());
				else enter(*world);
			}
		}
	}

	void Entity::exit(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) exit(*world);
		}
	}

	void Entity::setPosition(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			if(target_ && target_->root()) target_->root()->val.getLCM().setPos(VectorC((float)L[1].toNumber(),(float)L[2].toNumber(),(float)L[3].toNumber()));
		}
	}

	int Entity::getPosition(luapp::Stack &L)
	{
		if(target_ && target_->root()) {
			Vector v = target_->root()->val.lcm().position();
			L << v.x << v.y << v.z;
			return 3;
		}
		return 0;
	}

	void Entity::setPosture(luapp::Stack &L)
	{
		if(target_ && target_->root()) {
			Coord c = target_->root()->val.lcm();
			if(L.top() == 1) {
				c.posture = QuatC((float)L[1].toNumber(), 0.0f, 0.0f);
			}
			else if(L.top() == 3) {
				c.posture = QuatC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber());
			}
			else if(L.top() >= 4) {
				c.posture = QuatC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber(), (float)L[4].toNumber());
			}
			target_->root()->val.getLCM() = c.toMatrix();
		}
	}

	int Entity::getPosture(luapp::Stack &L)
	{
		if(target_ && target_->root()) {
			Coord c = target_->root()->val.lcm();
			L << c.posture.w << c.posture.x << c.posture.y << c.posture.z;
			return 4;
		}
		return 0;
	}

	void Entity::setScale(luapp::Stack &L)
	{
		if(target_ && target_->root()) {
			if(L.top() == 1) {
				Coord c = target_->root()->val.lcm();
				c.scale.x = c.scale.y = c.scale.z = (float)L[1].toNumber();
				target_->root()->val.getLCM() = c.toMatrix();
			}
			else if(L.top() >= 3) {
				Coord c = target_->root()->val.lcm();
				c.scale.x = (float)L[1].toNumber();
				c.scale.y = (float)L[2].toNumber();
				c.scale.z = (float)L[3].toNumber();
				target_->root()->val.getLCM() = c.toMatrix();
			}
		}
	}

	int Entity::getScale(luapp::Stack &L)
	{
		if(target_ && target_->root()) {
			Vector v = target_->root()->val.lcm().getScale();
			L << v.x << v.y << v.z;
			return 3;
		}
		return 0;
	}

	int Entity::getDirection(luapp::Stack &L)
	{
		if(target_ && target_->root()) {
			Vector dir = target_->root()->val.wtm().forward();
			L << dir.x << dir.y << dir.z;
			return 3;
		}
		return 0;
	}

	int Entity::getMotionMixer(luapp::Stack &L)
	{
		if(mixer_) {
			TukiRegister::push(L, Hndl(mixer_.get()));
			return 1;
		}
		return 0;
	}

	int Entity::getStrutumNode(luapp::Stack &L)
	{
		if(target_) {
			if(L.top()>=1) {
				StrutumNode *node = target_->get(L[1].toCStr());
				if(node) {
					TukiRegister::push(L, Hndl(node));
					return 1;
				}
			}
			else {
				if(target_->root()) {
					TukiRegister::push(L, Hndl(target_->root()));
					return 1;
				}
			}
		}
		return 0;
	}
	
	int Entity::getFlesh(luapp::Stack &L)
	{
		if(target_) {
			if(L.top() >= 1) {
				if(L[1].isNumber()) {
					int modelidx = L[1].toInteger();
					int j = 0;
					for(PointerList<Flesh>::iterator i = target_->fleshies().begin(); i != target_->fleshies().end(); ++i) {
						if(j == modelidx) {
							TukiRegister::push(L, (*i));
							return 1;
						}
						j++;
					}
				}
				else {
					const char *modelname = L[1].toCStr();
					if(modelname) {
						for(PointerList<Flesh>::iterator i = target_->fleshies().begin(); i != target_->fleshies().end(); ++i) {
							/*if((*i)->model() && (*i)->model()->name()) dbgout << "MODEL NAME : " << (*i)->model()->name() << endl;
							else dbgout << "MODEL NAME : NONAME" << endl;*/

							if((*i)->model() && (*i)->model()->name() && (strcmp((*i)->model()->name(), modelname)==0)) {
								TukiRegister::push(L, (*i));
								return 1;
							}
						}
					}
				}
			}
			else {
				TukiRegister::push(L, Hndl(target_->fleshies().front().get()));
				return 1;
			}
		}
		return 0;
	}
	
	int Entity::getBoundingSphere(luapp::Stack &L)
	{
		if(target_) {
			L << target_->bs().c.x << target_->bs().c.y << target_->bs().c.z << target_->bs().r;
			return 4;
		}
		return 0;
	}

	int Entity::getModelAABB(luapp::Stack &L)
	{
		AABox aabb = source()->fleshies().front()->model()->getAABB(source()->fleshies().front()->node()->val.wtm());
		//dbgout << "aabb : " << source()->fleshies().front()->node()->val.wtm() << aabb.upper << aabb.lower << endl;
		for(PointerList<Flesh>::iterator i = ++source()->fleshies().begin(); i != source()->fleshies().end(); ++i)
		{
			AABox aabb2 = (*i)->model()->getAABB((*i)->node()->val.wtm());
			//dbgout << "aabb : " << (*i)->node()->val.wtm() << aabb2.upper << aabb2.lower << endl;
			aabb |= aabb2;
		}
		L << aabb.upper.x << aabb.upper.y << aabb.upper.z << aabb.lower.x << aabb.lower.y << aabb.lower.z;
		return 6;
	}
	
	int Entity::printHierarchy(luapp::Stack &L)
	{
		if(L[1].toBoolean()) {
			if(target_) target_->print(gctp::dbgout);
		}
		else if(source_) source_->printLCM(gctp::dbgout);
		return 0;
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, Entity, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, Entity)
		TUKI_METHOD(Entity, load)
		TUKI_METHOD(Entity, enter)
		TUKI_METHOD(Entity, exit)
		TUKI_METHOD(Entity, setPosition)
		TUKI_METHOD(Entity, getPosition)
		TUKI_METHOD(Entity, setPosture)
		TUKI_METHOD(Entity, getPosture)
		TUKI_METHOD(Entity, setScale)
		TUKI_METHOD(Entity, getScale)
		TUKI_METHOD(Entity, getDirection)
		TUKI_METHOD(Entity, getMotionMixer)
		TUKI_METHOD(Entity, getStrutumNode)
		TUKI_METHOD(Entity, getFlesh)
		TUKI_METHOD(Entity, getBoundingSphere)
		TUKI_METHOD(Entity, getModelAABB)
		TUKI_METHOD(Entity, printHierarchy)
	TUKI_IMPLEMENT_END(Entity)
	
	Handle<Entity> newEntity(core::Context &context, World &world, const char *classname, const _TCHAR *name, const _TCHAR *srcfilename)
	{
		if(srcfilename) {
			if(!context.load(srcfilename)) return Handle<Entity>();
		}
		Pointer<Entity> ret = context.create(classname, name).lock();
		if(ret) {
			if(srcfilename) ret->setUp(srcfilename);
			ret->enter(world);
		}
		return ret;
	}

	Handle<Entity> newEntity(core::Context &context, World &world, const GCTP_TYPEINFO &typeinfo, const _TCHAR *name, const _TCHAR *srcfilename)
	{
		if(srcfilename) context.load(srcfilename);
		Pointer<Entity> ret = context.create(typeinfo, name).lock();
		if(ret) {
			if(srcfilename) ret->setUp(srcfilename);
			ret->enter(world);
		}
		return ret.get();
	}

}} // namespace gctp::scene
