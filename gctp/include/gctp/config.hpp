#ifndef _GCTP_CONFIG_HPP_
#define _GCTP_CONFIG_HPP_
/** @file
 * GameCatapult �ݒ�t�@�C��
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */

// C++�W����RTTI���g�p����
#define GCTP_USE_STD_RTTI

// �v���p�e�B�[�@�\���g�p����
//#define GCTP_USE_PROPERTY

// ���I�v���p�e�B�[�@�\���g�p����
//#define GCTP_USE_DYNAMIC_PROPERTY

// zlib���g�p����
#define GCTP_USE_ZLIB

// once�v���O�}������
#define GCTP_ONCE

// D3DX���w���C�u�������g��
#ifndef GCTP_LITE
# define GCTP_USE_D3DXMATH
#endif

// �Öق̍��W�n���E��n
//#define GCTP_COORD_RH

// �g�ݍ��݃V�X�e�������̃Z�b�e�B���O
//#define GCTP_FOR_EMBEDDED	1

/// �G���f�B�A���X�w��
#define GCTP_LITTLE_ENDIAN
//#define GCTP_BIG_ENDIAN

#if defined(GCTP_FOR_EMBEDDED) && GCTP_FOR_EMBEDDED
// �n���h�����ʗʂ̍ő�l
#define GCTP_HANDLE_MAX		512
// ���X�g�m�[�h�̍ő�l
#define GCTP_LIST_MAX		512
#endif

#ifndef GCTP_USE_STD_RTTI
# ifdef GCTP_USE_PROPERTY
#  undef GCTP_USE_PROPERTY
# endif
# ifdef GCTP_USE_DYNAMIC_PROPERTY
#  undef GCTP_USE_DYNAMIC_PROPERTY
# endif
#endif

#if defined(_DEBUG) && !defined(GCTP_DEBUG)
# define GCTP_DEBUG	1
#endif

#ifdef _MSC_VER
# ifndef NOMINMAX
#  define NOMINMAX
# endif
# if _MSC_VER < 1310
# endif
#endif

#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE

#include <boost/config.hpp>

#endif //_GCTP_CONFIG_HPP_
