// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���A
//            �܂��͎Q�Ɖ񐔂������A�����܂�ύX����Ȃ�
//            �v���W�F�N�g��p�̃C���N���[�h �t�@�C�����L�q���܂��B
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#pragma warning(disable:4786 4503)
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Windows �w�b�_�[����w�ǎg�p����Ȃ��X�^�b�t�����O���܂�
#define _WIN32_IE 0x0550

#include <SmartWin.h>
#define WIN32_LEAN_AND_MEAN		// Windows �w�b�_�[����w�ǎg�p����Ȃ��X�^�b�t�����O���܂�
#include <windows.h>
#define COM_NO_WINDOWS_H
#include <objbase.h>
#include <mmsystem.h>
#include <tchar.h>

#ifdef GCTP_LITE
# include <d3d8.h>
# include <d3d8types.h>
# define DIRECTSOUND_VERSION (0x0800)
# include <dsound.h>
# define DIRECTINPUT_VERSION (0x0800)
# include <dinput.h>
#else
# include <d3d9.h>
# include <d3d9types.h>
# define D3D_OVERLOADS
# include <d3dx9.h>
# include <dxfile.h>
# include <dsound.h>
# define DIRECTINPUT_VERSION (0x0800)
# include <dinput.h>
#endif

#if defined(_DEBUG) && !defined(DEBUG)
#define DEBUG	1
#endif

#if _MSC_VER < 1300
#define for if(0); else for
#else
#pragma conform(forScope, on)
#endif

#include <gctp/def.hpp>

// TODO: �v���O�����ŕK�v�ȃw�b�_�[�Q�Ƃ�ǉ����Ă��������B

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
