/** @file
 * GameCatapult ���[���h�N���X
 *
 * update_signal(��Ԃ̍X�V),animate_signal(�X�V�������ŃA�j���[�V�����X�V�i��ʂ���啝�ɊO�ꂽ����s����Ȃ��B�s��c���[�ɐώZ�����~�ߐݒ�j),
 * predraw_signal(�`����ԂłȂ��A���[���h���W�n�����ɏ����ł���`�����X�B�Փˉ����͂����Ɉړ�),draw_signal
 * �̎l�ɕ�����ׂ�����
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/world.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/stateblock.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/context.hpp>
#include <gctp/scene/motion.hpp>
#include <gctp/scene/graphfile.hpp>
#include <gctp/scene/camera.hpp>
#include <gctp/scene/body.hpp>
#include <gctp/scene/light.hpp>
#include <gctp/scene/speaker.hpp>
#include <gctp/app.hpp>

#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, World, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, World)
		TUKI_METHOD(World, load)
		TUKI_METHOD(World, activate)
		TUKI_METHOD(World, setPosition)
		TUKI_METHOD(World, getPosition)
		TUKI_METHOD(World, setPosture)
		TUKI_METHOD(World, getPosture)
		TUKI_METHOD(World, getStrutumNode)
		TUKI_METHOD(World, getBoundingSphere)
		TUKI_METHOD(World, printHierarchy)
	TUKI_IMPLEMENT_END(World);

	World* World::current_ = NULL;	///< �J�����g�X�e�[�W�i���̃X�e�[�W��update�Adraw�c�Ȃǂ̊Ԃ����L���j

	World::World()
	{
		update_slot.bind(this);
		strutum_tree.setUp();
	}

	void World::setUp(const _TCHAR *filename)
	{
		// XFile����LightNode��Camera��Entity���Z�b�g�A�b�v����
		// �Ȃ񂾂��ǁA�W����XFile�ł́Axfile=Entity�ɂ��邵���Ȃ���Ȃ�(Animation���K���P�c���[�ɂȂ����Ⴄ����j
		// gctp�g��������ꍇ�̂ݏ�肭�����A�Ƃ��邩
		// �Ƃ肠�����A�����q�G�����L�[�ւ̑Ή����P�c���[�ւ̍����ōs���A�Ƃ����̂�World��XFile���󂯕t������悤�ɂȂ��
		// �s�v�Ȃ̂Ŏ~�߂悤
		// ���̑���AEntity��XFile��n����1�c���[��v������A��������ƂQ�ڂ͖����A�Ƃ����ލs�d�l��
		if(filename) {
			Pointer<GraphFile> file = context()[filename].lock();
			if(file) {
				Pointer<Light> plight;
				Pointer<Body> pbody;
				for(GraphFile::iterator i = file->begin(); i != file->end(); ++i) {
					pbody = *i;
					if(pbody) {
						world_body_ = pbody;
						body_list.push_back(pbody);
						strutum_tree.root()->push(pbody->root());
					}
					plight = *i;
					if(plight) {
					}
				}
			}
		}
	}

	/** �V�[���X�V
	 *
	 * �X�V�V�O�i���ƏՓˉ����V�O�i�����s
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool World::onUpdate(float delta)
	{
#ifdef _MT
		ScopedLock al(monitor_);
#endif
		return doOnUpdate(delta);
	}

	bool World::doOnUpdate(float delta)
	{
		begin();
			update_signal(delta);
			strutum_tree.setTransform();

			for(HandleList<Body>::iterator i = body_list.begin(); i != body_list.end(); ++i) {
				Pointer<Body> body = i->lock();
				if(body) body->update();
			}

			for(HandleList<Light>::iterator i = light_list.begin(); i != light_list.end(); ++i) (*i)->update();
			struct Pred {
				bool operator()(const Hndl &lhs, const Hndl &rhs) const
				{
					Handle<Light> l = lhs;
					Handle<Light> r = rhs;
					if(l && r) return (int)l->type < (int)r->type;
					else if(l) return true;
					return false;
				}
			};
			light_list.sort(Pred());

			postupdate_signal(delta);
			//strutum_tree.resync();
		end();
		return true;
	}

	/** �`��E�X�V�J�n
	 *
	 * ���b�N���ăJ�����g�ɂ���B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	void World::begin() const
	{
#ifdef _MT
		monitor_.lock();
#endif
		backup_current_ = const_cast<World *>(current_);
		current_ = const_cast<World *>(this);
#ifndef __ee__
		graphic::clearLight();
#endif
	}

	/** �`��E�X�V�I��
	 *
	 * �A�����b�N���ăJ�����g����͂����B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	void World::end() const
	{
		current_ = backup_current_;
#ifdef _MT
		monitor_.unlock();
#endif
	}
	
	void World::draw() const
	{
		GCTP_ASSERT(this == current_);
		World *self = const_cast<World *>(this);
		for(HandleList<Body>::iterator i = self->body_list.begin(); i != self->body_list.end();) {
			if(*i) {
				(*i)->draw(); // �p�X�Ɋւ�����������ő���ׂ��H
				++i;
			}
			else i = self->body_list.erase(i);
		}
	}
	
	void World::applyLights(const Sphere &bs, int max_area_num, int max_point_num, int max_spot_num) const
	{
		HandleVector<Light> area_lights, point_lights, spot_lights;
		area_lights.resize(max_area_num);
		point_lights.resize(max_point_num);
		spot_lights.resize(max_spot_num);
		selectLights(bs, area_lights, point_lights, spot_lights);
		graphic::clearLight();
		for(HandleVector<Light>::iterator i = area_lights.begin(); i != area_lights.end() && *i; ++i) (*i)->apply();
		for(HandleVector<Light>::iterator i = point_lights.begin(); i != point_lights.end() && *i; ++i) (*i)->apply();
		for(HandleVector<Light>::iterator i = spot_lights.begin(); i != spot_lights.end() && *i; ++i) (*i)->apply();
	}

	void World::selectLights(const Sphere &bs, HandleVector<Light> &area_lights, HandleVector<Light> &point_lights, HandleVector<Light> &spot_lights) const
	{
		struct Pred {
			Pred(const Vector &pos) : pos(pos) {}
			const Vector &pos;
			bool operator()(const Handle<Light> &lhs, const Handle<Light> &rhs) const
			{
				if(lhs && rhs) {
					Handle<StrutumNode> l = lhs->node();
					Handle<StrutumNode> r = rhs->node();
					if(l && r) return gctp::distance(pos, l->val.wtm().position()) < gctp::distance(pos, r->val.wtm().position());
					else if(l) return true;
					return false;
				}
				else if(lhs) return true;
				return false;
			}
		} pred(bs.c);

		HandleList<Light>::const_iterator l = light_list.begin();
		size_t i = 0;
		if(area_lights.size() > 0) {
			for(; l != light_list.end() && (*l)->type == Light::AREA; ++l) {
				if(bs.isColliding((*l)->bs())) {
					if(i < area_lights.size()) {
						area_lights[i] = *l;
						if(i > 0) sort(area_lights.begin(), area_lights.end(), pred);
						i++;
					}
					else {
						if(!pred(area_lights[i-1], *l)) {
							area_lights[i-1] = *l;
							if(i > 0) sort(area_lights.begin(), area_lights.end(), pred);
						}
					}
				}
			}
		}
		for(; l != light_list.end() && (*l)->type == Light::AREA; ++l) {}

		i = 0;
		if(point_lights.size() > 0) {
			for(;  l != light_list.end() && (*l)->type == Light::POINT; ++l) {
				if(bs.isColliding((*l)->bs())) {
					if(i < point_lights.size()) {
						point_lights[i] = *l;
						if(i > 0) sort(point_lights.begin(), point_lights.end(), pred);
						i++;
					}
					else {
						if(!pred(point_lights[i-1], *l)) {
							point_lights[i-1] = *l;
							if(i > 0) sort(point_lights.begin(), point_lights.end(), pred);
						}
					}
				}
			}
		}
		for(; l != light_list.end() && (*l)->type != Light::POINT; ++l) {}

		i = 0;
		if(spot_lights.size() > 0) {
			for(; l != light_list.end() && (*l)->type == Light::SPOT; ++l) {
				if(bs.isColliding((*l)->bs())) {
					if(i < spot_lights.size()) {
						spot_lights[i] = *l;
						if(i > 0) sort(spot_lights.begin(), spot_lights.end(), pred);
						i++;
					}
					else {
						if(!pred(spot_lights[i-1], *l)) {
							spot_lights[i-1] = *l;
							if(i > 0) sort(spot_lights.begin(), spot_lights.end(), pred);
						}
					}
				}
			}
		}
	}

	bool World::LuaCtor(luapp::Stack &L)
	{
		// Context:create�Ő��삷��
		return false;
	}

	void World::load(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			if(L[1].isString()) {
#ifdef UNICODE
				setUp(WCStr(L[1].toCStr()).c_str());
#else
				setUp(L[1].toCStr());
#endif
			}
		}
	}

	void World::activate(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			if(L[1].toBoolean()) app().update_signal.connectOnce(update_slot);
			else app().update_signal.disconnect(update_slot);
		}
		else app().update_signal.connectOnce(update_slot);
	}
	
	void World::setPosition(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			if(strutum_tree.root()) strutum_tree.root()->val.getLCM().setPos(VectorC((float)L[1].toNumber(),(float)L[1].toNumber(),(float)L[1].toNumber()));
		}
	}

	int World::getPosition(luapp::Stack &L)
	{
		if(strutum_tree.root()) {
			Vector v = strutum_tree.root()->val.lcm().position();
			L << v.x << v.y << v.z;
			return 3;
		}
		return 0;
	}

	void World::setPosture(luapp::Stack &L)
	{
		if(world_body_ && world_body_->root()) {
			Coord c = world_body_->root()->val.lcm();
			if(L.top() == 1) {
				c.posture = QuatC((float)L[1].toNumber(), 0.0f, 0.0f);
			}
			else if(L.top() == 3) {
				c.posture = QuatC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber());
			}
			else if(L.top() >= 4) {
				c.posture = QuatC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber(), (float)L[4].toNumber());
			}
			world_body_->root()->val.getLCM() = c.toMatrix();
		}
	}

	int World::getPosture(luapp::Stack &L)
	{
		if(world_body_ && world_body_->root()) {
			Coord c = world_body_->root()->val.lcm();
			L << c.posture.w << c.posture.x << c.posture.y << c.posture.z;
			return 4;
		}
		return 0;
	}

	int World::getStrutumNode(luapp::Stack &L)
	{
		if(world_body_) {
			if(L.top()>=1) {
				StrutumNode *node = world_body_->get(L[1].toCStr());
				if(node) {
					TukiRegister::push(L, Hndl(node));
					return 1;
				}
			}
			else {
				if(world_body_->root()) {
					TukiRegister::push(L, Hndl(world_body_->root()));
					return 1;
				}
			}
		}
		return 0;
	}

	int World::getBoundingSphere(luapp::Stack &L)
	{
		if(world_body_) {
			L << world_body_->bs().c.x << world_body_->bs().c.y << world_body_->bs().c.z << world_body_->bs().r;
			return 4;
		}
		return 0;
	}

	int World::printHierarchy(luapp::Stack &L)
	{
		if(world_body_) world_body_->printLCM(gctp::dbgout);
		return 0;
	}
	
}} // namespace gctp::scene
