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
			FIXEDPITCH = 1<<4	/**< 固定ピッチ*/,
			SHADOW = 1<<5	/**< 影文字(袋文字と同時に指定した場合袋文字が有効になる)*/,
			OUTLINE = 1<<6	/**< 袋文字（輪郭付）*/,
			_FORCE_DWORD_ = 0x7FFFFFFF,
		};
		enum ExStyle {
			EX_NONE,
			EX_SHADOW,
			EX_OUTLINE,
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
		uint cellsize() const {
			// 影文字、袋文字の場合の補正
			if(exstyle_ == EX_OUTLINE) return cellsize_ + 2;
			else if(exstyle_ == EX_SHADOW) return cellsize_ + 1;
			return cellsize_;
		}

		ExStyle exStyle() const { return exstyle_; }

		/// フォントの高さから、文字セルサイズを計算する
		static uint cellsize(uint height);

		/// パラメータからsetUpに渡す文字列パラメータを作る補助関数
		static SmartUtil::tstring paramToString(const _TCHAR *fontname, uint height, uint32_t style);

		/// 文字列パラメータから、各パラメータに変換する補助関数
		static bool stringToParam(const _TCHAR *param, SmartUtil::tstring &fontname, uint &height, uint32_t &style);

	private:
		ExStyle exstyle_;
		uint height_;
		uint cellsize_;
		SmartWin::FontPtr font_;
	};

} //namespace gctp

#endif //_GCTP_FONT_HPP_