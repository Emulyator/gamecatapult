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

using namespace std;

namespace gctp {

	/////////////////////
	// Input
	//
	IDirectInput8Ptr Input::api_;

	Input::DeviceList Input::devicies_;

#ifdef UNICODE
	std::wostream &operator<<(std::wostream &os, const Input::Device &device)
#else
	std::ostream &operator<<(std::ostream &os, const Input::Device &device)
#endif
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

	HRslt Input::update()
	{
		HRslt ret(S_OK), hr;
		if(mouse_) {
			hr = mouse_.update();
			if(!hr) ret = hr;
		}
		if(kbd_) {
			hr = kbd_.update();
			if(!hr) ret = hr;
		}
		return ret;
	}

	/////////////////////
	// InputDevice
	//

	/** InputDeviceを取得
	 *
	 * この時点では、Acquireしない。<BR>
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
		HRslt hr = ptr_->GetDeviceState(size, buf);
		if(hr == DIERR_INPUTLOST) is_acquire_ = false;
		return hr;
	}

	HRslt InputDevice::getData(DIDEVICEOBJECTDATA *obd, DWORD &elemnum)
	{
		if(!is_acquire_) return DIERR_INPUTLOST;
		HRslt hr = ptr_->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), obd, &elemnum, 0);
		if(hr == DIERR_INPUTLOST) is_acquire_ = false;
		return hr;
	}

	/////////////////////
	// MoudeDevice
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
		//次に、ヘッダー (DIPROPDWORD 構造体内の DIPROPHEADER 構造体) のアドレスと、変更したいプロパティの識別子を、次のように IDirectInputDevice8::SetProperty メソッドに渡す。
		hr = get()->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

		x = y = dx = dy = dz = 0;
		memset(push, 0, sizeof(push));
		memset(press, 0, sizeof(press));
		memset(release, 0, sizeof(release));
		return hr;
	}

	HRslt MouseDevice::update()
	{
		HRslt hr;
		dx = dy = dz = 0;
		memset(push, 0, sizeof(push));
		memset(release, 0, sizeof(release));
		for(;;) {
			DIDEVICEOBJECTDATA od;
			DWORD dwElements = 1;   // 取得すべき項目数
			hr = getData(&od, dwElements);
			if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
				acquire();
				break;
			}
			/* データが読めない、または利用可能データがない。*/
			if(!hr || dwElements == 0) break;

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
            case DIMOFS_X:
				dx = od.dwData;
                break;
            case DIMOFS_Y:
				dy = od.dwData;
                break;
            case DIMOFS_Z:
				dz = od.dwData;
                break;
			}
		}
		return hr;
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
		memset(buffer_, 0, sizeof(buffer_));
		first_ = true;
		return hr;
	}

	HRslt KbdDevice::update()
	{
		HRslt hr;
		char buffer[BUFFER_SIZE];
		hr = getState(sizeof(buffer), &buffer);
		if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
			// 失敗の場合、デバイスは失われている。
			// (hr == DIERR_INPUTLOST) をチェックし、
			// 再取得を試みる必要がある。
			hr = acquire();
			if(hr) hr = getState(sizeof(buffer), &buffer);
		}
		if(!hr) memset(buffer, 0, sizeof(buffer));
		for(int i = 0; i < BUFFER_SIZE; i++) {
			char prev = buffer_[i];
			if(buffer[i]&0x80) buffer_[i] = PRESS|(prev&PRESS ? 0 : PUSH);
			else buffer_[i] = (prev&PRESS ? RELEASE : 0);
		}
		return hr;
	}

} // !gctp
