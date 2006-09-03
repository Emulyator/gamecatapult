#ifndef _GCTP_MATRIX2_HPP_
#define _GCTP_MATRIX2_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 2D用行列クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/math/matrix3x3.hpp>

namespace gctp {

	typedef math::Matrix3x3<float> Matrix2;
	typedef math::Matrix3x3C<float> Matrix2C;

} //namespace gctp

#endif //_GCTP_MATRIX2_HPP_