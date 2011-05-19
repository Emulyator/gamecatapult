#ifndef _GCTP_COMMON_H_
#define _GCTP_COMMON_H_
/** @file
 * �v���R���p�C���w�b�_�����p�w�b�_�t�@�C��
 *
 * �����t�@�C���̐擪�ł�����C���N���[�h
 *
 * ���łɂ��낢�다�ʂ̃v���v���Z�b�T��`���s��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 2:44:48
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#if _MSC_VER > 1000
#pragma once
#pragma warning(disable:4786 4503)
#endif // _MSC_VER > 1000

#ifdef _DEBUG
// �������Ȃ���isleadbyte����d��`�ɂȂ�
#define _WCTYPE_INLINE_DEFINED
#endif

#include <iosfwd>
#include <cstddef>
#include <algorithm>

#include <gctp/config.hpp>

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

#if (defined(WIN32) || defined(_WIN32)) && !defined(__WIN32__)
// Platform SDK�ɂ���Ă͂��ꂪ�Ȃ��ꍇ������H Boost.Pool��mutex�ł��ꂪ�Ȃ��Ă�����ꍇ������
# define __WIN32__
#endif

#ifdef _MSC_VER
# if _MSC_VER < 1300
#  define for if(0); else for
# else
#  pragma conform(forScope, on)
# endif
#endif

#include <gctp/def.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/com_ptr.hpp>
#include <gctp/dbgout.hpp>

#endif //_GCTP_COMMON_H_