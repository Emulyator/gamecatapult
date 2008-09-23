#ifndef _GCTP_SCENE_WORLDRENDERER_HPP_
#define _GCTP_SCENE_WORLDRENDERER_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �ȈՌ��ʃm�[�h�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/tuki.hpp>
#include <gctp/handlelist.hpp>
#include <gctp/scene/renderer.hpp>
#include <gctp/scene/drawpacket.hpp>

namespace gctp {
	namespace graphic {
		namespace dx {
			class StateBlockRsrc;
		}
	}
}

namespace gctp { namespace scene {

	class World;
	class Camera;
	class Flesh;
	
	/** �\�[�g�ςݕ`��p�P�b�g��ێ�����m�[�h�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class WorldSorter : public Renderer
	{
	public:
		WorldSorter();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		/// �`��Ώېݒ�
		void attach(Handle<World> target) { target_ = target; }
		/// ���[���h�擾
		Handle<World> target() const { return target_; }

		mutable DrawPacketVector packets;
	
	protected:
		Handle<World> target_;

		bool setUp(luapp::Stack &L);
		void attach(luapp::Stack &L);

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(WorldSorter);
	};

	/** �ȈՌ��ʃm�[�h�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class WorldRenderer : public WorldSorter
	{
	public:
		WorldRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;
	
	private:
		Pointer<graphic::dx::StateBlockRsrc> sb_;

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(WorldRenderer);
	};

	/** �s�����̕`����ʃm�[�h�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class OpequeWorldRenderer : public Renderer
	{
	public:
		OpequeWorldRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		void attach(Handle<WorldSorter> target)
		{
			target_ = target;
		}

	protected:
		Handle<WorldSorter> target_;

		bool setUp(luapp::Stack &L);
		void attach(luapp::Stack &L);
	
	private:
		Pointer<graphic::dx::StateBlockRsrc> sb_;

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(OpequeWorldRenderer);
	};

	/** �������̕`��m�[�h�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 1:05:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class TranslucentWorldRenderer : public Renderer
	{
	public:
		TranslucentWorldRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		void attach(Handle<WorldSorter> target)
		{
			target_ = target;
		}

	protected:
		Handle<WorldSorter> target_;

		bool setUp(luapp::Stack &L);
		void attach(luapp::Stack &L);
	
	private:
		Pointer<graphic::dx::StateBlockRsrc> sb_;

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(TranslucentWorldRenderer);
	};

	/** Z�t�B���p
	 *
	 * �\�[�g���Ȃ��łƂɂ�������
	 */
	class ZFillRenderer : public Renderer
	{
	public:
		ZFillRenderer();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		/// �`��Ώےǉ�
		void add(Handle<World> world);
		/// �`��Ώۍ폜
		void remove(Handle<World> world);
	
	protected:
		HandleList<World> worlds_;
		mutable const char *backup_tech_;

		bool setUp(luapp::Stack &L);
		void add(luapp::Stack &L);
		void remove(luapp::Stack &L);

	private:
		Pointer<graphic::dx::StateBlockRsrc> sb_;

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(ZFillRenderer);
	};

	/** �����_�����O�m�[�h�̃t���X�^���J�����O�p
	 *
	 */
	class IsVisibleOperator : public Renderer
	{
	public:
		virtual bool onReach(float delta) const;

		/// �Ώۂ̃J�������w��
		void set(Handle<Camera> camera);
		/// �`�F�b�N�Ώےǉ�
		void add(Handle<Flesh> flesh);
		/// �`�F�b�N�Ώۍ폜
		void remove(Handle<Flesh> flesh);
	
	protected:
		Handle<Camera> camera_;
		HandleList<Flesh> fleshies_;

		bool setUp(luapp::Stack &L);
		void set(luapp::Stack &L);
		void add(luapp::Stack &L);
		void remove(luapp::Stack &L);

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(IsVisibleOperator);
	};

}} // namespace gctp::scene

#endif // _GCTP_SCENE_WORLDRENDERER_HPP_