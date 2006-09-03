#ifndef _GCTP_PLANE_HPP_
#define _GCTP_PLANE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 平面クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/math/plane3d.hpp>

namespace gctp {

	typedef math::Plane3d<float> Plane;
	typedef math::Plane3dC<float> PlaneC;

} //namespace gctp

#endif //_GCTP_PLANE_HPP_
