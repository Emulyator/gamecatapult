#ifndef _GCTP_APP_HPP_
#define _GCTP_APP_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �A�v���P�[�V�����N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 * @date 2001/8/6
 */

#include <deque>
#include <gctp/fpsinfo.hpp>
#include <gctp/profiler.hpp>
#include <gctp/signal.hpp>
#include <gctp/types.hpp>
#ifdef _MT
#include <boost/thread/mutex.hpp>
#endif

namespace gctp {

	/*struct UUID {
		uint32_t	Data1;       // 00000000-
		uint16_t	Data2;       // 0000-
		uint16_t	Data3;       // 0000-
		uint8_t		Data4[8];    // C000-000000000046
	};*/

	/** ���̓C�x���g
	 *
	 * �L���[�̂ق��ł�����|�C���^�[�⑫���A�|�����[�t�\�Ȃ悤�ɂ���ƁA�������f�Љ��ւ̑Ώ�������Ȃ�
	 *
	 * �_����A�g�p�������ʂ��Œ�ł��邱�Ƃ̂ق����d�v�ł���Ɣ��f�
	 *�i�|�����[�t�ŁE��|�����[�t�łƂŃC���^�[�t�F�[�X���ꏏ�ɂ͏o���Ȃ��A�͂��B�؂�ւ��\�͂�����߂��ق��������j
	 *
	 * �������ATTR_SIZE�o�C�g������ꍇ�i�l�b�g���[�N���b�Z�[�W�Ƃ��j�́A�����o�b�t�@�Ƀ|�C���^���܂߂Ă��ꂪ
	 * �l�b�g���[�N���b�Z�[�W�������A�őΏ��B�̈����́Atype���Ƃɖ����I�ɂ���K�v����
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/24 1:02:41
	 */
	struct Event {
		enum {
			ATTR_SIZE = 24,
		};
		GUID guid; // UUID�̂ق����������Ȃ�
		char attr[ATTR_SIZE];
	};

	/** ���̓C�x���g�L���[�N���X
	 *
	 * �Q�[���X���b�h�ŏ����ł���悤�ɁAUI�X���b�h�i���C���X���b�h�j�Ŕ�������
	 * �������̓��̓C�x���g�𗭂ߍ��ރL���[�B
	 *
	 * �}�E�X�A�b�v�E�}�E�X�_�E���E�}�E�X�h���b�O�E�J�[�\�����[�u�E�L�[�A�b�v��L�[�_�E���Ȃǂ݈̂���
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/11/21 17:55:36
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Events : public Object {
	public:
		void post(const Event &event);
		void flush();
		/// �C�x���g�V�O�i��
		Signal1<true, const Event &> event_signal;
		/// �C�x���g�̉�̂���Ƃ��ɔ��s�����V�O�i���i�������ʂȏ������K�v�Ȃ�A����ɓo�^�j
		Signal1<true, Event &> erase_signal;
	private:
		typedef std::deque<Event> QType;
		QType q_;
#ifdef _MT
		boost::mutex monitor_;
#endif
	};

	/** GUI�n���̓C�x���g�L���[�N���X
	 *
	 * �Q�[���X���b�h�ŏ����ł���悤�ɁAUI�X���b�h�i���C���X���b�h�j�Ŕ�������
	 * �������̓��̓C�x���g�𗭂ߍ��ރL���[�B
	 *
	 * �}�E�X�A�b�v�E�}�E�X�_�E���E�}�E�X�h���b�O�E�J�[�\�����[�u�E�L�[�A�b�v��L�[�_�E���Ȃǂ݈̂���
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/11/21 17:55:36
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class GUIEvents : public Object {
	public:
		GUIEvents();

		enum Button {
			LB = 1,
			RB = 1<<1,
			MB = 1<<2,
			X1B = 1<<3,
			X2B = 1<<4,
			ALT = 1<<5,
			CTRL = 1<<6,
			SHIFT = 1<<7,
		};

		enum Wheel {
			MOUSE,
			PAD_X,
			PAD_Y,
		};

		const static GUID TYPEID;

		static void postDownMsg(Events &q, Point2 p, uint8_t button, uint8_t opt);
		static void postUpMsg(Events &q, Point2 p, uint8_t button, uint8_t opt);
		static void postDblClickMsg(Events &q, Point2 p, uint8_t button, uint8_t opt);
		static void postMoveMsg(Events &q, Point2 p, uint8_t button);
		static void postWheelMsg(Events &q, Point2 p, int16_t delta, uint8_t wheel, uint8_t opt);
		static void postKeyMsg(Events &q, int key);
		static void postCharMsg(Events &q, int ch);

		/// �{�^���������ꂽ��(X, Y, BUTTON, OPTBUTTON)
		Signal3<true, Point2, uint8_t, uint8_t>			down_signal;
		/// �{�^���������ꂽ��(X, Y, BUTTON, OPTBUTTON)
		Signal3<true, Point2, uint8_t, uint8_t>			up_signal;
		/// �_�u���N���b�N���ꂽ��(X, Y, BUTTON, OPTBUTTON)
		Signal3<true, Point2, uint8_t, uint8_t>			dblclick_signal;
		/// �|�C���^�̈ړ�(X, Y, OPTBUTTON)
		Signal2<true, Point2, uint8_t>					move_signal;
		/// �}�E�X�z�C�[����A�p�b�h�̃A�i�R������(X, Y, DELTA, WHEEL, OPT)
		Signal4<true, Point2, int16_t, uint8_t, uint8_t>	wheel_signal;
		/// �L�[���́i�v���b�g�z�[���ˑ��AWindows�Ȃ�VK_*������j
		Signal1<true, int>								key_signal;
		/// ��������
		Signal1<true, int>								char_signal;

		bool procEvent(const Event &event);
#ifndef BOOST_NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
		/** �C�x���g�����X���b�g
		 * �����Event::event_signal�ɐڑ�
		 */
		MemberSlot1<GUIEvents, const Event &, &GUIEvents::procEvent> event_slot;
#else
		/** �C�x���g�����X���b�g�ւ̃|�C���^
		 * �����Event::event_signal�ɐڑ�
		 */
		Pointer<Slot> event_slot;
#endif
	};

	/** �Q�[�����C�����N���X
	 *
	 * �h���N���X��present�ł́A���̃N���X��present���Ăяo���Ă��K�v������B
	 * 
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/14 5:54:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class GameApp {
		GCTP_TLS static GameApp *app_;
		static uint window_count_;
		Profiler profiler_even_;
		Profiler profiler_odd_;
		Profiler *draw_profile_;
		bool odd_frame_;
		bool present_;
		Events events_;
		GUIEvents guievents_;
	public:
		GameApp();
		virtual bool canContinue();
		virtual bool canDraw();
		virtual void present();
		virtual void showCursor(bool yes) = 0;
		virtual void holdCursor(bool yes) = 0;

		/// �I�[�v������Ă��錻�݂̃��[�g�v���t�@�C��
		Profiler &profiler() { return odd_frame_ ? profiler_odd_ : profiler_even_; }
		/// 1�t���[���O�̃v���t�@�C���i�N���[�Y�ς݁j
		Profiler &profile() { return odd_frame_ ? profiler_even_ : profiler_odd_; }
		/// �E�B���h�E�̐��i�A�v���P�[�V��������GameApp�̐��j
		uint window_count() { return window_count_; }

		/// �C�x���g�L���[
		Events &events() { return events_; }
		/// GUI�C�x���g�V�O�i��
		GUIEvents &guievents() { return guievents_; }
		/// �X�V�V�O�i��
		Signal1<false, float /*delta*/> update_signal;
		/// �`��V�O�i��
		Signal0<false> draw_signal;

		/// ���s
		int run();

		void incWindow();
		uint decWindow();

		friend GameApp &app();
		void setCurrent() { app_ = this; }
		float lap;
		float true_lap;
		FPSInfo fps;
	};

	inline GameApp &app() { return *GameApp::app_; }

	/** �X�R�[�v�ɂ��������ăv���t�@�C������N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:09:26
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Profiling
	{
	public:
		Profiling(const char *name, Profiler &_profiler = app().profiler().top()) : profiler_(_profiler.begin(name)) {}
		~Profiling() { end(); }
		void end() { profiler_.end(); }
	private:
		Profiler &profiler_;
	};

} //namespace gctp

#endif //_GCTP_APP_HPP_