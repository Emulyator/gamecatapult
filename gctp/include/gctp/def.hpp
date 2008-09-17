#ifndef _GCTP_DEF_HPP_
#define _GCTP_DEF_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 共通定義ヘッダファイル
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: def.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */

/// GameCatapultを構成するほとんどの要素は、このgctpに属する
namespace gctp {
	typedef unsigned char		uchar;
	typedef unsigned int		uint;
	typedef unsigned short		ushort;
	typedef unsigned long		ulong;
	typedef unsigned __int64	uhyper;

	typedef int					int_t;
	typedef unsigned int		uint_t;
#if 0
	typedef __int8				int8_t;
	typedef unsigned __int8		uint8_t;
	typedef __int16				int16_t;
	typedef unsigned __int16	uint16_t;
	typedef __int32				int32_t;
	typedef unsigned __int32	uint32_t;
	typedef __int64				int64_t;
	typedef unsigned __int64	uint64_t;
#else
	typedef char				int8_t;
	typedef unsigned char		uint8_t;
	typedef short				int16_t;
	typedef unsigned short		uint16_t;
	typedef long				int32_t;
	typedef unsigned long		uint32_t;
	typedef __int64				int64_t;
	typedef unsigned __int64	uint64_t;
#endif
	typedef float Real;
	const Real g_pi = 3.141592654f;
	const Real g_root2 = 1.41421356f;
	inline Real toDeg(Real rad) { return rad/g_pi*180.0f; }
	inline Real toRad(Real deg) { return deg/180.0f*g_pi; }

	template<typename _Type>
	inline _Type abs(_Type x)
	{
		return x > 0 ? x : -x;
	}
} // namespace gctp

#define GCTP_TLS	__declspec( thread )
#define GCTP_NAKED	__declspec( naked )

#ifdef __MWERKS__
#define GCTP_MWERKS_OPTIMIZATION_LEVEL __option(opt_dead_code)
#else
#define GCTP_MWERKS_OPTIMIZATION_LEVEL 0
#endif

#define GCTP_SHIFT_JIS 1

#endif //_GCTP_DEF_HPP_