#ifndef _GCTP_CONFIG_HPP_
#define _GCTP_CONFIG_HPP_
/** @file
 * GameCatapult 設定ファイル
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */

// C++標準のRTTIを使用する
#define GCTP_USE_STD_RTTI

// プロパティー機構を使用する
//#define GCTP_USE_PROPERTY

// 動的プロパティー機構を使用する
//#define GCTP_USE_DYNAMIC_PROPERTY

// zlibを使用する
#define GCTP_USE_ZLIB

// onceプラグマがある
#define GCTP_ONCE

// D3DX数学ライブラリを使う
#ifndef GCTP_LITE
# define GCTP_USE_D3DXMATH
#endif

// 暗黙の座標系が右手系
//#define GCTP_COORD_RH

// 組み込みシステム向けのセッティング
//#define GCTP_FOR_EMBEDDED	1

/// エンディアンス指定
#define GCTP_LITTLE_ENDIAN
//#define GCTP_BIG_ENDIAN

#if defined(GCTP_FOR_EMBEDDED) && GCTP_FOR_EMBEDDED
// ハンドル流通量の最大値
#define GCTP_HANDLE_MAX		512
// リストノードの最大値
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
