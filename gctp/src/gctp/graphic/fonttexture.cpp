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
#include <gctp/graphic/fonttexture.hpp>
#include <gctp/graphic/spritebuffer.hpp>
#include <gctp/font.hpp>
#include <gctp/allocmap.hpp>
#include <gctp/utils.hpp>
#include <gctp/tcsv.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	GCTP_IMPLEMENT_CLASS_NS(gctp, FontTexture, Texture);

	namespace {
		
		struct Figure {
			Handle<Font> font;
			int c;
			Figure(Handle<Font> _font, int _c) : font(_font), c(_c) {}
			bool operator == (const Figure &rhs) const { return font == rhs.font && c == rhs.c; }
			bool operator < (const Figure &rhs) const {
				if(font == rhs.font) return c < rhs.c;
				return font < rhs.font;
			}
		};
		struct Attr {
			int idx;
			Size2 size;
			int priority;
		};
		typedef std::map<Figure, Attr> FigureMap;

	}

#define __GLYPH_OUTLINE_ // GetGlyphOutlineで文字を書く（そうでない場合はExtTextOutでDIBに書いてコピー）

	namespace detail {
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

#ifndef __GLYPH_OUTLINE_
		class Bitmap {
		public:
			HBITMAP bitmap;
			Bitmap() : bitmap(0) {}
			Bitmap(HBITMAP bitmap) : bitmap(bitmap) {}
			void setUp(HBITMAP bitmap)
			{
				this->bitmap = bitmap;
			}
			~Bitmap()
			{
				 DeleteObject(bitmap);
			}
		};
#endif

		class DrawContext : public Object {
		public:
			DC dc;
			Texture::ScopedLock tex;
#ifndef __GLYPH_OUTLINE_
			uint32_t *buf;
			Bitmap bitmap;
#endif
#ifdef __GLYPH_OUTLINE_
			DrawContext(FontTexture &ftex) : tex(ftex, 0) {
				dc.setTextColor(RGB(255,255,255));
				dc.setBkColor(0x00000000);
				dc.setTextAlign(TA_TOP);
			}
#else
			DrawContext(FontTexture &ftex) : buf(0), tex(ftex, 0) {
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

		class FontTextureDetail {
		public:
			AllocBitMap<FontTexture::LEVEL> alloc_;
			FigureMap map_;
		};
	}

	namespace {

		enum DrawMode {
			DM_NORMAL, // 只描画
			DM_SHADOW, // 影文字
			DM_OUTLINE, // 袋文字
		};

		uint16_t bkcolor__ = 0x0000;

		void draw(detail::DrawContext *cntx, int bitmapsize, const Size2 &tsize, HFONT font, int c, int x, int y, Size2 &gsize, DrawMode mode)
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
							srccolor = bkcolor__;
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

	FontTexture::FontTexture() : detail_(new detail::FontTextureDetail) {}

	HRslt FontTexture::setUp(const _TCHAR *name)
	{
		if(name) {
			basic_stringstream<_TCHAR> ioss;
			ioss << name;
			TCSVRow csv;
			ioss >> csv;
			if(csv.size() > 2) {
				return setUp(boost::lexical_cast<int>(csv[1]), boost::lexical_cast<int>(csv[2]));
			}
		}
		return setUp(256, 256);
	}

	HRslt FontTexture::setUp(int width, int height)
	{
		HRslt hr = Texture::setUp(Texture::NORMAL, width, height, D3DFMT_A4R4G4B4);
		if(!hr) return hr;
		if(cellSize() > 0) return hr;
		else return E_FAIL;
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
	bool FontTexture::isCached(const Handle<Font> font, int c) const
	{
		if(font && isPrintChar(c)) {
			FigureMap::const_iterator match = detail_->map_.find(Figure(font, c));
			if(match != detail_->map_.end()) {
				const Attr *attr = &match->second;
				const_cast<Attr *>(attr)->priority = 8;
				return true;
			}
			return false;
		}
		return true;
	}

	/** 対応するUV情報を返す
	 *
	 * 表示文字のみ相手にする。見つからなかったら、sizeに-1が入り、FontGlyphのboolおよび!演算子で
	 * falseが返ることでわかる。
	 * 文字が不正(表示文字でない）などの場合は、すべてに0が入り、FontGlyphのbool演算をパスする。
	 * (ただし、当然その情報を使っては表示不可能)
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/23 4:43:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	FontGlyph FontTexture::find(const Handle<Font> font, int c) const
	{
		FontGlyph ret;
		if(font && isPrintChar(c)) {
			FigureMap::const_iterator match = detail_->map_.find(Figure(font, c));
			if(match != detail_->map_.end()) {
				AllocBitMap<LEVEL>::Block block;
				block.set(match->second.idx);
				Size2 tsize = size();
				ret.uv.left   = ((float)block.pos.x*cellSize()+0.5f)/tsize.x;
				ret.uv.top    = ((float)block.pos.y*cellSize()+0.5f)/tsize.y;
				ret.uv.right  = ((float)(block.pos.x*cellSize()+match->second.size.x+0.5f))/tsize.x;
				ret.uv.bottom = ((float)(block.pos.y*cellSize()+match->second.size.y+0.5f))/tsize.y;
				ret.size.x    = (float)match->second.size.x;
				ret.size.y    = (float)match->second.size.y;
				return ret;
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

	void FontTexture::begin()
	{
		cntx_ = new detail::DrawContext(*this);
	}

	/** 事前にbeginが呼び出されている必要がある
	 * 一連の文字キャッシュが終わったら、endを呼び出すこと。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 21:23:40
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void FontTexture::cache(const Handle<Font> font, int c)
	{
		if(cntx_.get() && font) {
			FigureMap::const_iterator match = detail_->map_.find(Figure(font, c));
			if(match == detail_->map_.end()) {
				Pointer<Font> _font = font.lock();
				if(_font->cellsize()+1 < maxCellSize()) {
					int level = AllocBitMap<LEVEL>::blockToLevel(toBlockSize(_font->cellsize()+1));
					Attr attr;
					attr.idx = alloc(level);
					if(attr.idx != -1) {
						attr.priority = 8;
						AllocBitMap<LEVEL>::Block block;
						block.set(attr.idx);
						DrawMode mode = DM_NORMAL;
						if(_font->exStyle()==Font::EX_OUTLINE) mode = DM_OUTLINE;
						else if(_font->exStyle()==Font::EX_SHADOW) mode = DM_SHADOW;
#ifdef __GLYPH_OUTLINE_
						draw(cntx_.get(), block.w*cellSize(), size(), _font->get(), c, block.pos.x*cellSize(), block.pos.y*cellSize(), attr.size, mode);
#else
						draw(cntx_.get(), maxCellSize(), size(), _font->get(), c, block.pos.x*cellSize(), block.pos.y*cellSize(), attr.size);
#endif
						detail_->map_[Figure(_font.get(), c)] = attr;
					}
					else {
						PRNN("文字がキャッシュできなかった : '"<<charToStr(c)<<"'");
					}
				}
			}
		}
	}

	void FontTexture::gabage()
	{
		for(FigureMap::iterator i = detail_->map_.begin(); i != detail_->map_.end(); i++) {
			if(i->second.priority > 0) i->second.priority--;
		}
	}

	int FontTexture::alloc(int level)
	{
		int threshold = 1;
		int ret = detail_->alloc_.alloc(level);
		while(ret < 0 && threshold < 9) {
			for(FigureMap::iterator i = detail_->map_.begin(); i != detail_->map_.end();) {
				if(i->second.priority<threshold) {
					detail_->alloc_.free(i->second.idx);
					detail_->map_.erase(i++);
				}
				else i++;
			}
			ret = detail_->alloc_.alloc(level);
			threshold++;
		}
		return ret;
	}

	void FontTexture::end()
	{
		cntx_ = 0;
	}

	void FontTexture::clear()
	{
		detail_->alloc_.reset();
		detail_->map_.clear();
	}

	void FontTexture::setShadowColor(Color32 bkcolor)
	{
		bkcolor__ = ((bkcolor.r>>4)&0xf)<<8|((bkcolor.g>>4)&0xf)<<4|((bkcolor.b>>4)&0xf);
	}

	namespace detail {

		class FontTextureSet {
		public:
			std::map< Handle<Font>, Pointer<FontTexture> > fonts_;
		};

	}
	
	/** 文字表示
	 *
	 * 非アスキー文字にも対応
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/17 6:08:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void drawText(SpriteBuffer &spr, FontTexture &font, const Handle<Font> cfont, const Point2f &pos, Color32 color, const _TCHAR *_text)
	{
		const _TCHAR *text = _text;
		uint line_height, default_line_height;
		FontGlyph glyph = font.find(cfont, _T(' '));
		uint space_size = static_cast<uint>(glyph.size.x);
		line_height = default_line_height = static_cast<uint>(glyph.size.y);
		// 文字をキャッシュ
		bool began = false;
		while(text && *text) {
			int c;
			text += getChar(c, text);
			if(!font.isCached(cfont, c)) {
				if(!began) {
					font.begin();
					began = true;
				}
				font.cache(cfont, c);
			}
		}
		if(began) font.end();
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
	Point2f getTextPos(const FontTexture &font, const Handle<Font> cfont, const _TCHAR *text, uint idx)
	{
		Point2f ret = {0, 0};
		if(text) {
			FontGlyph glyph = font.find(cfont, _T(' '));
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
