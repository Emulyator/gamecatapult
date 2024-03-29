#ifndef _GCTP_INPUT_HPP_
#define _GCTP_INPUT_HPP_
/** @file
 * GameCatapult DirectInputクラスヘッダファイル
 *
 * @todo DXのインターフェースそのまますぎ。もっと抽象化する。
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 2:53:36
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <ostream>
#include <vector>
#include <dinput.h>
#include <gctp/com_ptr.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/pointerlist.hpp>
#include <gctp/staticringbuffer.hpp>

namespace gctp {

	TYPEDEF_DXCOMPTR(IDirectInput8);
	TYPEDEF_DXCOMPTR(IDirectInputDevice8);

	/** 入力デバイスクラス
	 *
	 * DirectInputの仮想デバイスをあらわすクラス。
	 */
	class InputDevice : public Object {
	public:
		HRslt setUp(IDirectInput8Ptr input, REFGUID rguid, HWND hwnd, ulong flags);
		void tearDown();
		
		IDirectInputDevice8 *get() const {
			return ptr_;
		}
		IDirectInputDevice8 *operator->() const {
			if(this) return ptr_;
			return NULL;
		}
		operator IDirectInputDevice8 *() const {
			if(this) return ptr_;
			return NULL;
		}
		
		HWND hwnd() { return hwnd_; }
		HRslt setCooperativeLevel(ulong flags);
		HRslt acquire();
		HRslt unacquire();
		bool isAcquire() {return is_acquire_; }
		HRslt getState(DWORD size, void *buf);
		HRslt getData(DIDEVICEOBJECTDATA *obd, DWORD &elemnum);

		bool getData(HRslt &hr, DIDEVICEOBJECTDATA &data);

		virtual void update() = 0;

		InputDevice();
		virtual ~InputDevice();

	private:
		bool is_acquire_;	// フォーカスを得ているか？
		IDirectInputDevice8Ptr ptr_;	// デバイスのハンドル
		HWND hwnd_;
	};

	/** マウス入力デバイスクラス
	 *
	 * マウス入力デバイスをあらわすクラス。
	 *
	 * 状態は60hzで保持。step一回で1/60秒進む。
	 */
	class MouseDevice : public InputDevice {
	public:
		enum {
			BUFFER_SIZE = 256,

			ST_MOVING = 0,
			ST_STOPING = 1,
			ST_PRESSING = 2,
			ST_DRAGING = 3,
		};
		HRslt setUp(IDirectInput8Ptr input, HWND hwnd);
		/// 状態更新
		virtual void update();

		bool push[8];
		bool press[8];
		bool release[8];
		int dx;
		int dy;
		int dz;

	private:
		HANDLE mouse_event_;
	};

	/** キーボードデバイスクラス
	 *
	 * キーボード入力デバイスをあらわすクラス。
	 */
	class KbdDevice : public InputDevice {
		enum Const {
			BUFFER_SIZE = 256,
			STATE_SIZE = 256,

			PRESS = 1,
			PUSH = 1<<1,
			RELEASE = 1<<2
		};
	public:
		HRslt setUp(IDirectInput8Ptr input, HWND hwnd);

		virtual void update();

		inline bool press(int key) { return (state_[key]&PRESS)?true:false; }
		inline bool push(int key) { return (state_[key]&PUSH)?true:false; }
		inline bool release(int key) { return (state_[key]&RELEASE)?true:false; }
		static int stringToKey(const char *name);

	private:
		char state_[STATE_SIZE];
	};

	/** パッドデバイス基底クラス
	 *
	 * パッド入力デバイスをあらわすクラス。
	 */
	class PadDevice : public InputDevice {
		enum {
			BUFFER_SIZE = 256,

			PRESS = 1,
			PUSH = 1<<1,
			RELEASE = 1<<2,
			
			MAX_BUTTON = 128
		};
	public:
		HRslt setUp(IDirectInput8Ptr input, REFGUID rguid, HWND hwnd);

		virtual void update();

		inline bool press(int key) { return (buttons_[key]&PRESS)?true:false; }
		inline bool push(int key) { return (buttons_[key]&PUSH)?true:false; }
		inline bool release(int key) { return (buttons_[key]&RELEASE)?true:false; }
		inline int getHat(int index) { return LOWORD(state_.rgdwPOV[index]) == 0xFFFF ? 0 : state_.rgdwPOV[index]/4500+1; }

		DIJOYSTATE2 state_;

	private:
		char buttons_[MAX_BUTTON];
	};

	/** DirectInputクラス
	 *
	 * DirectInputの仮想デバイスをあらわすクラス。
	 * デバイスに依存するリソースのデータベースも兼ねる。
	 *
	 * @todo パッドとか色々な種類のデバイスを、どう抽象化するんだ？
	 */
	class Input {
	public:
		/** Inputの取得
		 *
		 * Inputオブジェクトのインスタンスを返す。
		 */
		friend Input &input();
		void setCurrent() { input_ = this; }

		IDirectInput8 *operator->() const {
			if(this) return api_;
			return NULL;
		}
		operator IDirectInput8 *() const {
			if(this) return api_;
			return NULL;
		}

		/// 入力デバイス情報クラス
		struct Device : public DIDEVICEINSTANCE {
			Device() {}
			Device(const DIDEVICEINSTANCE &src) : DIDEVICEINSTANCE(src) {}
		};

		static HRslt initialize(HINSTANCE hinst);
		typedef std::vector<Device> DeviceList;
		/// デバイス情報
		static const DeviceList &devicies() { return devicies_;}
	
	private:
		static DeviceList devicies_;	///< デバイス列挙情報
		static BOOL CALLBACK enumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);
		static IDirectInput8Ptr api_;				// APIのハンドル
		GCTP_TLS static Input *input_;	// カレントインスタンス

	public:
		HRslt setUp(HWND hWnd);
		void tearDown();

		/** バインドされているウィンドウを返す。
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/07/28 3:49:23
		 * $Id: input.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		HWND hwnd() { return hwnd_; }
		/// マウスデバイスを返す
		MouseDevice &mouse() { return mouse_; }
		/// キーボードデバイスを返す
		KbdDevice &kbd() { return kbd_; }

		bool isInFrame(DWORD timestamp);

		void update(float delta);
		void step(float delta);

		Handle<PadDevice> newPad(const Device &device);
		Handle<PadDevice> getPad(int i) const;

	private:
		HWND hwnd_;
		MouseDevice mouse_;
		KbdDevice kbd_;
		PointerList<PadDevice> pads_;
		DWORD frame_time_;
		DWORD cur_frame_time_;
		DWORD prev_frame_time_;
	};

	inline Input &input() { return *Input::input_; }

	/// 入力デバイス情報デバッグ出力
	extern std::basic_ostream<_TCHAR> &operator<<(std::basic_ostream<_TCHAR> &os, const Input::Device &device);

} // namespace gctp

#endif //_GCTP_INPUT_HPP_