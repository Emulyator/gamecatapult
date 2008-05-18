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
#include <gctp/app.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, World, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, World)
		TUKI_METHOD(World, load)
		TUKI_METHOD(World, activate)
	TUKI_IMPLEMENT_END(World)

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
						body_list.push_back(pbody);
						strutum_tree.root()->push(*pbody);
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
			postupdate_signal(delta);
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
		graphic::setAmbient(graphic::getAmbient());
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

	bool World::setUp(luapp::Stack &L)
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

}} // namespace gctp::scene
