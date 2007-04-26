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
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	GCTP_IMPLEMENT_CLASS_NS(gctp, FontTexture, Texture);

	namespace detail {
		struct AsciiAttr {
			ulong	cellsize;
			Rectf	tex_coords[128-32];
			int max_width;
			float scale;
			int idx;
			int priority;
		};
	}

	namespace {
		
		struct Figure {
			Handle<Font> font;
			int c;
			Figure(Handle<Font> _font, int _c) : font(_font), c(_c) {}
			bool operator == (const Figure &rhs) const { return font == rhs.font && c == rhs.c; }
			bool operator < (const Figure &rhs) const { return font < rhs.font || c < rhs.c; }
		};
		struct Attr {
			int idx;
			Size2 size;
			int priority;
		};
		typedef std::map<Figure, Attr> FigureMap;
		typedef std::map<Handle<Font>, detail::AsciiAttr> AsciiFigureMap;

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

		class DrawContext {
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
			AsciiFigureMap asciimap_;
		};
	}

	namespace {

		void draw(detail::DrawContext *cntx, int bitmapsize, const Size2 &tsize, HFONT font
			, int c, int x, int y, Size2 &gsize)
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
			uint8_t alpha;
			for(int dy = 0; dy < bitmapsize; dy++) {
				if(ggo_yofs <= dy && dy < ggo_height) {
					for(int dx = 0; dx < bitmapsize; dx++ ) {
						if(ggo_xofs <= dx && dx < ggo_width) {
							alpha = buf[ggo_pitch*(dy-ggo_yofs) + (dx-ggo_xofs)];
							if(alpha) dst[(y+dy)*tsize.y+(x+dx)] = (((alpha-1)&0xf) << 12) | 0x0fff;
							else dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
						}
						else dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
					}
				}
				else {
					for(int dx = 0; dx < bitmapsize; dx++) dst[(y+dy)*tsize.y+(x+dx)] = 0x0000;
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

	HRslt FontTexture::setUp()
	{
		HRslt hr = Texture::setUp(1024, 1024, D3DFMT_A4R4G4B4);
		//HRslt hr = Texture::setUp(256, 256, D3DFMT_A4R4G4B4);
		if(!hr) return hr;
		if(cellSize() > 0) return hr;
		else return E_FAIL;
	}

	/** 文字がキャッシュされているか？
	 *
	 * 不正な文字や非表示文字は、常にtrueを返す
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/23 4:43:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool FontTexture::isCached(const Handle<Font> font, int c) const
	{
		if(font && isPrintChar(c)) {
			if(isascii(c)) {
				AsciiFigureMap::const_iterator match = detail_->asciimap_.find(font);
				if(match != detail_->asciimap_.end()) return true;
			}
			else {
				FigureMap::const_iterator match = detail_->map_.find(Figure(font, c));
				if(match != detail_->map_.end()) return true;
			}
			return false;
		}
		return true;
	}

	/** 対応するUV情報を返す
	 *
	 * 表示文字のみ相手にする。見つからなかったら、sizeに-1が入り、FontGlyphのboolおよび!演算子で
	 * falseが変えることでわかる。
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
			if(isascii(c)) {
				AsciiFigureMap::const_iterator match = detail_->asciimap_.find(font);
				if(match != detail_->asciimap_.end()) {
					ret.uv = match->second.tex_coords[c-32];
					Size2f tex_size = Vector2C(size());
					ret.size.x = ret.uv.width()  * tex_size.x / match->second.scale;
					ret.size.y = ret.uv.height() * tex_size.y / match->second.scale;
					const detail::AsciiAttr *attr = &match->second;
					const_cast<detail::AsciiAttr *>(attr)->priority = 8;
					return ret;
				}
			}
			else {
				FigureMap::const_iterator match = detail_->map_.find(Figure(font, c));
				if(match != detail_->map_.end()) {
					AllocBitMap<LEVEL>::Block block;
					block.set(match->second.idx);
					Size2 tsize = size();
					ret.uv.left   = ((float)block.pos.x*cellSize())/tsize.x;
					ret.uv.top    = ((float)block.pos.y*cellSize())/tsize.y;
					ret.uv.right  = ((float)(block.pos.x*cellSize()+match->second.size.x))/tsize.x;
					ret.uv.bottom = ((float)(block.pos.y*cellSize()+match->second.size.y))/tsize.y;
					ret.size.x    = (float)match->second.size.x;
					ret.size.y    = (float)match->second.size.y;
					const Attr *attr = &match->second;
					const_cast<Attr *>(attr)->priority = 8;
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

	/** begin endで囲む必要なし
	 *
	 * と言うかむしろbegin-endの外でやってください
	 *
	 * @note 現在、一枚につき一種類のアスキーフォントしか持てない。（２度目以降は上書きされる。しかも領域を開放しないまま）
	 *
	 * @todo 複数のアスキーフォントをキャッシュできるようにする
	 * @todo せめて以前の奴は領域を開放するようにする
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 21:23:22
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void FontTexture::cacheAscii(const Handle<Font> font)
	{
		if(font) {
			AsciiFigureMap::const_iterator match = detail_->asciimap_.find(font);
			if(match == detail_->asciimap_.end()) {
				Pointer<Font> _font = font.lock();
				int level = AllocBitMap<LEVEL>::blockToLevel(toBlockSize(getSmallestWidth(*_font, cellSize())));
				int idx = alloc(level);
				if(idx != -1) {
					detail::AsciiAttr &attr = detail_->asciimap_[font];
					attr.idx = idx;
					attr.priority = 8;
					AllocBitMap<LEVEL>::Block block;
					block.set(idx);
					//attr.size.x = attr.size.y = block.w*cellSize();
					attr.scale = 1.0f;
					setUpAscii(attr, *_font, block.w*cellSize(), block.w*cellSize(), block.pos.x*cellSize(), block.pos.y*cellSize());
				}
			}
		}
	}

	void FontTexture::begin()
	{
		cntx_ = std::auto_ptr<detail::DrawContext>(new detail::DrawContext(*this));
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
					attr.priority = 8;
					if(attr.idx != -1) {
						AllocBitMap<LEVEL>::Block block;
						block.set(attr.idx);
#ifdef __GLYPH_OUTLINE_
						draw(cntx_.get(), block.w*cellSize(), size(), _font->get(), c, block.pos.x*cellSize(), block.pos.y*cellSize(), attr.size);
#else
						draw(cntx_.get(), maxCellSize(), size(), _font->get(), c, block.pos.x*cellSize(), block.pos.y*cellSize(), attr.size);
#endif
						detail_->map_[Figure(_font.get(), c)] = attr;
					}
				}
			}
		}
	}

	void FontTexture::gabage()
	{
		for(AsciiFigureMap::iterator i = detail_->asciimap_.begin(); i != detail_->asciimap_.end(); i++) {
			if(i->second.priority > 0) i->second.priority--;
		}
		for(FigureMap::iterator i = detail_->map_.begin(); i != detail_->map_.end(); i++) {
			if(i->second.priority > 0) i->second.priority--;
		}
	}

	int FontTexture::alloc(int level)
	{
		int ret = detail_->alloc_.alloc(level);
		if(ret == -1) {
			for(AsciiFigureMap::iterator i = detail_->asciimap_.begin(); i != detail_->asciimap_.end();) {
				if(i->second.priority<4) {
					detail_->alloc_.free(i->second.idx);
					detail_->asciimap_.erase(i++);
				}
				else i++;
			}
			for(FigureMap::iterator i = detail_->map_.begin(); i != detail_->map_.end();) {
				if(i->second.priority<4) {
					detail_->alloc_.free(i->second.idx);
					detail_->map_.erase(i++);
				}
				else i++;
			}
			ret = detail_->alloc_.alloc(level);
		}
		return ret;
	}

	void FontTexture::end()
	{
		cntx_ = std::auto_ptr<detail::DrawContext>();
	}

	void FontTexture::clear()
	{
		detail_->alloc_.reset();
		detail_->map_.clear();
	}

	/** 文字をレンダしてテクスチャに転送
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 1:59:28
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt FontTexture::setUpAscii(detail::AsciiAttr &attr, const Font &font, ulong width, ulong height, int ofsx, int ofsy)
	{
		HRslt hr;
		attr.cellsize = font.cellsize();

#ifdef __GLYPH_OUTLINE_
		// Create a DC and a bitmap for the font
		detail::DC dc;
		dc.setMapMode(MM_TEXT);
		dc.selectFont(font.get());
		TEXTMETRIC tm; // 現在のフォント情報
		dc.getTextMetrics(&tm);
		MAT2 mat2 = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
		GLYPHMETRICS glyph;

		uint x = 0;
		uint y = 0;
		SIZE gsize;
		_TCHAR str[2] = _T("x");
		Size2 tsize = size();
		int default_line_height, line_height;
		Texture::ScopedLock al(*this, 0);
		uint16_t *dst = (uint16_t *)al.lr.buf;
		uint8_t alpha; // 4-bit measure of pixel intensity
		attr.max_width = 0;

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

			if(c == 32) default_line_height = line_height = gsize.cy+1;
			if(attr.max_width < gsize.cx) attr.max_width = gsize.cx;
			if(line_height < gsize.cy+1) line_height = gsize.cy+1;
			if( (int)(x+gsize.cx+1) > width ) {
				x  = 0; y += line_height; line_height = default_line_height;
			}

			attr.tex_coords[c-32].left   = ((float)x+ofsx)/tsize.x;
			attr.tex_coords[c-32].top    = ((float)y+ofsx)/tsize.y;
			attr.tex_coords[c-32].right  = ((float)(x+ofsx+gsize.cx))/tsize.x;
			attr.tex_coords[c-32].bottom = ((float)(y+ofsx+gsize.cy))/tsize.y;
			
			for(int dy = 0; dy < gsize.cy; dy++) {
				if(ggo_yofs <= dy && dy < ggo_height) {
					for(int dx = 0; dx < gsize.cx; dx++ ) {
						if(ggo_xofs <= dx && dx < ggo_width) {
							alpha = buf[ggo_pitch*(dy-ggo_yofs) + (dx-ggo_xofs)];
							if(c==32) dst[(y+dy)*tsize.y+(x+dx)] = 0xffff; // 空白は真っ白に（バックカラーとかで使う）
							else if(alpha) dst[(y+dy)*tsize.y+(x+dx)] = (((alpha-1)&0xf) << 12) | 0x0fff;
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
	uint FontTexture::getSmallestWidth(const Font &font, uint unit)
	{
		// Create a DC and a bitmap for the font
		detail::DC dc;
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
		font.gabage();
		// 文字をキャッシュ
		// まずアスキーから。２回に分けるのは無駄だけど…
		while(text && *text) {
			int c;
			text += getChar(c, text);
			if( isascii(c) ) {
				if(!font.isCached(cfont, c)) font.cacheAscii(cfont);
			}
		}
		text = _text;
		bool began = false;
		while(text && *text) {
			int c;
			text += getChar(c, text);
			if( !isascii(c) ) {
				if(!font.isCached(cfont, c)) {
					if(!began) {
						font.begin();
						began = true;
					}
					font.cache(cfont, c);
				}
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
					desc.setPos(RectfC(wpos.x-0.5f, wpos.y-0.5f, wpos.x+glyph.size.x-0.5f, wpos.y+glyph.size.y-0.5f));
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
