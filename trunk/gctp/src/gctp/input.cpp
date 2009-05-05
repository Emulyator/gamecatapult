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
#include <gctp/stringmap.hpp>

using namespace std;

namespace gctp {

	/////////////////////
	// Input
	//
	IDirectInput8Ptr Input::api_;

	Input::DeviceList Input::devicies_;

	namespace {
		// DIK->������̃}�b�v
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

	/** InputDevice���擾
	 *
	 * ���̎��_�ł́AAcquire���Ȃ��B@n
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
		// �w�b�_�[
		dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj        = 0;
		dipdw.diph.dwHow        = DIPH_DEVICE;
		// �f�[�^
		dipdw.dwData            = BUFFER_SIZE;
		// ���ɁA�w�b�_�[ (DIPROPDWORD �\���̓��� DIPROPHEADER �\����) �̃A�h���X�ƁA�ύX�������v���p�e�B�̎��ʎq���A���̂悤�� IDirectInputDevice8::SetProperty ���\�b�h�ɓn���B
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
		// �f�[�^
		dipdw.dwData  = BUFFER_SIZE;
		// ���ɁA�w�b�_�[ (DIPROPDWORD �\���̓��� DIPROPHEADER �\����) �̃A�h���X�ƁA�ύX�������v���p�e�B�̎��ʎq���A���̂悤�� IDirectInputDevice8::SetProperty ���\�b�h�ɓn���B
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
			/* �v�f�𒲂ׂĉ����������������m�F����B*/
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
		// �f�[�^
		dipdw.dwData  = BUFFER_SIZE;
		// ���ɁA�w�b�_�[ (DIPROPDWORD �\���̓��� DIPROPHEADER �\����) �̃A�h���X�ƁA�ύX�������v���p�e�B�̎��ʎq���A���̂悤�� IDirectInputDevice8::SetProperty ���\�b�h�ɓn���B
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
