#ifndef _GCTP_SCENE_UPDATER_HPP_
#define _GCTP_SCENE_UPDATER_HPP_
/** @file
 * GameCatapult �X�V�Ҋ��N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/signal.hpp>
#include <gctp/tuki.hpp>

namespace gctp { namespace scene {

	/** �X�V�Ҋ��N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/16 1:06:15
	 */
	class Updater : public Object
	{
	public:
		/// �R���X�g���N�^
		Updater(int16_t priority = 0, int16_t mask = 0xFF);

		/// �X�V
		bool update(float delta);
		/// �X�V�X���b�g
		MemberSlot1<Updater, float /*delta*/, &Updater::update> update_slot;

		/// App��update�V�O�i���ɘA��
		virtual void activate(bool yes);

	protected:
		virtual bool doUpdate(float delta) = 0;

		bool setUp(luapp::Stack &L);
		void activate(luapp::Stack &L);
		void setPriority(luapp::Stack &L);

		GCTP_DECLARE_TYPEINFO;
		TUKI_DECLARE(Updater);
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_UPDATER_HPP_