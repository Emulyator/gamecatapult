#ifndef _GCTP_SCENE_RENDERER_HPP_
#define _GCTP_SCENE_RENDERER_HPP_
/** @file
 * GameCatapult �����_�����O�v�f�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/object.hpp>
#include <gctp/class.hpp>

namespace gctp { namespace scene {

	/** �`�揈���N���X
	 *
	 */
	class Renderer : public Object
	{
	public:
		/// �R���X�g���N�^
		Renderer() {}

		/// �m�[�h���B���̏���(�Ԃ�l�́A�q�����s���邩�ۂ�)
		virtual bool onReach(float /*delta*/) const { return true; }
		/// �m�[�h���E���̏���(�Ԃ�l�́A���s�𑱍s���邩�ۂ�)
		virtual bool onLeave(float /*delta*/) const { return true; }

	GCTP_DEFINE_TYPEINFO(Renderer, Object)
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_RENDERER_HPP_