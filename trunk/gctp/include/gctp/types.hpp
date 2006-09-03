#ifndef _GCTP_TYPES_HPP_
#define _GCTP_TYPES_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 汎用型定義クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/math/vector2d.hpp>
#include <gctp/math/vector3d.hpp>
#include <gctp/math/rectangle.hpp>

namespace gctp {

	/// ２次元点
	typedef math::Vector2d<int> Point2;
	typedef math::Vector2dC<int> Point2C;
	typedef Point2 Size2;
	typedef Point2C Size2C;

	/// ３次元整数点
	typedef math::Vector3d<int> Point3;
	typedef math::Vector3dC<int> Point3C;
	typedef Point3 Size3;
	typedef Point3C Size3C;

	/// 矩形
	typedef math::Rectangle<unsigned long> Rect;
	typedef math::RectangleC<unsigned long> RectC;
	typedef math::Rectangle<float> Rectf;
	typedef math::RectangleC<float> RectfC;

} //namespace gctp

#endif //_GCTP_TYPES_HPP_