#ifndef _GCTP_COMMON_H_
#define _GCTP_COMMON_H_
/** @file
 * プリコンパイルヘッダ生成用ヘッダファイル
 *
 * 実装ファイルの先頭でこれをインクルード
 *
 * ついでにいろいろ共通のプリプロセッサ定義も行う
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
// こうしないとisleadbyteが二重定義になる
#define _WCTYPE_INLINE_DEFINED
#endif

#include <iosfwd>
#include <cstddef>
#include <algorithm>

#include <gctp/config.hpp>

#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します
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
// Platform SDKによってはこれがない場合がある？ Boost.Poolのmutexでこれがなくてこける場合がある
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