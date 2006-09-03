#ifndef _GCTP_QUAT_HPP_
#define _GCTP_QUAT_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult クォータニオンクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: quat.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/math/quaternion.hpp>

namespace gctp {

	typedef gctp::math::Quaternion<float> Quat;
	typedef gctp::math::QuaternionC<float> QuatC;

} // namespace gctp

#endif //_GCTP_QUAT_HPP_