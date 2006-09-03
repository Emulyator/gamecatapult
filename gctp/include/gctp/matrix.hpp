#ifndef _GCTP_MATRIX_HPP_
#define _GCTP_MATRIX_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 行列クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: matrix.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/def.hpp>
#include <gctp/math/matrix4x4.hpp>

namespace gctp {

	typedef math::Matrix4x4<Real> Matrix;
	typedef math::Matrix4x4C<Real> MatrixC;

} // namespace gctp

#endif //_GCTP_MATRIX_HPP_
