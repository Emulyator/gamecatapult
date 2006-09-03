/** @file
 * GameCatapult DirectInput�N���X
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
		os<<_T("   �f�o�C�X�^�C�v:");
		switch(GET_DIDEVICE_TYPE(device.dwDevType)) {
		case DI8DEVTYPE_1STPERSON:
			os<<_T("FPS�p : "); // �Ζʌ^�̃A�N�V���� �Q�[�� �f�o�C�X�B
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPE1STPERSON_LIMITED:
				os<<_T("�A�N�V���� �}�b�s���O�Ώۂ̃f�o�C�X �I�u�W�F�N�g�̍ŏ������w�肵�Ȃ��f�o�C�X�B");
				break;
			case DI8DEVTYPE1STPERSON_SHOOTER:
				os<<_T("FPS�p�ɐ݌v���ꂽ�f�o�C�X�B");
				break;
			case DI8DEVTYPE1STPERSON_SIXDOF:
				os<<_T("6 �i�K (3 �̉��������� 3 �̉�]��) �̎��R��������f�o�C�X�B");
				break;
			case DI8DEVTYPE1STPERSON_UNKNOWN:
				os<<_T("�s���ȃT�u�^�C�v�B");
				break;
			default:
				os<<_T("����`�̒l");
				break;
			}
			break;
		case DI8DEVTYPE_DEVICE:
			os<<_T("���ޕs�\");
			break;
		case DI8DEVTYPE_DEVICECTRL:
			os<<_T("�A�v���P�[�V�����̃R���e�L�X�g����ʃ^�C�v�̃f�o�C�X�̐���Ɏg�p�������̓f�o�C�X : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEDEVICECTRL_COMMSSELECTION:
				os<<_T("�ʐM�̑I���Ɏg�p�����R���g���[���B");
				break;
			case DI8DEVTYPEDEVICECTRL_COMMSSELECTION_HARDWIRED:
				os<<_T("�f�t�H���g�̍\�����g�p����K�v������A�}�b�s���O��ύX�ł��Ȃ��f�o�C�X�B");
				break;
			case DI8DEVTYPEDEVICECTRL_UNKNOWN:
				os<<_T("�s���ȃT�u�^�C�v�B");
				break;
			default:
				os<<_T("����`�̒l");
				break;
			}
			break;
		case DI8DEVTYPE_DRIVING:
			os<<_T("�X�e�A�����O : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEDRIVING_COMBINEDPEDALS:
				os<<_T("�P��̎�����A�N�Z���ƃu���[�L �y�_���̒l��񍐂���X�e�A�����O �f�o�C�X�B");
				break;
			case DI8DEVTYPEDRIVING_DUALPEDALS:
				os<<_T("�ʁX�̎�����A�N�Z���ƃu���[�L �y�_���̒l��񍐂���X�e�A�����O �f�o�C�X�B");
				break;
			case DI8DEVTYPEDRIVING_HANDHELD:
				os<<_T("�n���h�w���h�̃X�e�A�����O �f�o�C�X�B");
				break;
			case DI8DEVTYPEDRIVING_LIMITED:
				os<<_T("�A�N�V���� �}�b�s���O�Ώۂ̃f�o�C�X �I�u�W�F�N�g�̍ŏ������w�肵�Ȃ��X�e�A�����O �f�o�C�X�B");
				break;
			case DI8DEVTYPEDRIVING_THREEPEDALS:
				os<<_T("�ʁX�̎�����A�N�Z���A�u���[�L�A����уN���b�` �y�_���̒l��񍐂���X�e�A�����O �f�o�C�X�B");
				break;
			default:
				os<<_T("����`�̒l");
				break;
			}
			break;
		case DI8DEVTYPE_FLIGHT:
			os<<_T("�t���C�g �V�~�����[�V�����p�R���g���[�� : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEFLIGHT_LIMITED:
				os<<_T("�A�N�V���� �}�b�s���O�Ώۂ̃f�o�C�X �I�u�W�F�N�g�̍ŏ������w�肵�Ȃ��t���C�g �R���g���[���B");
				break;
			case DI8DEVTYPEFLIGHT_RC:
				os<<_T("�͌^��s�@�̃����[�g �R���g���[���Ɋ�Â��t���C�g �f�o�C�X�B");
				break;
			case DI8DEVTYPEFLIGHT_STICK:
				os<<_T("�W���C�X�e�B�b�N�B");
				break;
			case DI8DEVTYPEFLIGHT_YOKE:
				os<<_T("���c�ցB");
				break;
			default:
				os<<_T("����`�̒l");
				break;
			}
			break;
		case DI8DEVTYPE_GAMEPAD:
			os<<_T("�Q�[���p�b�h : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEGAMEPAD_LIMITED:
				os<<_T("�A�N�V���� �}�b�s���O�Ώۂ̃f�o�C�X �I�u�W�F�N�g�̍ŏ������w�肵�Ȃ��Q�[���p�b�h�B");
				break;
			case DI8DEVTYPEGAMEPAD_STANDARD:
				os<<_T("�A�N�V���� �}�b�s���O�Ώۂ̃f�o�C�X �I�u�W�F�N�g�̍ŏ������w�肵�Ȃ��W���Q�[���p�b�h�B");
				break;
			case DI8DEVTYPEGAMEPAD_TILT:
				os<<_T("�R���g���[���̎p������ x ���� y ���̃f�[�^��񍐂���Q�[���p�b�h�B");
				break;
			default:
				os<<_T("����`�̒l");
				break;
			}
			break;
		case DI8DEVTYPE_JOYSTICK:
			os<<_T("�W���C�X�e�B�b�N : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEJOYSTICK_LIMITED:
				os<<_T("�A�N�V���� �}�b�s���O�Ώۂ̃f�o�C�X �I�u�W�F�N�g�̍ŏ������w�肵�Ȃ��W���C�X�e�B�b�N�B");
				break;
			case DI8DEVTYPEJOYSTICK_STANDARD:
				os<<_T("�A�N�V���� �}�b�s���O�Ώۂ̃f�o�C�X �I�u�W�F�N�g�̍ŏ������w�肷��W���W���C�X�e�B�b�N�B");
				break;
			default:
				os<<_T("����`�̒l");
				break;
			}
			break;
		case DI8DEVTYPE_KEYBOARD:
			os<<_T("�L�[�{�[�h�܂��̓L�[�{�[�h�ގ��f�o�C�X : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEKEYBOARD_UNKNOWN:
				os<<_T("�T�u�^�C�v�͊m��ł��Ȃ��B");
				break;
			case DI8DEVTYPEKEYBOARD_PCXT:
				os<<_T("IBM PC/XT 83 �L�[ �L�[�{�[�h");
				break;
			case DI8DEVTYPEKEYBOARD_OLIVETTI:
				os<<_T("Olivetti 102 �L�[ �L�[�{�[�h");
				break;
			case DI8DEVTYPEKEYBOARD_PCAT:
				os<<_T("IBM PC/AT 84 �L�[ �L�[�{�[�h");
				break;
			case DI8DEVTYPEKEYBOARD_PCENH:
				os<<_T("IBM PC �g�� 101/102 �L�[���L�[�{�[�h�܂��� Microsoft �i�`������ �L�[�{�[�h");
				break;
			case DI8DEVTYPEKEYBOARD_NOKIA1050:
				os<<_T("Nokia 1050 �L�[�{�[�h");
				break;
			case DI8DEVTYPEKEYBOARD_NOKIA9140:
				os<<_T("Nokia 9140 �L�[�{�[�h");
				break;
			case DI8DEVTYPEKEYBOARD_NEC98:
				os<<_T("���{�� NEC PC98 �L�[�{�[�h");
				break;
			case DI8DEVTYPEKEYBOARD_NEC98LAPTOP:
				os<<_T("���{�� NEC PC98 ���b�v�g�b�v �L�[�{�[�h");
				break;
			case DI8DEVTYPEKEYBOARD_NEC98106:
				os<<_T("���{�� NEC PC98 106 �L�[ �L�[�{�[�h");
				break;
			case DI8DEVTYPEKEYBOARD_JAPAN106:
				os<<_T("���{�� 106 �L�[ �L�[�{�[�h");
				break;
			case DI8DEVTYPEKEYBOARD_JAPANAX:
				os<<_T("���{�� AX �L�[�{�[�h");
				break;
			case DI8DEVTYPEKEYBOARD_J3100:
				os<<_T("���{�� J3100 �L�[�{�[�h");
				break;
			default:
				os<<_T("����`�̒l");
				break;
			}
			break;
		case DI8DEVTYPE_MOUSE:
			os<<_T("�}�E�X�܂��̓}�E�X�ގ��f�o�C�X (�g���b�N�{�[���Ȃ�) : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEMOUSE_ABSOLUTE:
				os<<_T("��Ύ��f�[�^��Ԃ��}�E�X�B");
				break;
			case DI8DEVTYPEMOUSE_FINGERSTICK:
				os<<_T("�t�B���K�[�X�e�B�b�N�B");
				break;
			case DI8DEVTYPEMOUSE_TOUCHPAD:
				os<<_T("�^�b�`�p�b�h�B");
				break;
			case DI8DEVTYPEMOUSE_TRACKBALL:
				os<<_T("�g���b�N�{�[���B");
				break;
			case DI8DEVTYPEMOUSE_TRADITIONAL:
				os<<_T("�]���^�}�E�X�B");
				break;
			case DI8DEVTYPEMOUSE_UNKNOWN:
				os<<_T("�T�u�^�C�v�͊m��ł��Ȃ��B");
				break;
			default:
				os<<_T("����`�̒l");
				break;
			}
			break;
		case DI8DEVTYPE_REMOTE:
			os<<_T("�����[�g �R���g���[�� �f�o�C�X : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPEREMOTE_UNKNOWN:
				os<<_T("�T�u�^�C�v�͊m��ł��Ȃ��B");
				break;
			default:
				os<<_T("����`�̒l");
				break;
			}
			break;
		case DI8DEVTYPE_SCREENPOINTER:
			os<<_T("�X�N���[�� �|�C���^ : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPESCREENPTR_UNKNOWN:
				os<<_T("�s���ȃT�u�^�C�v");
				break;
			case DI8DEVTYPESCREENPTR_LIGHTGUN:
				os<<_T("���C�g �K���B");
				break;
			case DI8DEVTYPESCREENPTR_LIGHTPEN:
				os<<_T("���C�g �y���B");
				break;
			case DI8DEVTYPESCREENPTR_TOUCH:
				os<<_T("�^�b�` �X�N���[���B");
				break;
			default:
				os<<_T("����`�̒l");
				break;
			}
			break;
		case DI8DEVTYPE_SUPPLEMENTAL:
			os<<_T("�z�C�[���Ƌ��Ɏg�p����y�_���ȂǁA�A�v���P�[�V�����̃��C�� �R���g���[���ɓK���Ȃ��@�\�����������f�o�C�X : ");
			switch(GET_DIDEVICE_SUBTYPE(device.dwDevType)) {
			case DI8DEVTYPESUPPLEMENTAL_2NDHANDCONTROLLER:
				os<<_T("2 ���I�n���h�w���h �R���g���[���B");
				break;
			case DI8DEVTYPESUPPLEMENTAL_COMBINEDPEDALS:
				os<<_T("��@�\�Ƃ��āA�P��̎�����A�N�Z���ƃu���[�L �y�_���̒l��񍐂���f�o�C�X�B");
				break;
			case DI8DEVTYPESUPPLEMENTAL_DUALPEDALS:
				os<<_T("��@�\�Ƃ��āA�ʁX�̎�����A�N�Z���ƃu���[�L �y�_���̒l��񍐂���f�o�C�X�B");
				break;
			case DI8DEVTYPESUPPLEMENTAL_HANDTRACKER:
				os<<_T("��̈ړ���ǐՂ���f�o�C�X�B");
				break;
			case DI8DEVTYPESUPPLEMENTAL_HEADTRACKER:
				os<<_T("���̈ړ���ǐՂ���f�o�C�X�B");
				break;
			case DI8DEVTYPESUPPLEMENTAL_RUDDERPEDALS:
				os<<_T("�����ǃy�_����������f�o�C�X�B");
				break;
			case DI8DEVTYPESUPPLEMENTAL_SHIFTER:
				os<<_T("������M�A�I����񍐂���f�o�C�X�B");
				break;
			case DI8DEVTYPESUPPLEMENTAL_SHIFTSTICKGATE:
				os<<_T("�{�^����Ԃ���M�A�I����񍐂���f�o�C�X�B");
				break;
			case DI8DEVTYPESUPPLEMENTAL_SPLITTHROTTLE:
				os<<_T("��@�\�Ƃ��āA2 �ȏ�̃X���b�g���l��񍐂���f�o�C�X�B���̑��̃R���g���[�������ꍇ������B");
				break;
			case DI8DEVTYPESUPPLEMENTAL_THREEPEDALS:
				os<<_T("��@�\�Ƃ��āA�ʁX�̎�����A�N�Z���A�u���[�L�A����уN���b�` �y�_���̒l��񍐂���f�o�C�X�B");
				break;
			case DI8DEVTYPESUPPLEMENTAL_THROTTLE:
				os<<_T("��@�\�Ƃ��āA�P��̃X���b�g���l��񍐂���f�o�C�X�B���̑��̃R���g���[�������ꍇ������B");
				break;
			case DI8DEVTYPESUPPLEMENTAL_UNKNOWN:
				os<<_T("�s���ȃT�u�^�C�v�B");
				break;
			default:
				os<<_T("����`�̒l");
				break;
			}
			break;
		default:
			os<<_T("����`�̒l");
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

	/** ������
	 *
	 * API�Z�b�g���擾
	 */
	HRslt Input::initialize(
		HINSTANCE hinst	///< �A�v���P�[�V�����̃C���X�^���X
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

	Input *Input::input_ = NULL;	// �J�����g�C���X�^���X
	
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

	/** InputDevice���擾
	 *
	 * ���̎��_�ł́AAcquire���Ȃ��B<BR>
	 * �h���N���X�ŁA���S�ȃZ�b�g�A�b�v���I��������_�ŁAacquire����K�v������B
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
		// �w�b�_�[
		dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj        = 0;
		dipdw.diph.dwHow        = DIPH_DEVICE;
		// �f�[�^
		dipdw.dwData            = BUFFER_SIZE;
		//���ɁA�w�b�_�[ (DIPROPDWORD �\���̓��� DIPROPHEADER �\����) �̃A�h���X�ƁA�ύX�������v���p�e�B�̎��ʎq���A���̂悤�� IDirectInputDevice8::SetProperty ���\�b�h�ɓn���B
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
			DWORD dwElements = 1;   // �擾���ׂ����ڐ�
			hr = getData(&od, dwElements);
			if(hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
				acquire();
				break;
			}
			/* �f�[�^���ǂ߂Ȃ��A�܂��͗��p�\�f�[�^���Ȃ��B*/
			if(!hr || dwElements == 0) break;

			/* �v�f�𒲂ׂĉ����������������m�F����B*/
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
			// ���s�̏ꍇ�A�f�o�C�X�͎����Ă���B
			// (hr == DIERR_INPUTLOST) ���`�F�b�N���A
			// �Ď擾�����݂�K�v������B
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
