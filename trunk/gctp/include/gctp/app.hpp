#ifndef _GCTP_APP_HPP_
#define _GCTP_APP_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult アプリケーションクラスヘッダファイル
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
#include <gctp/mutex.hpp>
#endif

namespace gctp {

	/*struct UUID {
		uint32_t	Data1;       // 00000000-
		uint16_t	Data2;       // 0000-
		uint16_t	Data3;       // 0000-
		uint8_t		Data4[8];    // C000-000000000046
	};*/

	/** 入力イベント
	 *
	 * キューのほうでこれをポインター補足し、ポリモーフ可能なようにすると、メモリ断片化への対処が難しくなる
	 *
	 * 柔軟性より、使用メモリ量を固定できることのほうが重要であると判断｡
	 *（ポリモーフ版・非ポリモーフ版とでインターフェースを一緒には出来ない、はず。切り替え可能はあきらめたほうがいい）
	 *
	 * 属性情報がATTR_SIZEバイト超える場合（ネットワークメッセージとか）は、属性バッファにポインタを含めてそれが
	 * ネットワークメッセージを差す、で対処。領域解放は、typeごとに明示的にする必要あり
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/24 1:02:41
	 */
	struct EventHeader {
		enum {
			ATTR_SIZE = 24,
		};
		GUID guid; ///< イベント識別子
		// UUIDのほうがいいかなぁ
		ulong time; ///< 基準時間からのオフセット(ms)
	};

	/// イベントブロック実体
	struct Event : EventHeader {
		char attr[ATTR_SIZE];
	};

	/** 入力イベントキュークラス
	 *
	 * ゲームスレッドで処理できるように、UIスレッド（メインスレッド）で発生した
	 * いくつかの入力イベントを溜め込むキュー。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/11/21 17:55:36
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class EventQue : public Object {
	public:
		void post(const Event &event);
		void flush();
		/// イベントシグナル
		Signal1<true, const Event &> event_signal;
		/// イベントの解体するときに発行されるシグナル（もし特別な処理が必要なら、これに登録）
		Signal1<true, Event &> erase_signal;
	private:
		typedef std::deque<Event> QType;
		QType q_;
#ifdef _MT
		Mutex monitor_;
#endif
	};

	/** GUI系入力イベントキュークラス
	 *
	 * ゲームスレッドで処理できるように、UIスレッド（メインスレッド）で発生した
	 * いくつかの入力イベントを溜め込むキュー。
	 *
	 * マウスアップ・マウスダウン・マウスドラッグ・カーソルムーブ・キーアップ･キーダウンなどのみ扱う
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

		static void postDownMsg(EventQue &q, ulong time, Point2 p, uint8_t button, uint8_t opt);
		static void postUpMsg(EventQue &q, ulong time, Point2 p, uint8_t button, uint8_t opt);
		static void postDblClickMsg(EventQue &q, ulong time, Point2 p, uint8_t button, uint8_t opt);
		static void postMoveMsg(EventQue &q, ulong time, Point2 p, uint8_t button);
		static void postWheelMsg(EventQue &q, ulong time, Point2 p, int16_t delta, uint8_t wheel, uint8_t opt);
		static void postKeyMsg(EventQue &q, ulong time, int key);
		static void postCharMsg(EventQue &q, ulong time, int ch);

		/// ボタンが押されたら(TimeStamp, X, Y, BUTTON, OPTBUTTON)
		Signal4<true, const ulong &, const Point2 &, uint8_t, uint8_t>			down_signal;
		/// ボタンが離されたら(TimeStamp, X, Y, BUTTON, OPTBUTTON)
		Signal4<true, const ulong &, const Point2 &, uint8_t, uint8_t>			up_signal;
		/// ダブルクリックされたら(TimeStamp, X, Y, BUTTON, OPTBUTTON)
		Signal4<true, const ulong &, const Point2 &, uint8_t, uint8_t>			dblclick_signal;
		/// ポインタの移動(TimeStamp, X, Y, OPTBUTTON)
		Signal3<true, const ulong &, const Point2 &, uint8_t>					move_signal;
		/// マウスホイールや、パッドのアナコン操作(TimeStamp, X, Y, DELTA, WHEEL, OPT)
		Signal5<true, const ulong &, const Point2 &, int16_t, uint8_t, uint8_t>	wheel_signal;
		/// キー入力（TimeStamp, プラットホーム依存、WindowsならVK_*が入る）
		Signal2<true, const ulong &, int>										key_signal;
		/// 文字入力
		Signal2<true, const ulong &, int>										char_signal;

		bool procEvent(const Event &event);
#ifndef BOOST_NO_POINTER_TO_MEMBER_TEMPLATE_PARAMETERS
		/** イベント応答スロット
		 * これをEvent::event_signalに接続
		 */
		MemberSlot1<GUIEvents, const Event &, &GUIEvents::procEvent> event_slot;
#else
		/** イベント応答スロットへのポインタ
		 * これをEvent::event_signalに接続
		 */
		Pointer<Slot> event_slot;
#endif
	};

	/** ゲームメイン基底クラス
	 *
	 * 派生クラスのpresentでは、このクラスのpresentを呼び出してやる必要がある。
	 * 
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/14 5:54:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class GameApp {
		GCTP_TLS static GameApp *app_;
		static uint window_count_;
		Profiler profiler_;
		bool present_;
		EventQue events_;
		GUIEvents guievents_;
		ulong basetime_;
		ulong current_clock_;
	public:
		GameApp();
		virtual bool canContinue();
		virtual bool canDraw();
		virtual void present();
		virtual HWND getHandle() const = 0;
		virtual void showCursor(bool yes) = 0;
		virtual void holdCursor(bool yes) = 0;

		/// ルートプロファイラ
		Profiler &profiler() { return profiler_; }
		/// ウィンドウの数（アプリケーション中のGameAppの数）
		uint window_count() { return window_count_; }

		/// イベントキュー
		EventQue &events() { return events_; }
		/// GUIイベントシグナル
		GUIEvents &guievents() { return guievents_; }
		/// 更新シグナル
		Signal1<false, float /*delta*/> update_signal;
		/// 描画シグナル
		Signal1<false, float /*delta*/> draw_signal;
		/// 基準時間(イベントのtimeを扱うときの値)(ms)
		ulong basetime();
		/// イベントを発行するときの、現在時間取得(ms)
		ulong getTimeStamp();

		/// 実行
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

	/** スコープにしたがってプロファイルするクラス
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