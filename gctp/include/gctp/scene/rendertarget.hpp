#ifndef _GCTP_SCENE_RENDERTARGET_HPP_
#define _GCTP_SCENE_RENDERTARGET_HPP_
/** @file
 * GameCatapult �����_�[�^�[�Q�b�g�����_���[�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/types.hpp>
#include <gctp/strutumnode.hpp>
#include <gctp/frustum.hpp>
#include <gctp/tuki.hpp>
#include <gctp/scene/renderer.hpp>

namespace gctp { namespace graphic {

	class Texture;
	class RenderTarget;
	class DepthStencil;

}}

namespace gctp { namespace scene {

	/** �����_�[�^�[�Q�b�g�����_���[
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 8:10:04
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class RenderTarget : public Renderer
	{
	public:
		/// �R���X�g���N�^
		RenderTarget();

		virtual bool onReach(float delta) const;
		virtual bool onLeave(float delta) const;

		/// �����_�[�^�[�Q�b�g�ݒ�J�n
		void begin() const;
		/// �����_�[�^�[�Q�b�g�ݒ�I��
		void end() const;

	protected:
		bool LuaCtor(luapp::Stack &L);
		void setUp(luapp::Stack &L);
		int texture(luapp::Stack &L);
		static int setUpRT(luapp::Stack &L);
		static int setUpDS(luapp::Stack &L);

	private:
		mutable Handle<graphic::RenderTarget> rt_;
		mutable Handle<graphic::DepthStencil> ds_;

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(RenderTarget);
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_RENDERTARGET_HPP_