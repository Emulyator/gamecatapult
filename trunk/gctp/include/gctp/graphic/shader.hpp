#ifndef _GCTP_GRAPHIC_SHADER_HPP_
#define _GCTP_GRAPHIC_SHADER_HPP_
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

namespace gctp { namespace graphic {

	/// シェーダーリソースクラス
	class Shader : public Rsrc {
	public:
		virtual HRslt setUp(const _TCHAR *fname) = 0;

		/// 指定のテクニックがあるか？
		virtual bool hasTechnique(const char *name) const = 0;
		/// 適用開始
		virtual HRslt begin() const = 0;
		/// 適用終了
		virtual HRslt end() const = 0;
		/// 指定のパスを適用開始
		virtual HRslt beginPass(uint passno) const = 0;
		/// 指定のパスを適用終了
		virtual HRslt endPass() const = 0;
		/// パス数を返す（begin~endの間のみ有効）
		virtual uint passnum() const = 0;

		// とりあえず。。。
		// NULLにすると先頭のテクニックで描画
		static void setTechnique(const char *techname)
		{
			current_technique_ = techname;
		}

		static const char *getTechnique()
		{
			return current_technique_;
		}

	protected:
		static const char *current_technique_;

	GCTP_DECLARE_TYPEINFO
	};

}} //namespace gctp

#endif // _GCTP_GRAPHIC_SHADER_HPP_