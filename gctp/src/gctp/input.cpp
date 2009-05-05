/** @file
 * GameCatapult DirectInputクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:10:40
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/input.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/stringmap.hpp>

using namespace std;

namespace gctp {

	/////////////////////
	// Input
	//
	IDirectInput8Ptr Input::api_;

	Input::DeviceList Input::devicies_;

	namespace {
		// DIK->文字列のマップ
		class DIKMap : public StaticStringMap<int>
		{
			DIKMap()
			{
#define DEC_STR_TO_DIK(s)	(*this).put(#s, DIK_##s)
				DEC_STR_TO_DIK(ESCAPE);
				DEC_STR_TO_DIK(1);
				DEC_STR_TO_DIK(2);
				DEC_STR_TO_DIK(3);
				DEC_STR_TO_DIK(4);
				DEC_STR_TO_DIK(5);
				DEC_STR_TO_DIK(6);
				DEC_STR_TO_DIK(7);
				DEC_STR_TO_DIK(8);
				DEC_STR_TO_DIK(9);
				DEC_STR_TO_DIK(0);
				DEC_STR_TO_DIK(MINUS);
				DEC_STR_TO_DIK(EQUALS);
				DEC_STR_TO_DIK(BACK);
				DEC_STR_TO_DIK(TAB);
				DEC_STR_TO_DIK(Q);
				DEC_STR_TO_DIK(W);
				DEC_STR_TO_DIK(E);
				DEC_STR_TO_DIK(R);
				DEC_STR_TO_DIK(T);
				DEC_STR_TO_DIK(Y);
				DEC_STR_TO_DIK(U);
				DEC_STR_TO_DIK(I);
				DEC_STR_TO_DIK(O);
				DEC_STR_TO_DIK(P);
				DEC_STR_TO_DIK(LBRACKET);
				DEC_STR_TO_DIK(RBRACKET);
				DEC_STR_TO_DIK(RETURN);
				DEC_STR_TO_DIK(LCONTROL);
				DEC_STR_TO_DIK(A);
				DEC_STR_TO_DIK(S);
				DEC_STR_TO_DIK(D);
				DEC_STR_TO_DIK(F);
				DEC_STR_TO_DIK(G);
				DEC_STR_TO_DIK(H);
				DEC_STR_TO_DIK(J);
				DEC_STR_TO_DIK(K);
				DEC_STR_TO_DIK(L);
				DEC_STR_TO_DIK(SEMICOLON);
				DEC_STR_TO_DIK(APOSTROPHE);
				DEC_STR_TO_DIK(GRAVE);
				DEC_STR_TO_DIK(LSHIFT);
				DEC_STR_TO_DIK(BACKSLASH);
				DEC_STR_TO_DIK(Z);
				DEC_STR_TO_DIK(X);
				DEC_STR_TO_DIK(C);
				DEC_STR_TO_DIK(V);
				DEC_STR_TO_DIK(B);
				DEC_STR_TO_DIK(N);
				DEC_STR_TO_DIK(M);
				DEC_STR_TO_DIK(COMMA);
				DEC_STR_TO_DIK(PERIOD);
				DEC_STR_TO_DIK(SLASH);
				DEC_STR_TO_DIK(RSHIFT);
				DEC_STR_TO_DIK(MULTIPLY);/* * on numeric keypad */
				DEC_STR_TO_DIK(LMENU);
				DEC_STR_TO_DIK(SPACE);
				DEC_STR_TO_DIK(CAPITAL);
				DEC_STR_TO_DIK(F1);
				DEC_STR_TO_DIK(F2);
				DEC_STR_TO_DIK(F3);
				DEC_STR_TO_DIK(F4);
				DEC_STR_TO_DIK(F5);
				DEC_STR_TO_DIK(F6);
				DEC_STR_TO_DIK(F7);
				DEC_STR_TO_DIK(F8);
				DEC_STR_TO_DIK(F9);
				DEC_STR_TO_DIK(F10);
				DEC_STR_TO_DIK(NUMLOCK);
				DEC_STR_TO_DIK(SCROLL);
				DEC_STR_TO_DIK(NUMPAD7);
				DEC_STR_TO_DIK(NUMPAD8);
				DEC_STR_TO_DIK(NUMPAD9);
				DEC_STR_TO_DIK(SUBTRACT);/* - on numeric keypad */
				DEC_STR_TO_DIK(NUMPAD4);
				DEC_STR_TO_DIK(NUMPAD5);
				DEC_STR_TO_DIK(NUMPAD6);
				DEC_STR_TO_DIK(ADD);
				DEC_STR_TO_DIK(NUMPAD1);
				DEC_STR_TO_DIK(NUMPAD2);
				DEC_STR_TO_DIK(NUMPAD3);
				DEC_STR_TO_DIK(NUMPAD0);
				DEC_STR_TO_DIK(DECIMAL);/* . on numeric keypad */
				DEC_STR_TO_DIK(OEM_102);/* <> or \| on RT 102-key keyboard (Non-U.S.) */
				DEC_STR_TO_DIK(F11);
				DEC_STR_TO_DIK(F12);
				DEC_STR_TO_DIK(F13);
				DEC_STR_TO_DIK(F14);
				DEC_STR_TO_DIK(F15);
				DEC_STR_TO_DIK(KANA);
				DEC_STR_TO_DIK(ABNT_C1);/* /? on Brazilian keyboard */
				DEC_STR_TO_DIK(CONVERT);/* (Japanese keyboard)            */
				DEC_STR_TO_DIK(NOCONVERT);/* (Japanese keyboard)            */
				DEC_STR_TO_DIK(YEN);
				DEC_STR_TO_DIK(ABNT_C2);/* Numpad . on Brazilian keyboard */
				DEC_STR_TO_DIK(NUMPADEQUALS);/* = on numeric keypad (NEC PC98) */
				DEC_STR_TO_DIK(PREVTRACK);/* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
				DEC_STR_TO_DIK(AT);
				DEC_STR_TO_DIK(COLON);
				DEC_STR_TO_DIK(UNDERLINE);/*                     (NEC PC98) */
				DEC_STR_TO_DIK(KANJI);
				DEC_STR_TO_DIK(STOP);
				DEC_STR_TO_DIK(AX);
				DEC_STR_TO_DIK(UNLABELED);/*                        (J3100) */
				DEC_STR_TO_DIK(NEXTTRACK);/* Next Track */
				DEC_STR_TO_DIK(NUMPADENTER);/* Enter on numeric keypad */
				DEC_STR_TO_DIK(RCONTROL);
				DEC_STR_TO_DIK(MUTE);
				DEC_STR_TO_DIK(CALCULATOR);/* Calculator */
				DEC_STR_TO_DIK(PLAYPAUSE);/* Play / Pause */
				DEC_STR_TO_DIK(MEDIASTOP);/* Media Stop */
				DEC_STR_TO_DIK(VOLUMEDOWN);/* Volume - */
				DEC_STR_TO_DIK(VOLUMEUP);/* Volume + */
				DEC_STR_TO_DIK(WEBHOME);/* Web home */
				DEC_STR_TO_DIK(NUMPADCOMMA);/* , on numeric keypad (NEC PC98) */
				DEC_STR_TO_DIK(DIVIDE);
				DEC_STR_TO_DIK(SYSRQ);
				DEC_STR_TO_DIK(RMENU);
				DEC_STR_TO_DIK(PAUSE);
				DEC_STR_TO_DIK(HOME);
				DEC_STR_TO_DIK(UP);
				DEC_STR_TO_DIK(PRIOR);
				DEC_STR_TO_DIK(LEFT);
				DEC_STR_TO_DIK(RIGHT);
				DEC_STR_TO_DIK(END);
				DEC_STR_TO_DIK(DOWN);
				DEC_STR_TO_DIK(NEXT);
				DEC_STR_TO_DIK(INSERT);
				DEC_STR_TO_DIK(DELETE);
				DEC_STR_TO_DIK(LWIN);
				DEC_STR_TO_DIK(RWIN);
				DEC_STR_TO_DIK(APPS);
				DEC_STR_TO_DIK(POWER);
				DEC_STR_TO_DIK(SLEEP);
				DEC_STR_TO_DIK(WAKE);
				DEC_STR_TO_DIK(WEBSEARCH);
				DEC_STR_TO_DIK(WEBFAVORITES);
				DEC_STR_TO_DIK(WEBREFRESH);
				DEC_STR_TO_DIK(WEBSTOP);
				DEC_STR_TO_DIK(WEBFORWARD);
				DEC_STR_TO_DIK(WEBBACK);
				DEC_STR_TO_DIK(MYCOMPUTER);
				DEC_STR_TO_DIK(MAIL);
				DEC_STR_TO_DIK(MEDIASELECT);
#undef DEC_STR_TO_DIK
			}

		public:
			static const DIKMap &getInstance()
			{
				static DIKMap ret;
				return ret;
			}
		};
	}

	std::basic_ostream<_TCHAR> &operator<<(std::basic_ostream<_TCHAR> &os, const Input::Device &device)
	{
		os<<_T("name : ")<<device.tszInstanceName<<_T("   caption : ")<<device.tszProductName<<std::endl;
		os<<_T("   デバイスタイプ:");
		switch(GET_DIDEVICE_TYPE(device.dwDevType)) {
		case DI8DEVTYPE_1STPERSON:
			os<<_T("FPS用 : "); // 対面型のアクション ゲーム デバイス。
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPE1STPERSON_LIMITED:
				os<<_T("アクション マッピング対象のデバイス オブジェクトの最小数を指定しないデバイス。");
				break;
			case DI8DEVTYPE1STPERSON_SHOOTER:
				os<<_T("FPS用に設計されたデバイス。");
				break;
			case DI8DEVTYPE1STPERSON_SIXDOF:
				os<<_T("6 段階 (3 つの横方向軸と 3 つの回転軸) の自由を備えたデバイス。");
				break;
			case DI8DEVTYPE1STPERSON_UNKNOWN:
				os<<_T("不明なサブタイプ。");
				break;
			default:
				os<<_T("未定義の値");
				break;
			}
			break;
		case DI8DEVTYPE_DEVICE:
			os<<_T("分類不能");
			break;
		case DI8DEVTYPE_DEVICECTRL:
			os<<_T("アプリケーションのコンテキストから別タイプのデバイスの制御に使用される入力デバイス : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEDEVICECTRL_COMMSSELECTION:
				os<<_T("通信の選択に使用されるコントロール。");
				break;
			case DI8DEVTYPEDEVICECTRL_COMMSSELECTION_HARDWIRED:
				os<<_T("デフォルトの構成を使用する必要があり、マッピングを変更できないデバイス。");
				break;
			case DI8DEVTYPEDEVICECTRL_UNKNOWN:
				os<<_T("不明なサブタイプ。");
				break;
			default:
				os<<_T("未定義の値");
				break;
			}
			break;
		case DI8DEVTYPE_DRIVING:
			os<<_T("ステアリング : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEDRIVING_COMBINEDPEDALS:
				os<<_T("単一の軸からアクセルとブレーキ ペダルの値を報告するステアリング デバイス。");
				break;
			case DI8DEVTYPEDRIVING_DUALPEDALS:
				os<<_T("別々の軸からアクセルとブレーキ ペダルの値を報告するステアリング デバイス。");
				break;
			case DI8DEVTYPEDRIVING_HANDHELD:
				os<<_T("ハンドヘルドのステアリング デバイス。");
				break;
			case DI8DEVTYPEDRIVING_LIMITED:
				os<<_T("アクション マッピング対象のデバイス オブジェクトの最小数を指定しないステアリング デバイス。");
				break;
			case DI8DEVTYPEDRIVING_THREEPEDALS:
				os<<_T("別々の軸からアクセル、ブレーキ、およびクラッチ ペダルの値を報告するステアリング デバイス。");
				break;
			default:
				os<<_T("未定義の値");
				break;
			}
			break;
		case DI8DEVTYPE_FLIGHT:
			os<<_T("フライト シミュレーション用コントローラ : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEFLIGHT_LIMITED:
				os<<_T("アクション マッピング対象のデバイス オブジェクトの最小数を指定しないフライト コントローラ。");
				break;
			case DI8DEVTYPEFLIGHT_RC:
				os<<_T("模型飛行機のリモート コントロールに基づくフライト デバイス。");
				break;
			case DI8DEVTYPEFLIGHT_STICK:
				os<<_T("ジョイスティック。");
				break;
			case DI8DEVTYPEFLIGHT_YOKE:
				os<<_T("操縦輪。");
				break;
			default:
				os<<_T("未定義の値");
				break;
			}
			break;
		case DI8DEVTYPE_GAMEPAD:
			os<<_T("ゲームパッド : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEGAMEPAD_LIMITED:
				os<<_T("アクション マッピング対象のデバイス オブジェクトの最小数を指定しないゲームパッド。");
				break;
			case DI8DEVTYPEGAMEPAD_STANDARD:
				os<<_T("アクション マッピング対象のデバイス オブジェクトの最小数を指定しない標準ゲームパッド。");
				break;
			case DI8DEVTYPEGAMEPAD_TILT:
				os<<_T("コントローラの姿勢から x 軸と y 軸のデータを報告するゲームパッド。");
				break;
			default:
				os<<_T("未定義の値");
				break;
			}
			break;
		case DI8DEVTYPE_JOYSTICK:
			os<<_T("ジョイスティック : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEJOYSTICK_LIMITED:
				os<<_T("アクション マッピング対象のデバイス オブジェクトの最小数を指定しないジョイスティック。");
				break;
			case DI8DEVTYPEJOYSTICK_STANDARD:
				os<<_T("アクション マッピング対象のデバイス オブジェクトの最小数を指定する標準ジョイスティック。");
				break;
			default:
				os<<_T("未定義の値");
				break;
			}
			break;
		case DI8DEVTYPE_KEYBOARD:
			os<<_T("キーボードまたはキーボード類似デバイス : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEKEYBOARD_UNKNOWN:
				os<<_T("サブタイプは確定できない。");
				break;
			case DI8DEVTYPEKEYBOARD_PCXT:
				os<<_T("IBM PC/XT 83 キー キーボード");
				break;
			case DI8DEVTYPEKEYBOARD_OLIVETTI:
				os<<_T("Olivetti 102 キー キーボード");
				break;
			case DI8DEVTYPEKEYBOARD_PCAT:
				os<<_T("IBM PC/AT 84 キー キーボード");
				break;
			case DI8DEVTYPEKEYBOARD_PCENH:
				os<<_T("IBM PC 拡張 101/102 キー式キーボードまたは Microsoft ナチュラル キーボード");
				break;
			case DI8DEVTYPEKEYBOARD_NOKIA1050:
				os<<_T("Nokia 1050 キーボード");
				break;
			case DI8DEVTYPEKEYBOARD_NOKIA9140:
				os<<_T("Nokia 9140 キーボード");
				break;
			case DI8DEVTYPEKEYBOARD_NEC98:
				os<<_T("日本語 NEC PC98 キーボード");
				break;
			case DI8DEVTYPEKEYBOARD_NEC98LAPTOP:
				os<<_T("日本語 NEC PC98 ラップトップ キーボード");
				break;
			case DI8DEVTYPEKEYBOARD_NEC98106:
				os<<_T("日本語 NEC PC98 106 キー キーボード");
				break;
			case DI8DEVTYPEKEYBOARD_JAPAN106:
				os<<_T("日本語 106 キー キーボード");
				break;
			case DI8DEVTYPEKEYBOARD_JAPANAX:
				os<<_T("日本語 AX キーボード");
				break;
			case DI8DEVTYPEKEYBOARD_J3100:
				os<<_T("日本語 J3100 キーボード");
				break;
			default:
				os<<_T("未定義の値");
				break;
			}
			break;
		case DI8DEVTYPE_MOUSE:
			os<<_T("マウスまたはマウス類似デバイス (トラックボールなど) : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEMOUSE_ABSOLUTE:
				os<<_T("絶対軸データを返すマウス。");
				break;
			case DI8DEVTYPEMOUSE_FINGERSTICK:
				os<<_T("フィンガースティック。");
				break;
			case DI8DEVTYPEMOUSE_TOUCHPAD:
				os<<_T("タッチパッド。");
				break;
			case DI8DEVTYPEMOUSE_TRACKBALL:
				os<<_T("トラックボール。");
				break;
			case DI8DEVTYPEMOUSE_TRADITIONAL:
				os<<_T("従来型マウス。");
				break;
			case DI8DEVTYPEMOUSE_UNKNOWN:
				os<<_T("サブタイプは確定できない。");
				break;
			default:
				os<<_T("未定義の値");
				break;
			}
			break;
		case DI8DEVTYPE_REMOTE:
			os<<_T("リモート コントロール デバイス : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEREMOTE_UNKNOWN:
				os<<_T("サブタイプは確定できない。");
				break;
			default:
				os<<_T("未定義の値");
				break;
			}
			break;
		case DI8DEVTYPE_SCREENPOINTER:
			os<<_T("スクリーン ポインタ : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPESCREENPTR_UNKNOWN:
				os<<_T("不明なサブタイプ");
				break;
			case DI8DEVTYPESCREENPTR_LIGHTGUN:
				os<<_T("ライト ガン。");
				break;
			case DI8DEVTYPESCREENPTR_LIGHTPEN:
				os<<_T("ライト ペン。");
				break;
			case DI8DEVTYPESCREENPTR_TOUCH:
				os<<_T("タッチ スクリーン。");
				break;
			default:
				os<<_T("未定義の値");
				break;
			}
			break;
		case DI8DEVTYPE_SUPPLEMENTAL:
			os<<_T("ホイールと共に使用するペダルなど、アプリケーションのメイン コントロールに適さない機能を備えた特殊デバイス : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPESUPPLEMENTAL_2NDHANDCONTROLLER:
				os<<_T("2 次的ハンドヘルド コントローラ。");
				break;
			case DI8DEVTYPESUPPLEMENTAL_COMBINEDPEDALS:
				os<<_T("主機能として、単一の軸からアクセルとブレーキ ペダルの値を報告するデバイス。");
				break;
			case DI8DEVTYPESUPPLEMENTAL_DUALPEDALS:
				os<<_T("主機能として、別々の軸からアクセルとブレーキ ペダルの値を報告するデバイス。");
				break;
			case DI8DEVTYPESUPPLEMENTAL_HANDTRACKER:
				os<<_T("手の移動を追跡するデバイス。");
				break;
			case DI8DEVTYPESUPPLEMENTAL_HEADTRACKER:
				os<<_T("頭の移動を追跡するデバイス。");
				break;
			case DI8DEVTYPESUPPLEMENTAL_RUDDERPEDALS:
				os<<_T("方向舵ペダルを備えたデバイス。");
				break;
			case DI8DEVTYPESUPPLEMENTAL_SHIFTER:
				os<<_T("軸からギア選択を報告するデバイス。");
				break;
			case DI8DEVTYPESUPPLEMENTAL_SHIFTSTICKGATE:
				os<<_T("ボタン状態からギア選択を報告するデバイス。");
				break;
			case DI8DEVTYPESUPPLEMENTAL_SPLITTHROTTLE:
				os<<_T("主機能として、2 つ以上のスロットル値を報告するデバイス。その他のコントロールを持つ場合がある。");
				break;
			case DI8DEVTYPESUPPLEMENTAL_THREEPEDALS:
				os<<_T("主機能として、別々の軸からアクセル、ブレーキ、およびクラッチ ペダルの値を報告するデバイス。");
				break;
			case DI8DEVTYPESUPPLEMENTAL_THROTTLE:
				os<<_T("主機能として、単一のスロットル値を報告するデバイス。その他のコントロールを持つ場合がある。");
				break;
			case DI8DEVTYPESUPPLEMENTAL_UNKNOWN:
				os<<_T("不明なサブタイプ。");
				break;
			default:
				os<<_T("未定義の値");
				break;
			}
			break;
		default:
			os<<_T("未定義の値");
			break;
		}
		return os;
	}

	BOOL Input::enumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
	{
		std::vector<Device> &devicies = *(std::vector<Device> *)pvRef;
		devicies.push_back(Device(*lpddi));
		return TRUE;
	}

	/** 初期化
	 *
	 * APIセットを取得
	 */
	HRslt Input::initialize(
		HINSTANCE hinst	///< アプリケーションのインスタンス
	) {
		HRslt hr;
		if(!api_) {
			hr = DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&api_, NULL);
			if(!hr) return hr;
		}
		if(devicies_.size() > 0) devicies_.clear();
		hr = api_->EnumDevices(DI8DEVCLASS_GAMECTRL, enumDevicesCallback, &devicies_, DIEDFL_ATTACHEDONLY);
		return hr;
	}

	Input *Input::input_ = NULL;	// カレントインスタンス
	
	HRslt Input::setUp(HWND hWnd)
	{
		HRslt ret(S_OK), hr;
		hwnd_ = hWnd;
		cur_frame_time_ = prev_frame_time_ = frame_time_ = ::GetTickCount();
		hr = mouse_.setUp(api_, hWnd);
		if(!hr) {
			ret = hr;
			GCTP_TRACE(hr);
		}
		hr = kbd_.setUp(api_, hWnd);
		if(!hr) {
			ret = hr;
			GCTP_TRACE(hr);
		}
		return ret;
	}

	void Input::tearDown()
	{
		mouse_.tearDown();
		kbd_.tearDown();
	}

	void Input::update(float delta)
	{
		cur_frame_time_ = prev_frame_time_ = frame_time_;
		frame_time_ = ::GetTickCount();
		step(delta);
	}

	void Input::step(float delta)
	{
		cur_frame_time_ += (DWORD)(delta*1000);
		if(mouse_) mouse_.update();
		if(kbd_) kbd_.update();
		for(PointerList<PadDevice>::iterator i = pads_.begin(); i != pads_.end(); ++i) {
			(*i)->update();
		}
	}

	bool Input::isInFrame(DWORD timestamp)
	{
//		return (prev_frame_time_ <= cur_frame_time_ ? (prev_frame_time_ <= timestamp && timestamp < cur_frame_time_) : (timestamp < cur_frame_time_|| prev_frame_time_ <= timestamp));
		return ((int)(timestamp - cur_frame_time_) <= 0);
	}

	Handle<PadDevice> Input::newPad(const Device &device)
	{
		for(PointerList<PadDevice>::const_iterator i = pads_.begin(); i != pads_.end(); ++i) {
			DIDEVICEINSTANCE info;
			(**i)->GetDeviceInfo(&info);
			if(info.guidInstance == device.guidInstance) return *i;
		}
		Pointer<PadDevice> pad = new PadDevice;
		HRslt hr = pad->setUp(api_, device.guidInstance, hwnd_);
		if(hr) pads_.push_back(pad);
		else GCTP_ERRORINFO(hr);
		return pad;
	}
	
	Handle<PadDevice> Input::getPad(int i) const
	{
		int ii = 0;
		for(PointerList<PadDevice>::const_iterator j = pads_.begin(); j != pads_.end() && ii <= i; ++j, ii++) {
			if(i == ii) return *j;
		}
		return 0;
	}

	/////////////////////
	// InputDevice
	//

	/** InputDeviceを取得
	 *
	 * この時点では、Acquireしない。@n
	 * 派生クラスで、完全なセットアップが終わった時点で、acquireする必要がある。
	 */
	HRslt InputDevice::setUp(IDirectInput8Ptr input, REFGUID rguid, HWND hwnd, ulong flags)
	{
		HRslt hr;
		hwnd_ = hwnd;
		hr = input->CreateDevice(rguid, &ptr_, NULL);
		if(!hr) return hr;
		hr = setCooperativeLevel(flags);
		if(!hr) PRNN(hr);
		return hr;
	}

	void InputDevice::tearDown()
	{
		if(ptr_) unacquire();
		ptr_ = 0;
	}

	InputDevice::InputDevice()
		: is_acquire_(false)
	{
	}

	InputDevice::~InputDevice()
	{
		tearDown();
	}

	HRslt InputDevice::setCooperativeLevel(ulong flags)
	{
		return ptr_->SetCooperativeLevel(hwnd_, flags);
	}

	HRslt InputDevice::acquire()
	{
		HRslt hr;
		hr = ptr_->Acquire();
		if(hr) is_acquire_ = true;
		else is_acquire_ = false;
		return hr;
	}

	HRslt InputDevice::unacquire()
	{
		is_acquire_ = false;
		return ptr_->Unacquire();
	}

	HRslt InputDevice::getState(DWORD size, void *buf)
	{
		if(!is_acquire_) return DIERR_INPUTLOST;
		ptr_->Poll();
		HRslt hr = ptr_->GetDeviceState(size, buf);
		if(hr == DIERR_INPUTLOST) is_acquire_ = false;
		return hr;
	}

	HRslt InputDevice::getData(DIDEVICEOBJECTDATA *obd, DWORD &elemnum)
	{
		if(!is_acquire_) return DIERR_INPUTLOST;
		ptr_->Poll();
		HRslt hr = ptr_->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), obd, &elemnum, 0);
		if(hr == DIERR_INPUTLOST) is_acquire_ = false;
		return hr;
	}
	
	bool InputDevice::getData(HRslt &hr, DIDEVICEOBJECTDATA &data)
	{
		if(!is_acquire_) hr = acquire();
		if(is_acquire_) {
			ptr_->Poll();
			DWORD elemnum = 1;
			HRslt hr = ptr_->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &data, &elemnum, DIGDD_PEEK);
			if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
				is_acquire_ = false;
				hr = acquire();
			}
			else if(hr && elemnum > 0 && input().isInFrame(data.dwTimeStamp)) {
				elemnum = 1;
				hr = ptr_->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), 0, &elemnum, 0);
				if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
					is_acquire_ = false;
					hr = acquire();
				}
				return hr;
			}
		}
		return false;
	}

	/////////////////////
	// MouseDevice
	//
	HRslt MouseDevice::setUp(IDirectInput8Ptr input, HWND hwnd)
	{
		HRslt hr;
		hr = InputDevice::setUp(input, GUID_SysMouse, hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		//hr = InputDevice::setUp(input, GUID_SysMouse, hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		if(!hr) return hr;
		hr = get()->SetDataFormat(&c_dfDIMouse);
		if(!hr) return hr;

		mouse_event_ = CreateEvent(NULL, FALSE, FALSE, NULL);
		if(mouse_event_ == NULL) return E_FAIL;
		hr = get()->SetEventNotification(mouse_event_);
		if(!hr) return hr;

		DIPROPDWORD dipdw;
		// ヘッダー
		dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj        = 0;
		dipdw.diph.dwHow        = DIPH_DEVICE;
		// データ
		dipdw.dwData            = BUFFER_SIZE;
		// 次に、ヘッダー (DIPROPDWORD 構造体内の DIPROPHEADER 構造体) のアドレスと、変更したいプロパティの識別子を、次のように IDirectInputDevice8::SetProperty メソッドに渡す。
		hr = get()->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

		dx = dy = dz = 0;
		memset(push, 0, sizeof(push));
		memset(press, 0, sizeof(press));
		memset(release, 0, sizeof(release));

		return hr;
	}

	void MouseDevice::update()
	{
		dx = dy = dz = 0;
		memset(push, 0, sizeof(push));
		memset(release, 0, sizeof(release));
		HRslt hr;
		DIDEVICEOBJECTDATA od;
		while(getData(hr, od)) {
			/* 要素を調べて何が発生したかを確認する。*/
			switch (od.dwOfs) {
            case DIMOFS_BUTTON0:
				if(od.dwData&0x80) {
					push[0] = true;
					press[0] = true;
				}
				else {
					release[0] = true;
					press[0] = false;
				}
                break;
            case DIMOFS_BUTTON1:
				if(od.dwData&0x80) {
					push[1] = true;
					press[1] = true;
				}
				else {
					release[1] = true;
					press[1] = false;
				}
                break;
            case DIMOFS_BUTTON2:
				if(od.dwData&0x80) {
					push[2] = true;
					press[2] = true;
				}
				else {
					release[2] = true;
					press[2] = false;
				}
                break;
            case DIMOFS_BUTTON3:
				if(od.dwData&0x80) {
					push[3] = true;
					press[3] = true;
				}
				else {
					release[3] = true;
					press[3] = false;
				}
                break;
            case DIMOFS_BUTTON4:
				if(od.dwData&0x80) {
					push[4] = true;
					press[4] = true;
				}
				else {
					release[4] = true;
					press[4] = false;
				}
                break;
            case DIMOFS_BUTTON5:
				if(od.dwData&0x80) {
					push[5] = true;
					press[5] = true;
				}
				else {
					release[5] = true;
					press[5] = false;
				}
                break;
            case DIMOFS_BUTTON6:
				if(od.dwData&0x80) {
					push[6] = true;
					press[6] = true;
				}
				else {
					release[6] = true;
					press[6] = false;
				}
                break;
            case DIMOFS_BUTTON7:
				if(od.dwData&0x80) {
					push[7] = true;
					press[7] = true;
				}
				else {
					release[7] = true;
					press[7] = false;
				}
                break;
            case DIMOFS_X:
				dx += (int)od.dwData;
                break;
            case DIMOFS_Y:
				dy += (int)od.dwData;
                break;
            case DIMOFS_Z:
				dz += (int)od.dwData;
                break;
			}
		}
	}

	/////////////////////
	// KbdDevice
	//
	HRslt KbdDevice::setUp(IDirectInput8Ptr input, HWND hwnd)
	{
		HRslt hr;
		hr = InputDevice::setUp(input, GUID_SysKeyboard, hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		if(!hr) return hr;
		hr = get()->SetDataFormat(&c_dfDIKeyboard);
		DIPROPDWORD dipdw = { sizeof(DIPROPDWORD), sizeof(DIPROPHEADER), 0, DIPH_DEVICE };
		// データ
		dipdw.dwData  = BUFFER_SIZE;
		// 次に、ヘッダー (DIPROPDWORD 構造体内の DIPROPHEADER 構造体) のアドレスと、変更したいプロパティの識別子を、次のように IDirectInputDevice8::SetProperty メソッドに渡す。
		hr = get()->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

		memset(state_, 0, sizeof(state_));
		return hr;
	}

	void KbdDevice::update()
	{
		for(int i = 0; i < STATE_SIZE; i++) state_[i] = state_[i]&PRESS;
		HRslt hr;
		DIDEVICEOBJECTDATA od;
		while(getData(hr, od)) {
			/* 要素を調べて何が発生したかを確認する。*/
			if(od.dwData&0x80) state_[od.dwOfs] = PRESS|(state_[od.dwOfs]&PRESS ? 0 : PUSH)|(state_[od.dwOfs]&RELEASE);
			else state_[od.dwOfs] = (state_[od.dwOfs]&PRESS ? RELEASE : 0)|(state_[od.dwOfs]&PUSH);
		}
	}
	
	int KbdDevice::stringToKey(const char *name)
	{
		return DIKMap::getInstance().get(name);
	}

	/////////////////////
	// PadDevice
	//
	HRslt PadDevice::setUp(IDirectInput8Ptr input, REFGUID rguid, HWND hwnd)
	{
		HRslt hr;
		hr = InputDevice::setUp(input, rguid, hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		if(!hr) {
			GCTP_ERRORINFO(hr);
			return hr;
		}
		hr = get()->SetDataFormat(&c_dfDIJoystick2);
		if(!hr) GCTP_ERRORINFO(hr);

		DIPROPDWORD dipdw = { sizeof(DIPROPDWORD), sizeof(DIPROPHEADER), 0, DIPH_DEVICE };
		// データ
		dipdw.dwData  = BUFFER_SIZE;
		// 次に、ヘッダー (DIPROPDWORD 構造体内の DIPROPHEADER 構造体) のアドレスと、変更したいプロパティの識別子を、次のように IDirectInputDevice8::SetProperty メソッドに渡す。
		hr = get()->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
		if(!hr) GCTP_ERRORINFO(hr);

		memset(&state_, 0, sizeof(state_));
		memset(&buttons_, 0, sizeof(buttons_));
		return hr;
	}

	void PadDevice::update()
	{
		for(int i = 0; i < MAX_BUTTON; i++) buttons_[i] = buttons_[i]&PRESS;
		HRslt hr;
		/*hr = getState(sizeof(DIJOYSTATE2), &state_);
		if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
			hr = acquire();
			return;
		}*/
		DIDEVICEOBJECTDATA od;
		while(getData(hr, od)) {
			if(DIJOFS_POV(0) <= od.dwOfs && od.dwOfs <= DIJOFS_POV(3)) {
				*(DWORD *)(((BYTE *)&state_)+od.dwOfs) = od.dwData;
			}
			else if(DIJOFS_BUTTON0 <= od.dwOfs && od.dwOfs <= DIJOFS_BUTTON31) {
				BYTE &rgbbtn = *(BYTE *)(((BYTE *)&state_)+od.dwOfs);
				int i = (od.dwOfs-DIJOFS_BUTTON0)/(DIJOFS_BUTTON1-DIJOFS_BUTTON0);
				if(od.dwData&0x80) buttons_[i] = PRESS|(rgbbtn&PRESS ? 0 : PUSH)|(buttons_[i]&RELEASE);
				else buttons_[i] = (rgbbtn&PRESS ? RELEASE : 0)|(buttons_[i]&PUSH);
				rgbbtn = (BYTE)od.dwData;
			}
			else {
				*(LONG *)(((BYTE *)&state_)+od.dwOfs) = (LONG)od.dwData;
			}
		}
		//if(!hr) GCTP_ERRORINFO(hr);
	}

} // !gctp
