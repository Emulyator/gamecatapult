#ifndef _GCTP_GRAPHIC_BRUSH_HPP_
#define _GCTP_GRAPHIC_BRUSH_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 描画処理クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:43:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic.hpp>
#include <gctp/class.hpp>

namespace gctp { namespace graphic {

	class Model;
	/// 描画手段クラス
	class Brush : public Object {
	public:
		Brush(Model &target) : target_(target) {}
		virtual ~Brush() {}
		virtual HRslt draw(const Skeleton &skl) const = 0;
		virtual HRslt draw(const Skeleton &skl, int mtrlno) const = 0;
		//virtual HRslt begin(int template_mtrlno) const = 0;
		//virtual HRslt draw(const Skeleton &skel, int mtrlno) const = 0;
		//virtual HRslt draw(const Matrix &mat, int mtrlno) const = 0;
		//virtual HRslt end() const = 0;
		Model &target_;
	};

}} //namespace gctp

#endif // _GCTP_GRAPHIC_BRUSH_HPP_