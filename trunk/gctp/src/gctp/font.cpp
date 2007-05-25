/** @file
 * GameCatapult フォントリソースクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 22:56:37
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/types.hpp>
#include <gctp/font.hpp>
#include <gctp/tcsv.hpp>
#include <gctp/dbgout.hpp>
#include "SmartWin.h"

using namespace std;

namespace gctp {

	/** フォント作成
	 *
	 * heightはピクセルサイズで指定。マップモードはMM_TEXTを前提にしている。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 2:12:19
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Font::setUp(
		const _TCHAR *facename /**< フォント名*/,
		uint          height   /**< サイズ*/,
		uint32_t      style    /**< スタイル(BOLD,ITALIC)*/
	) {
		if(style & OUTLINE) exstyle_ = EX_OUTLINE;
		else if(style & SHADOW) exstyle_ = EX_SHADOW;
		else exstyle_ = EX_NONE;
		height_ = height;
		// 論理サイズに変換。論理デバイスでの縦のピクセル/インチ でかけてデフォルトのdpiである72で割ってやると、
		// インチでの高さの値が出てくる、と。-にすると、文字セルの高さから内部レディング
		// (アクセント記号などのためのスペース) の高さが引かれる
		int  _height   = -(int)cellsize(height);
		int  _weight   = (style & BOLD)   ? FW_BOLD : FW_NORMAL;
		cellsize_ = -_height;
		// フォント制作。ANTIALIASED_QUALITYを指定しているが、アンチがかかるかどうかはシステムに依存するので
		// 保証されない。
		font_ = SmartWin::FontPtr(new SmartWin::Font(facename
			, _height, 0, _weight, DEFAULT_CHARSET
			, (style & ITALIC) ? true : false
			, (style & UNDERLINE) ? true : false
			, (style & STRIKEOUT) ? true : false
			, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
			, ANTIALIASED_QUALITY, (style & FIXEDPITCH) ? FIXED_PITCH : VARIABLE_PITCH));
		return font_ ? S_OK : E_FAIL;
	}

	/** すべて文字列でパラメータを渡すバージョン。
	 *
	 * カンマ区切りで、パラメータを渡す。
	 * "Lucida Console,18,NORMAL" "MS UI Gothic,10,BOLD|ITALIC"のように。
	 *
	 * gctp::createOnDBで制作する場合、こっちが使われる
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 23:07:15
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Font::setUp(const _TCHAR *name)
	{
		SmartUtil::tstring fontname(name);
		uint height;
		uint32_t style;
		if(!stringToParam(name, fontname, height, style)) return E_INVALIDARG;
		return setUp(fontname.c_str(), height, style);
	}

	/** 事前にフォントサイズからセルサイズを確認
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 6:32:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	uint Font::cellsize(uint height)
	{
		HDC hdc = ::CreateCompatibleDC(0);
		::SetMapMode(hdc, MM_TEXT);
		uint ret = ::MulDiv(height, ::GetDeviceCaps(hdc, LOGPIXELSY), 72);
		::DeleteDC(hdc);
		return ret;
	}

	int Font::maxWidth() const
	{
		HDC hdc = ::CreateCompatibleDC(0);
		::SetMapMode(hdc, MM_TEXT);

		SelectFont(hdc, font_->getHandle()); // マクロなので

		::TEXTMETRIC tm; // 現在のフォント情報
		::GetTextMetrics(hdc, &tm);
		::DeleteDC(hdc);
		// 影文字、袋文字の場合の補正
		if(exstyle_ == EX_OUTLINE) tm.tmMaxCharWidth += 2;
		else if(exstyle_ == EX_SHADOW) tm.tmMaxCharWidth += 1;
		return tm.tmMaxCharWidth;
	}

	/** パラメータからsetUpに渡す文字列パラメータを作る補助関数
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 1:46:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	SmartUtil::tstring Font::paramToString(const _TCHAR *fontname, uint height, uint32_t style)
	{
		CSVRowBase< _TCHAR, std::char_traits<_TCHAR> > csv;
		csv.push_back(fontname);
		csv.push_back(boost::lexical_cast<SmartUtil::tstring>(height));
		csv.push_back(boost::lexical_cast<SmartUtil::tstring>(style));
#ifdef _UNICODE
		wstringstream ioss;
#else
		stringstream ioss;
#endif
		ioss << csv;
		return ioss.str();
	}

	/** 文字列パラメータから、各パラメータに変換
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 1:46:35
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool Font::stringToParam(const _TCHAR *param, SmartUtil::tstring &fontname, uint &height, uint32_t &style)
	{
		basic_stringstream<_TCHAR> ioss;
		ioss << param;
		TCSVRow csv;
		ioss >> csv;
		if(csv.size() != 3) return false;
		fontname = csv[0];
		height = boost::lexical_cast<ulong>(csv[1]);
		style = 0;
		if(string::npos!=csv[2].find(_T("BOLD"))) style |= BOLD;
		if(string::npos!=csv[2].find(_T("ITALIC"))) style |= ITALIC;
		if(string::npos!=csv[2].find(_T("UNDERLINE"))) style |= UNDERLINE;
		if(string::npos!=csv[2].find(_T("STRIKEOUT"))) style |= STRIKEOUT;
		if(string::npos!=csv[2].find(_T("FIXEDPITCH"))) style |= FIXEDPITCH;
		if(string::npos!=csv[2].find(_T("SHADOW"))) style |= SHADOW;
		if(string::npos!=csv[2].find(_T("OUTLINE"))) style |= OUTLINE;
		return true;
	}

} // namespace gctp
