/**@file
 * GameCatapult フォントテクスチャリソースクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 1:42:30
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/types.hpp>
#include <gctp/color.hpp>
#include <gctp/graphic/asciifonttexture.hpp>
#include <gctp/graphic/spritebuffer.hpp>
#include <gctp/font.hpp>
#include <gctp/allocmap.hpp>
#include <gctp/utils.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	GCTP_IMPLEMENT_CLASS_NS2(gctp, graphic, AsciiFontTexture, Texture);

	namespace detail {
		struct AsciiAttr {
			ulong cellsize;
			Rectf tex_coords[128-32];
			int max_width;
			float scale;
			int idx;
			int priority;
		};
	}

	namespace {
		
		typedef std::map<Handle<Font>, detail::AsciiAttr> AsciiAttrMap;

		class DC {
		public:
			HDC hdc;
			DC() : hdc(::CreateCompatibleDC(0))
			{
			}
			~DC()
			{
				::DeleteDC(hdc);
			}

			HFONT selectFont(HFONT font)
			{
				return (HFONT)::SelectObject(hdc, font);
			}

			HBITMAP selectBitmap(HBITMAP bitmap)
			{
				return (HBITMAP)::SelectObject(hdc, bitmap);
			}

			int setMapMode(int mode)
			{
				return ::SetMapMode(hdc, mode);
			}

			bool getTextMetrics(LPTEXTMETRIC ptm)
			{
				return ::GetTextMetrics(hdc, ptm) != 0;
			}

			COLORREF setTextColor(COLORREF rgb)
			{
				return ::SetTextColor(hdc, rgb);
			}

			COLORREF setBkColor(COLORREF rgb)
			{
				return ::SetBkColor(hdc, rgb);
			}
			
			UINT setTextAlign(UINT align)
			{
				return ::SetTextAlign(hdc, align);
			}

			DWORD getGlyphOutline(UINT uChar, UINT fuFormat
				, LPGLYPHMETRICS lpgm, DWORD cjBuffer, LPVOID pvBuffer, CONST MAT2 *lpmat2)
			{
				return ::GetGlyphOutline(hdc, uChar, fuFormat, lpgm, cjBuffer, pvBuffer, lpmat2);
			}

			bool getTextExtent(LPCTSTR lpszString, int nCount, LPSIZE lpSize)
			{
				if(nCount == -1) nCount = lstrlen(lpszString);
				return (::GetTextExtentPoint32(hdc, lpszString, nCount, lpSize) != 0);
			}
			
			BOOL drawText(
				int X,            // 開始位置（基準点）の x 座標
				int Y,            // 開始位置（基準点）の y 座標
				UINT fuOptions,   // 長方形領域の使い方のオプション
				CONST RECT *lprc, // 長方形領域の入った構造体へのポインタ
				LPCTSTR lpString, // 文字列
				UINT cbCount,     // 文字数
				CONST INT *lpDx   // 文字間隔の入った配列
			)
			{
				return ExtTextOut(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
			}

			HBITMAP createDIBSection(
				CONST BITMAPINFO *pbmi,  // ビットマップデータ
				UINT iUsage,             // データ種類のインジケータ
				VOID **ppvBits,          // ビット値
				HANDLE hSection,         // ファイルマッピングオブジェクトのハンドル
				DWORD dwOffset           // ビットマップのビット値へのオフセット
			)
			{
				return CreateDIBSection(hdc, pbmi, iUsage, ppvBits, hSection, dwOffset);
			}
		};

	}

#define __GLYPH_OUTLINE_ // GetGlyphOutlineで文字を書く（そうでない場合はExtTextOutでDIBに書いてコピー）

	namespace detail {

		class AsciiDrawContext : public Object {
		public:
			DC dc;
			Texture::ScopedLock tex;
#ifndef __GLYPH_OUTLINE_
			uint32_t *buf;
			Bitmap bitmap;
#endif
#ifdef __GLYPH_OUTLINE_
			AsciiDrawContext(AsciiFontTexture &ftex) : tex(ftex, 0) {
				dc.setTextColor(RGB(255,255,255));
				dc.setBkColor(0x00000000);
				dc.setTextAlign(TA_TOP);
			}
#else
			AsciiDrawContext(AsciiFontTexture &ftex) : buf(0), tex(ftex, 0) {
				BITMAPINFO bmi;
				ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
				bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biWidth       =  ftex.maxCellSize();
				bmi.bmiHeader.biHeight      = -bmi.bmiHeader.biWidth;
				bmi.bmiHeader.biPlanes      = 1;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biBitCount    = 32;
				bitmap.setUp(dc.createDIBSection(&bmi, DIB_RGB_COLORS, (void**)&buf, NULL, 0));
				dc.selectBitmap( bitmap.bitmap );
				dc.setTextColor(RGB(255,255,255));
				dc.setBkColor(0x00000000);
				dc.setTextAlign(TA_TOP);
			}
#endif
		};

		class AsciiFontTextureDetail {
		public:
			AllocBitMap<AsciiFontTexture::LEVEL> alloc_;
			AsciiAttrMap map_;
		};
	}

	namespace {

		enum DrawMode {
			DM_NORMAL, // 只描画
			DM_SHADOW, // 影文字
			DM_OUTLINE, // 袋文字
		};

		uint16_t bkcolor__ = 0x0000;

		void draw(detail::AsciiDrawContext *cntx, int bitmapsize, const Size2 &tsize, HFONT font, int c, int x, int y, Size2 &gsize, DrawMode mode)
		{
#ifdef __GLYPH_OUTLINE_
			cntx->dc.selectFont( font );
			cntx->dc.setMapMode( MM_TEXT );
			std::basic_string<_TCHAR> str = charToStr(c);
			TEXTMETRIC tm; // 現在のフォント情報
			cntx->dc.getTextMetrics(&tm);
			MAT2 mat2 = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
			GLYPHMETRICS glyph;
			
			ulong bufsize = cntx->dc.getGlyphOutline(c, GGO_GRAY4_BITMAP, &glyph, 0, NULL, &mat2);
			uint8_t *buf = new uint8_t[bufsize]; // 実際にバッファに取得
			// アウトラインをグレースケールで作成
			cntx->dc.getGlyphOutline(c, GGO_GRAY4_BITMAP, &glyph, bufsize, (void *)buf, &mat2);

			cntx->dc.getTextExtent(str.c_str(), (int)str.length(), (SIZE *)&gsize);

			int ggo_pitch = ( glyph.gmBlackBoxX + 3 ) & 0xfffc; // ダブルワード境界に整列
			int ggo_xofs = glyph.gmptGlyphOrigin.x;
			int ggo_yofs = tm.tmAscent-glyph.gmptGlyphOrigin.y;
			int ggo_width = ggo_xofs+glyph.gmBlackBoxX;
			int ggo_height = ggo_yofs+glyph.gmBlackBoxY;

			//PRNN(str<<":"<<gsize.x<<","<<gsize.y);
			uint16_t *dst = (uint16_t *)cntx->tex.lr.buf;
			if(mode == DM_NORMAL) {
				uint8_t alpha;
				for(int dy = 0; dy < bitmapsize; dy++) {
					if(ggo_yofs <= dy && dy < ggo_height) {
						for(int dx = 0; dx < bitmapsize; dx++ ) {
							if(ggo_xofs <= dx && dx < ggo_width) {
								alpha = buf[ggo_pitch*(dy-ggo_yofs) + (dx-ggo_xofs)];
								if(alpha) {
									dst[(y+dy)*tsize.y+(x+dx)] = (((alpha-1)&0xf) << 12) | 0x0fff;
								}
								else dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
							}
							else dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
						}
					}
					else {
						for(int dx = 0; dx < bitmapsize; dx++) dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
					}
				}
			}
			else {
				uint8_t srcalpha;
				uint16_t dstcolor, srccolor;
				int loop_count_max = 1;
				if(mode == DM_OUTLINE) {
					loop_count_max = 9;
					gsize.x += 2;
					gsize.y += 2;
				}
				else if(mode == DM_SHADOW) {
					loop_count_max = 2;
					gsize.x += 1;
					gsize.y += 1;
				}
				int ox = x, oy = y;
				int cell_width, cell_height;
				bool blend = false;
				for(int loop_count = 0; loop_count < loop_count_max; loop_count++) {
					if(mode == DM_OUTLINE) {
						static int ofs[][2] = {
							{0, 0},
							{1, 0},
							{2, 0},
							{0, 1},
							{2, 1},
							{0, 2},
							{1, 2},
							{2, 2},
							{1, 1},
						};
						x = ox + ofs[loop_count][0];
						y = oy + ofs[loop_count][1];
						cell_width = bitmapsize-ofs[loop_count][0];
						cell_height = bitmapsize-ofs[loop_count][1];
						if(loop_count < 8) {
							srccolor = bkcolor__;
							if(loop_count > 0) blend = true;
						}
						else {
							srccolor = 0x0fff;
						}
					}
					else if(mode == DM_SHADOW) {
						if(loop_count == 0) {
							cell_width = bitmapsize-1;
							cell_height = bitmapsize-1;
							srccolor = bkcolor__; // 影は一律黒（とりあえず）
							x += 1;
							y += 1;
						}
						else {
							cell_width = bitmapsize;
							cell_height = bitmapsize;
							blend = true;
							srccolor = 0x0fff;
							x = ox;
							y = oy;
						}
					}
					for(int dy = 0; dy < cell_height; dy++) {
						if(ggo_yofs <= dy && dy < ggo_height) {
							for(int dx = 0; dx < cell_width; dx++ ) {
								if(ggo_xofs <= dx && dx < ggo_width) {
									srcalpha = buf[ggo_pitch*(dy-ggo_yofs) + (dx-ggo_xofs)];
									if(srcalpha) {
										srcalpha -= 1;
										if(blend && srcalpha < 15) {
											if(srcalpha > 0) {
												dstcolor = dst[(y+dy)*tsize.y+(x+dx)];
												uint8_t a = std::max<uint8_t>(srcalpha, ((dstcolor>>12)&0xf));
												uint8_t r = (((((srccolor>>8)&0xf)-((dstcolor>>8)&0xf))*srcalpha)/15 + (dstcolor>>8)&0xf);
												uint8_t g = (((((srccolor>>4)&0xf)-((dstcolor>>4)&0xf))*srcalpha)/15 + (dstcolor>>4)&0xf);
												uint8_t b = (((((srccolor)&0xf)-((dstcolor)&0xf))*srcalpha)/15 + (dstcolor)&0xf);
												dst[(y+dy)*tsize.y+(x+dx)] = a << 12 | r << 8 | g << 4 | b;
											}
										}
										else dst[(y+dy)*tsize.y+(x+dx)] = ((srcalpha&0xf) << 12) | srccolor;
									}
									else if(!blend) dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
								}
								else if(!blend) dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
							}
						}
						else {
							if(!blend) for(int dx = 0; dx < cell_width; dx++) dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
						}
					}
				}
			}
			delete[] buf;
#else
			cntx->dc.selectFont( font );
			cntx->dc.setMapMode( MM_TEXT );
			std::string str = charToStr(c);
			cntx->dc.getTextExtent( str.c_str(), str.length(), (SIZE *)&gsize );
			//PRNN(str<<":"<<gsize.x<<","<<gsize.y);
			cntx->dc.drawText( 0, 0, ETO_OPAQUE, NULL, str.c_str(), str.length(), NULL );
			uint16_t *dst = (uint16_t *)cntx->tex.lr.buf;
			uint8_t alpha;
			for(int dy = 0; dy < gsize.y; dy++ ) {
				for(int dx = 0; dx < gsize.x; dx++ ) {
					alpha = static_cast<uint8_t>((cntx->buf[bitmapsize*dy + dx] & 0xff) >> 4);
					if(alpha > 0) dst[(y+dy)*tsize.y+(x+dx)] = (alpha << 12) | 0x0fff;
					else dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
				}
			}
#endif
		}

	}

	AsciiFontTexture::AsciiFontTexture() : detail_(new detail::AsciiFontTextureDetail) {}

	HRslt AsciiFontTexture::setUp()
	{
		HRslt hr = Texture::setUp(Texture::NORMAL, 1024, 1024, D3DFMT_A4R4G4B4);
		//HRslt hr = Texture::setUp(Texture::NORMAL, 256, 256, D3DFMT_A4R4G4B4);
		if(!hr) return hr;
		if(cellSize() > 0) return hr;
		else return E_FAIL;
	}

	/** 対応するUV情報を返す
	 *
	 * 表示文字のみ相手にする。見つからなかったら、sizeに-1が入り、AsciiFontGlyphのboolおよび!演算子で
	 * falseが変えることでわかる。
	 * 文字が不正(表示文字でない）などの場合は、すべてに0が入り、AsciiFontGlyphのbool演算をパスする。
	 * (ただし、当然その情報を使っては表示不可能)
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/23 4:43:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	AsciiFontGlyph AsciiFontTexture::find(const Handle<Font> font, int c) const
	{
		AsciiFontGlyph ret;
		if(font && isPrintChar(c)) {
			if(isascii(c)) {
				AsciiAttrMap::const_iterator match = detail_->map_.find(font);
				if(match != detail_->map_.end()) {
					ret.uv = match->second.tex_coords[c-32];
					Size2f tex_size = Vector2C(size());
					ret.size.x = ret.uv.width()  * tex_size.x / match->second.scale;
					ret.size.y = ret.uv.height() * tex_size.y / match->second.scale;
					const detail::AsciiAttr *attr = &match->second;
					const_cast<detail::AsciiAttr *>(attr)->priority = 8;
					return ret;
				}
			}
			ret.uv.set(0, 0, 0, 0);
			ret.size.set(-1, -1);
		}
		else {
			ret.uv.set(0, 0, 0, 0);
			ret.size.set(0, 0);
		}
		return ret;
	}

	/** 文字がキャッシュされているか？
	 *
	 * 不正な文字や非表示文字は、常にtrueを返す
	 *
	 * また、満杯時に追い出されないようpriorityをアップデートする効果もある
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/23 4:43:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool AsciiFontTexture::isCached(const Handle<Font> font) const
	{
		if(font) {
			AsciiAttrMap::const_iterator match = detail_->map_.find(font);
			if(match != detail_->map_.end()) {
				const detail::AsciiAttr *attr = &match->second;
				const_cast<detail::AsciiAttr *>(attr)->priority = 8;
				return true;
			}
			return false;
		}
		return true;
	}

	/** begin endで囲む必要なし
	 *
	 * と言うかむしろbegin-endの外でやってください
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 21:23:22
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void AsciiFontTexture::cache(const Handle<Font> font)
	{
		if(font) {
			AsciiAttrMap::const_iterator match = detail_->map_.find(font);
			if(match == detail_->map_.end()) {
				Pointer<Font> _font = font.lock();
				int level = AllocBitMap<LEVEL>::blockToLevel(toBlockSize(getSmallestWidth(*_font, cellSize())));
				uint32_t idx;
				if(alloc(level, idx)) {
					detail::AsciiAttr &attr = detail_->map_[font];
					attr.idx = idx;
					attr.priority = 8;
					AllocBitMap<LEVEL>::Block block;
					block.set(idx);
					//attr.size.x = attr.size.y = block.w*cellSize();
					attr.scale = 1.0f;
					setUp(attr, *_font, block.w*cellSize(), block.w*cellSize(), block.pos.x*cellSize(), block.pos.y*cellSize());
				}
			}
		}
	}

	void AsciiFontTexture::aging()
	{
		for(AsciiAttrMap::iterator i = detail_->map_.begin(); i != detail_->map_.end(); i++) {
			if(i->second.priority > 0) i->second.priority--;
		}
	}

	bool AsciiFontTexture::alloc(uint32_t level, uint32_t &index)
	{
		int threshold = 1;
		while(!detail_->alloc_.alloc(level, index)) {
			if(threshold > 8) return false;
			for(AsciiAttrMap::iterator i = detail_->map_.begin(); i != detail_->map_.end();) {
				if(i->second.priority<threshold) {
					detail_->alloc_.free(i->second.idx);
					detail_->map_.erase(i++);
				}
				else i++;
			}
			threshold++;
		}
		return true;
	}

	/** 文字をレンダしてテクスチャに転送
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 1:59:28
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt AsciiFontTexture::setUp(detail::AsciiAttr &attr, const Font &font, ulong width, ulong height, int ofsx, int ofsy)
	{
		HRslt hr;
		attr.cellsize = font.cellsize();

#ifdef __GLYPH_OUTLINE_
		// Create a DC and a bitmap for the font
		DC dc;
		dc.setMapMode(MM_TEXT);
		dc.selectFont(font.get());
		TEXTMETRIC tm; // 現在のフォント情報
		dc.getTextMetrics(&tm);
		MAT2 mat2 = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
		GLYPHMETRICS glyph;

		SIZE gsize;
		_TCHAR str[2] = _T("x");
		Size2 tsize = size();
		int default_line_height, line_height;
		Texture::ScopedLock al(*this, 0);
		uint16_t *dst = (uint16_t *)al.lr.buf;
		uint8_t alpha; // 4-bit measure of pixel intensity
		attr.max_width = 0;

		DrawMode mode = DM_NORMAL;
		if(font.exStyle()==Font::EX_OUTLINE) mode = DM_OUTLINE;
		else if(font.exStyle()==Font::EX_SHADOW) mode = DM_SHADOW;

		uint x = ofsx, y = ofsy;
		for( char c=32; c<127; c++ ) {
			ulong bufsize = dc.getGlyphOutline( c, GGO_GRAY4_BITMAP, &glyph, 0, NULL, &mat2 );
			uint8_t *buf = new uint8_t[bufsize]; // 実際にバッファに取得
			// アウトラインをグレースケールで作成
			dc.getGlyphOutline( c, GGO_GRAY4_BITMAP, &glyph, bufsize, (void *)buf, &mat2 );

			str[0] = c;
			dc.getTextExtent( str, 1, &gsize );

			int ggo_pitch = ( glyph.gmBlackBoxX + 3 ) & 0xfffc; // ダブルワード境界に整列
			int ggo_xofs = glyph.gmptGlyphOrigin.x;
			int ggo_yofs = tm.tmAscent-glyph.gmptGlyphOrigin.y;
			int ggo_width = ggo_xofs+glyph.gmBlackBoxX;
			int ggo_height = ggo_yofs+glyph.gmBlackBoxY;

			if(mode == DM_OUTLINE) {
				gsize.cx += 2;
				gsize.cy += 2;
			}
			else if(mode == DM_SHADOW) {
				gsize.cx += 1;
				gsize.cy += 1;
			}
			if(c == 32) default_line_height = line_height = gsize.cy+1;
			if(attr.max_width < gsize.cx) attr.max_width = gsize.cx;
			if(line_height < gsize.cy+1) line_height = gsize.cy+1;
			if( (int)((x-ofsx)+gsize.cx+1) > width ) {
				x  = ofsx; y += line_height; line_height = default_line_height;
			}

			attr.tex_coords[c-32].left   = ((float)x+0.5f)/tsize.x;
			attr.tex_coords[c-32].top    = ((float)y+0.5f)/tsize.y;
			attr.tex_coords[c-32].right  = ((float)(x+gsize.cx+0.5f))/tsize.x;
			attr.tex_coords[c-32].bottom = ((float)(y+gsize.cy+0.5f))/tsize.y;
			
			if(c == 32 || mode == DM_NORMAL) {
				for(int dy = 0; dy < gsize.cy; dy++) {
					if(ggo_yofs <= dy && dy < ggo_height) {
						for(int dx = 0; dx < gsize.cx; dx++ ) {
							if(ggo_xofs <= dx && dx < ggo_width) {
								alpha = buf[ggo_pitch*(dy-ggo_yofs) + (dx-ggo_xofs)];
								if(c==32) dst[(y+dy)*tsize.y+(x+dx)] = 0xffff; // 空白は真っ白に（バックカラーとかで使う）
								else if(alpha) {
									dst[(y+dy)*tsize.y+(x+dx)] = (((alpha-1)&0xf) << 12) | 0x0fff;
								}
								else dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
							}
							else {
								if(c==32) dst[(y+dy)*tsize.y+(x+dx)] = 0xffff; // 空白は真っ白に（バックカラーとかで使う）
								else dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
							}
						}
					}
					else {
						for(int dx = 0; dx < gsize.cx; dx++) {
							if(c==32) dst[(y+dy)*tsize.y+(x+dx)] = 0xffff; // 空白は真っ白に（バックカラーとかで使う）
							else dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
						}
					}
				}
			}
			else {
				uint8_t srcalpha;
				uint16_t dstcolor, srccolor;
				int loop_count_max = 1;
				if(mode == DM_OUTLINE) {
					loop_count_max = 9;
				}
				else if(mode == DM_SHADOW) loop_count_max = 2;
				int tmpx = x, tmpy = y;
				int cell_width, cell_height;
				bool blend = false;
				for(int loop_count = 0; loop_count < loop_count_max; loop_count++) {
					if(mode == DM_OUTLINE) {
						static int ofs[][2] = {
							{0, 0},
							{1, 0},
							{2, 0},
							{0, 1},
							{2, 1},
							{0, 2},
							{1, 2},
							{2, 2},
							{1, 1},
						};
						tmpx = x + ofs[loop_count][0];
						tmpy = y + ofs[loop_count][1];
						cell_width = gsize.cx-ofs[loop_count][0];
						cell_height = gsize.cy-ofs[loop_count][1];
						if(loop_count < 8) {
							srccolor = bkcolor__;
							if(loop_count > 0) blend = true;
						}
						else {
							srccolor = 0x0fff;
						}
					}
					else if(mode == DM_SHADOW) {
						if(loop_count == 0) {
							cell_width = gsize.cx-1;
							cell_height = gsize.cy-1;
							srccolor = bkcolor__;
							tmpx += 1;
							tmpy += 1;
						}
						else {
							cell_width = gsize.cx;
							cell_height = gsize.cy;
							blend = true;
							srccolor = 0x0fff;
							tmpx = x;
							tmpy = y;
						}
					}
					for(int dy = 0; dy < cell_height; dy++) {
						if(ggo_yofs <= dy && dy < ggo_height) {
							for(int dx = 0; dx < cell_width; dx++ ) {
								if(ggo_xofs <= dx && dx < ggo_width) {
									srcalpha = buf[ggo_pitch*(dy-ggo_yofs) + (dx-ggo_xofs)];
									if(srcalpha) {
										srcalpha -= 1;
										if(blend && srcalpha < 15) {
											if(srcalpha > 0) {
												dstcolor = dst[(tmpy+dy)*tsize.y+(tmpx+dx)];
												uint8_t a = std::max<uint8_t>(srcalpha, ((dstcolor>>12)&0xf));
												uint8_t r = (((((srccolor>>8)&0xf)-((dstcolor>>8)&0xf))*srcalpha)/15 + (dstcolor>>8)&0xf);
												uint8_t g = (((((srccolor>>4)&0xf)-((dstcolor>>4)&0xf))*srcalpha)/15 + (dstcolor>>4)&0xf);
												uint8_t b = (((((srccolor)&0xf)-((dstcolor)&0xf))*srcalpha)/15 + (dstcolor)&0xf);
												dst[(tmpy+dy)*tsize.y+(tmpx+dx)] = a << 12 | r << 8 | g << 4 | b;
											}
										}
										else dst[(tmpy+dy)*tsize.y+(tmpx+dx)] = ((srcalpha&0xf) << 12) | srccolor;
									}
									else if(!blend) dst[(tmpy+dy)*tsize.y+(tmpx+dx)] = 0x0000;
								}
								else if(!blend) dst[(tmpy+dy)*tsize.y+(tmpx+dx)] = 0x0000;
							}
						}
						else {
							if(!blend) for(int dx = 0; dx < cell_width; dx++) dst[(tmpy+dy)*tsize.y+(tmpx+dx)] = 0x0000;
						}
					}
				}
			}
			x += gsize.cx+1;
			delete[] buf;
		}
#else
		// Prepare to create a bitmap
		BITMAPINFO bmi;
		ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
		bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth       =  (int)width;
		bmi.bmiHeader.biHeight      = -(int)height;
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biBitCount    = 32;
		
		// Create a DC and a bitmap for the font
		detail::DC dc;
		DWORD *pBitmapBits;
		detail::Bitmap bitmap = dc.createDIBSection(&bmi, DIB_RGB_COLORS, (void**)&pBitmapBits, NULL, 0);
		
		dc.setMapMode(MM_TEXT);
		dc.selectBitmap(bitmap.bitmap);
		
		// Set text properties
		dc.setTextColor(RGB(255,255,255));
		dc.setBkColor(0x00000000);
		dc.setTextAlign(TA_TOP);
		
		dc.selectFont(font);

		// Loop through all printable character and output them to the bitmap..
		// Meanwhile, keep track of the corresponding tex coords for each character.
		uint x = 0;
		uint y = 0;
		char str[2] = _T("x");
		SIZE gsize;
		Size2 tsize = size();
		int default_line_height, line_height;
		//max_width_ = 0;
		
		for( char c=32; c<127; c++ ) {
			str[0] = c;
			dc.getTextExtent( str, 1, &gsize );
			
			if(c == 32) default_line_height = line_height = gsize.cy+1;
			if(line_height < gsize.cy+1) line_height = gsize.cy+1;
			//if(max_width_ < gsize.cx) max_width_ = gsize.cx;
			if( (int)(x+gsize.cx+1) > width ) {
				x  = 0; y += line_height; line_height = default_line_height;
			}
			
			dc.drawText( x, y, ETO_OPAQUE, NULL, str, 1, NULL );
			
			/*tex_coords_[c-32].left   = ((float)x+ofsx)/tsize.x;
			tex_coords_[c-32].top    = ((float)y+ofsx)/tsize.y;
			tex_coords_[c-32].right  = ((float)(x+ofsx+gsize.cx))/tsize.x;
			tex_coords_[c-32].bottom = ((float)(y+ofsx+gsize.cy))/tsize.y;*/
			
			x += gsize.cx+1;
		}
		
		// Lock the surface and write the alpha values for the set pixels
		{
			Texture::ScopedLock al(*this, 0);
			uint16_t *dst = (uint16_t *)al.lr.buf;
			uint8_t alpha; // 4-bit measure of pixel intensity
			
			for( y=0; y < height; y++ ) {
				for( x=0; x < width; x++ ) {
					alpha = (uint8_t)((pBitmapBits[width*y + x] & 0xff) >> 4);
					//if(c==32) dst[(y+dy)*tsize.y+(x+dx)] = 0xffff; // 空白は真っ白に（バックカラーとかで使う）
					//else 
					if(alpha > 0) dst[tsize.x*(y+ofsy)+x+ofsx] = (alpha << 12) | 0x0fff;
					else dst[tsize.x*(y+ofsy)+x+ofsx] = 0x0000;
				}
			}
		}
#endif
		return S_OK;
	}

	/** 指定フォントのアスキー文字が全部収まるサイズを返す
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 1:59:28
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	uint AsciiFontTexture::getSmallestWidth(const Font &font, uint unit)
	{
		// Create a DC and a bitmap for the font
		DC dc;
		dc.setMapMode(MM_TEXT);
		dc.setTextAlign(TA_TOP);
		dc.selectFont(font.get());
		_TCHAR str[2] = _T(" ");
		SIZE gsize;
		int default_line_height, line_height;
		uint char_width[128-32];
		
		uint total_height = 0, line_width = 0;
		
		for( char c=32; c<127; c++ ) {
			str[0] = c;
			dc.getTextExtent( str, 1, &gsize );

			if(c == 32) default_line_height = line_height = gsize.cy+1;
			if(line_height < gsize.cy+1) line_height = gsize.cy+1;
			char_width[c-32] = gsize.cx+1;
			if(line_width+gsize.cx+1 > unit) {
				line_width = 0; total_height += line_height; line_height = default_line_height;
			}
			line_width += gsize.cx+1;
		}
		
		uint ret = unit;
		for(;;) {
			total_height = 0;
			line_width = 0;
			for( char c=32; c<127; c++ ) {
				if(line_width+char_width[c-32] > ret) {
					line_width = 0;
					total_height += line_height;
				}
				line_width += char_width[c-32];
			}
			if(ret > total_height) break;
			ret *= 2;
		}
		return ret;
	}
	
	/** 文字表示
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/17 6:08:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void drawText(SpriteBuffer &spr, AsciiFontTexture &font, const Handle<Font> cfont, const Point2f &pos, Color32 color, const _TCHAR *_text)
	{
		const _TCHAR *text = _text;
		uint line_height, default_line_height;
		AsciiFontGlyph glyph = font.find(cfont, _T(' '));
		uint space_size = static_cast<uint>(glyph.size.x);
		line_height = default_line_height = static_cast<uint>(glyph.size.y);
		//font.gabage();
		// 文字をキャッシュ
		// まずアスキーから。２回に分けるのは無駄だけど…
		while(text && *text) {
			int c;
			text += getChar(c, text);
			if( isascii(c) ) {
				if(!font.isCached(cfont)) font.cache(cfont);
			}
		}
		text = _text;
		while(text && *text) {
			int c;
			text += getChar(c, text);
		}
		Point2f wpos = pos;
		text = _text;
		while(text && *text) {
			int c;
			text += getChar(c, text);
			if( c == _T('\n') || c == _T('\r') ) {
				wpos.x = pos.x; wpos.y += line_height; line_height = default_line_height;
				continue;
			}
			if( c == _T('\t') ) {
				wpos.x = pos.x + toTabbed(wpos.x-pos.x, space_size*4);
				continue;
			}
			if( isPrintChar(c) ) {
				glyph = font.find(cfont, c);
				if(glyph && !isSpaceChar(c)) {
					SpriteDesc desc;
					desc.setPos(RectfC(wpos.x, wpos.y, wpos.x+glyph.size.x, wpos.y+glyph.size.y));
					desc.setUV(glyph.uv);
					desc.setColor(color);
					spr.draw(desc);
					if(line_height < glyph.size.y) line_height = static_cast<uint>(glyph.size.y);
				}
				wpos.x += glyph.size.x;
			}
		}
	}

	/** 文字列中の指定文字の位置を取得
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/17 6:08:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Point2f getTextPos(const AsciiFontTexture &font, const Handle<Font> cfont, const _TCHAR *text, uint idx)
	{
		Point2f ret = {0, 0};
		if(text) {
			AsciiFontGlyph glyph = font.find(cfont, _T(' '));
			int space_size = static_cast<int>(glyph.size.x);
			int line_height, default_line_height;
			line_height = default_line_height = static_cast<uint>(glyph.size.y);
			std::size_t len = _tcslen(text);
			while(*text && idx && idx <= len) {
				int c, n;
				n = getChar(c, text);
				text += n;
				idx -= n;
				if( c == _T('\n') || c == _T('\r') ) {
					ret.x = 0; ret.y += line_height; line_height = default_line_height;
					continue;
				}
				if( c == _T('\t') ) {
					ret.x = toTabbed(ret.x, space_size*4);
					continue;
				}
				if(isPrintChar(c)) {
					glyph = font.find(cfont, c);
					if(glyph && !isSpaceChar(c)) {
						if(line_height < glyph.size.y) line_height = static_cast<uint>(glyph.size.y);
					}
					ret.x += glyph.size.x;
				}
			}
		}
		return ret;
	}

}}
