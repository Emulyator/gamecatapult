#ifndef _PCH_H_
#define _PCH_H_
/*
 * �v���R���p�C���w�b�_
 */

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#pragma warning(disable:4786 4503)
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Windows �w�b�_�[����w�ǎg�p����Ȃ��X�^�b�t�����O���܂�
#include <windows.h>
#endif

#include <lwhost.h>
#include <lwserver.h>
#include <lwgeneric.h>
#include <lwxpanel.h>

#ifdef _MSC_VER
#define for if(0); else for
#endif

#endif //_PCH_H_
