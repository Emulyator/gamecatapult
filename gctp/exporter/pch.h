#ifndef _PCH_H_
#define _PCH_H_
/*
 * プリコンパイルヘッダ
 */

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#pragma warning(disable:4786 4503)
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します
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
