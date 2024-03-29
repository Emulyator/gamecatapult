/** @file
 * GameCatapult アプリケーションクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: app.cpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/app.hpp>
#include <gctp/graphic.hpp>
#ifdef _MT
#include <gctp/mutex.hpp>
#endif

namespace gctp {

	void EventQue::post(const Event &event)
	{
#ifdef _MT
		ScopedLock _al(monitor_);
#endif
		q_.push_back(event);
	}

	void EventQue::flush()
	{
#ifdef _MT
		ScopedLock _al(monitor_);
#endif
		for(QType::iterator i = q_.begin(); i != q_.end(); ++i) {
			event_signal(*i);
			erase_signal(*i);
		}
		q_.clear();
	}

	namespace {
		
		struct GUIEvent : EventHeader {
			enum Message {
				DOWN,
				UP,
				DBLCLICK,
				MOVE,
				WHEEL,
				KEY,
				CHAR,
			};
			Message msg;
		};

		struct PointerEvent : GUIEvent{
			Point2  p;
			int16_t delta;
			uint8_t button;
			uint8_t opt;
		};

		struct KeyEvent : GUIEvent{
			int key;
		};

	}

#if 0
	GUIEvents::GUIEvents()
#ifndef BOOST_NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
		//: event_slot(this)
#else
		: event_slot(new MemberSlot1<GUIEvents, const Event &, &GUIEvents::procEvent>(this))
#endif
	{
	}
#else
	GUIEvents::GUIEvents()
	{
		event_slot.bind(this);
	}
#endif

	// {595BC867-DF86-4e22-B75D-4645B65093C7}
	const GUID GUIEvents::TYPEID = { 0x595bc867, 0xdf86, 0x4e22, { 0xb7, 0x5d, 0x46, 0x45, 0xb6, 0x50, 0x93, 0xc7 } };

	void GUIEvents::postDownMsg(EventQue &q, ulong time, Point2 p, uint8_t button, uint8_t opt)
	{
		Event event;
		PointerEvent *e = reinterpret_cast<PointerEvent *>(&event);
		e->time = time;
		e->guid = TYPEID;
		e->msg = GUIEvent::DOWN;
		e->p = p;
		e->button = button;
		e->opt = opt;
		q.post(event);
	}

	void GUIEvents::postUpMsg(EventQue &q, ulong time, Point2 p, uint8_t button, uint8_t opt)
	{
		Event event;
		PointerEvent *e = reinterpret_cast<PointerEvent *>(&event);
		e->time = time;
		e->guid = TYPEID;
		e->msg = GUIEvent::UP;
		e->p = p;
		e->button = button;
		e->opt = opt;
		q.post(event);
	}

	void GUIEvents::postDblClickMsg(EventQue &q, ulong time, Point2 p, uint8_t button, uint8_t opt)
	{
		Event event;
		PointerEvent *e = reinterpret_cast<PointerEvent *>(&event);
		e->time = time;
		e->guid = TYPEID;
		e->msg = GUIEvent::DBLCLICK;
		e->p = p;
		e->button = button;
		e->opt = opt;
		q.post(event);
	}

	void GUIEvents::postMoveMsg(EventQue &q, ulong time, Point2 p, uint8_t opt)
	{
		Event event;
		PointerEvent *e = reinterpret_cast<PointerEvent *>(&event);
		e->time = time;
		e->guid = TYPEID;
		e->msg = GUIEvent::MOVE;
		e->p = p;
		e->opt = opt;
		q.post(event);
	}

	void GUIEvents::postWheelMsg(EventQue &q, ulong time, Point2 p, int16_t delta, uint8_t wheel, uint8_t opt)
	{
		Event event;
		PointerEvent *e = reinterpret_cast<PointerEvent *>(&event);
		e->time = time;
		e->guid = TYPEID;
		e->msg = GUIEvent::WHEEL;
		e->p = p;
		e->delta = delta;
		e->button = wheel;
		e->opt = opt;
		q.post(event);
	}

	void GUIEvents::postKeyMsg(EventQue &q, ulong time, int key)
	{
		Event event;
		KeyEvent *e = reinterpret_cast<KeyEvent *>(&event);
		e->time = time;
		e->guid = TYPEID;
		e->msg = GUIEvent::KEY;
		e->key = key;
		q.post(event);
	}

	void GUIEvents::postCharMsg(EventQue &q, ulong time, int ch)
	{
		Event event;
		KeyEvent *e = reinterpret_cast<KeyEvent *>(&event);
		e->time = time;
		e->guid = TYPEID;
		e->msg = GUIEvent::CHAR;
		e->key = ch;
		q.post(event);
	}

	bool GUIEvents::procEvent(const Event &event)
	{
		if(event.guid == TYPEID) {
			const GUIEvent *e = reinterpret_cast<const GUIEvent *>(&event);
			switch(e->msg) {
			case GUIEvent::DOWN:
				{
					const PointerEvent *e = reinterpret_cast<const PointerEvent *>(&event);
					down_signal(e->time, e->p, e->button, e->opt);
				}
				break;
			case GUIEvent::UP:
				{
					const PointerEvent *e = reinterpret_cast<const PointerEvent *>(&event);
					up_signal(e->time, e->p, e->button, e->opt);
				}
				break;
			case GUIEvent::DBLCLICK:
				{
					const PointerEvent *e = reinterpret_cast<const PointerEvent *>(&event);
					dblclick_signal(e->time, e->p, e->button, e->opt);
				}
				break;
			case GUIEvent::MOVE:
				{
					const PointerEvent *e = reinterpret_cast<const PointerEvent *>(&event);
					move_signal(e->time, e->p, e->opt);
				}
				break;
			case GUIEvent::WHEEL:
				{
					const PointerEvent *e = reinterpret_cast<const PointerEvent *>(&event);
					wheel_signal(e->time, e->p, e->delta, e->button, e->opt);
				}
				break;
			case GUIEvent::KEY:
				{
					const KeyEvent *e = reinterpret_cast<const KeyEvent *>(&event);
					key_signal(e->time, e->key);
				}
				break;
			case GUIEvent::CHAR:
				{
					const KeyEvent *e = reinterpret_cast<const KeyEvent *>(&event);
					char_signal(e->time, e->key);
				}
				break;
			}
		}
		return true;
	}

#ifdef _MT
	namespace {
		Mutex monitor;
	}
# define MONITOR	ScopedLock __al(monitor)
#else
# define MONITOR
#endif

	GameApp *GameApp::app_ = NULL;
	uint GameApp::window_count_ = 0;

	GameApp::GameApp() : lap(0.0f), profiler_("root")
	{
#if !defined(_MSC_VER) || (_MSC_VER > 1300)
		events_.event_signal.connect(guievents_.event_slot);
#endif
		basetime_ = 0;
	}

	void GameApp::incWindow()
	{
		MONITOR;
		window_count_++;
	}

	uint GameApp::decWindow()
	{
		MONITOR;
		return --window_count_;
	}

	bool GameApp::canContinue()
	{
		profiler_.end();

		profiler_.commit();
		
		profiler_.begin();
		events_.flush();
		return true;
	}

	bool GameApp::canDraw()
	{
		return true;
	}

	void GameApp::present()
	{
		true_lap = lap = fps.update();
	}

	ulong GameApp::basetime()
	{
		return basetime_;
	}

	ulong GameApp::getTimeStamp()
	{
		ulong c = clock()/(CLOCKS_PER_SEC/1000);
		return c;
	}

	/** ゲームループの標準的な実装
	 *
	 @code
		while(app().canContinue()) {

			app().update(lap);

			if(app().canDraw()) {
				graphic::clear();
				graphic::begin();

				app().draw(lap);

				graphic::end();
				app().present();
			}
		}
	 @endcode
	 */
	int GameApp::run()
	{
		while(canContinue()) {

			update_signal(lap);

			if(canDraw()) {
				graphic::device().clear();
				graphic::device().begin();

				draw_signal(lap);

				graphic::device().end();
				present();
			}
		}
		return 0;
	}

} // namespace gctp
