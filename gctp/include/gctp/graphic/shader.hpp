#ifndef _GCTP_GRAPHIC_SHADER_HPP_
#define _GCTP_GRAPHIC_SHADER_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult シェーダークラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:43:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/class.hpp>

namespace gctp {
	class Skeleton;
}

namespace gctp { namespace graphic {

	class Model;

	/// シェーダーリソースクラス
	class Shader : public Rsrc {
	public:
		virtual HRslt setUp(const _TCHAR *fname) = 0;

		/// 指定のテクニックがあるか？
		virtual bool hasTechnique(const char *technique) const = 0;
		/// 適用開始
		virtual HRslt begin(const char *technique = 0, uint passno = 0) const = 0;
		/// 適用終了
		virtual HRslt end() const = 0;

	GCTP_DECLARE_TYPEINFO
	};

}} //namespace gctp

#endif // _GCTP_GRAPHIC_SHADER_HPP_