#ifndef _GCTP_FONT_HPP_
#define _GCTP_FONT_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult フォントリソースクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 21:29:23
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointer.hpp>
#include <boost/shared_ptr.hpp>
#include "smartwin/smartstring.h"
#include "smartwin/font.h"

namespace gctp {

	/** フォントリソース
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 0:36:12
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Font : public Object {
	public:
		/// スタイル定数
		enum Style {
			NORMAL = 0			/**< 通常*/,
			BOLD   = 1			/**< 太字*/,
			ITALIC = 1<<1		/**< 斜体*/,
			UNDERLINE = 1<<2	/**< 下線*/,
			STRIKEOUT = 1<<3	/**< 打ち消し線*/,
			_FORCE_DWORD_ = 0x7FFFFFFF,
		};
		
		/// フォント制作
		HRslt setUp(const _TCHAR *fontname, uint height, uint32_t flag=NORMAL);

		/// フォント制作
		HRslt setUp(const _TCHAR *name);

		/// フォントハンドルを取得
		HFONT get() const { return font_->getHandle(); }

		/// SmartWinフォントオブジェクトを取得
		SmartWin::FontPtr getFontPtr() const { return font_; }

		/// LOGFONT構造体を埋める
		bool getLogFont(LOGFONT &logfont) const
		{
			return font_.get() && font_->getHandle() && (::GetObject(font_->getHandle(), sizeof(LOGFONT), &logfont) == sizeof(LOGFONT));
		}

		/// 文字の高さを返します
		uint height() const { return height_; }

		/// 文字の最大の幅を返します
		int maxWidth() const;

		/// 文字セルのサイズを返します
		uint cellsize() const { return cellsize_; }

		/// フォントの高さから、文字セルサイズを計算する
		static uint cellsize(uint height);

		/// パラメータからsetUpに渡す文字列パラメータを作る補助関数
		static SmartWin::tstring paramToString(const _TCHAR *fontname, uint height, uint32_t style);

		/// 文字列パラメータから、各パラメータに変換する補助関数
		static bool stringToParam(const _TCHAR *param, SmartWin::tstring &fontname, uint &height, uint32_t &style);

	private:
		uint height_;
		uint cellsize_;
		SmartWin::FontPtr font_;
	};

} //namespace gctp

#endif //_GCTP_FONT_HPP_