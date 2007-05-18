/**@file
 * GameCatapult 文字表示ストリームクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 1:42:30
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/text.hpp>
#include <gctp/graphic/spritebuffer.hpp>
#include <gctp/graphic/fonttexture.hpp>
#include <gctp/font.hpp>
#include <sstream>
#include <gctp/utils.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {

	namespace detail {

		namespace {
			class Attr {
			public:
				struct Pos {
					float x;
					float y;
				};
				enum Type {
					POSITION,
					COLOR,
					BACKCOLOR,
					FONT,
					CURSOR,
				} type;

				Attr() {}
				Attr(float x, float y) : type(POSITION) { pos.x = x; pos.y = y; }
				explicit Attr(D3DCOLOR _color) : type(COLOR), color(_color) {}
				explicit Attr(D3DCOLOR _color, bool dummy) : type(BACKCOLOR), color(_color) {}
				explicit Attr(const Handle<Font> &_font) : type(FONT), font(_font) {}
				explicit Attr(Type _type) : type(_type) {}

				union {
					Pos pos;
					D3DCOLOR color;
				};
				Handle<Font> font;
			};
			typedef multimap<string::size_type, Attr> AttrMap;
		}

		class TextImpl {
		public:
			TextImpl()
			{
			}

			basic_string<_TCHAR>::size_type position(int ofs)
			{
				int n = static_cast<int>(os.tellp())+ofs;
				if(n > 0) return n;
				return 0;
			}

			basic_ostringstream<_TCHAR> os;
			AttrMap attrs_;
		};

	}

	GCTP_IMPLEMENT_CLASS_NS(gctp, Text, Object);
	
	using namespace detail;

	Text::Text() : impl_(new TextImpl)
	{
	}

	Text::~Text()
	{
	}

	Text &Text::setPos(float x, float y, int ofs) {
		impl_->attrs_.insert(std::make_pair(impl_->position(ofs), Attr(x, y)));
		return *this;
	}

	Text &Text::setColor(Color32 color, int ofs) {
		impl_->attrs_.insert(std::make_pair(impl_->position(ofs), Attr(color)));
		return *this;
	}

	Text &Text::setBackColor(Color32 color, int ofs) {
		impl_->attrs_.insert(std::make_pair(impl_->position(ofs), Attr(color, true)));
		return *this;
	}

	Text &Text::setFont(const Handle<Font> &font, int ofs) {
		impl_->attrs_.insert(std::make_pair(impl_->position(ofs), Attr(font)));
		return *this;
	}

	Text &Text::setClumpPos(float x, float y, int ofs)
	{
		D3DVIEWPORT9 vp = graphic::device().impl().getViewPort();
		impl_->attrs_.insert(std::make_pair(impl_->position(ofs), Attr(x>0?x:vp.Width+x, y>0?y:vp.Height+y)));
		return *this;
	}

	Text &Text::setCenterdPos(float x, float y, int ofs)
	{
		D3DVIEWPORT9 vp = graphic::device().impl().getViewPort();
		impl_->attrs_.insert(std::make_pair(impl_->position(ofs), Attr(vp.Width/2+x, vp.Height/2+y)));
		return *this;
	}

	std::basic_ostream<_TCHAR> &Text::out() { return impl_->os; }

	HRslt Text::draw(graphic::SpriteBuffer &spr, graphic::FontTexture &fonttex) const
	{
		return proccess(&spr, fonttex, NULL, 0);
	}

	Point2f Text::getPos(graphic::FontTexture &fonttex, int ofs)
	{
		Point2f ret;
		proccess(NULL, fonttex, &ret, ofs);
		return ret;
	}

	/// 書き込みバッファを巻き戻す
	void Text::reset()
	{
		if(impl_->os.tellp() > 0) impl_->os.seekp(0);
		impl_->attrs_.clear();
	}

	HRslt Text::proccess(graphic::SpriteBuffer *spr, graphic::FontTexture &fonttex, Point2f *lastpos, int ofs) const
	{
		basic_string<_TCHAR>::size_type i = 0, size = impl_->os.tellp(), len = impl_->os.str().length();
		if(size > len) {
			GCTP_TRACE("Text::draw: \"! size <= len\" ("<<size<<_T("): 何も文字が書き込まれないままdrawが呼ばれたか、不正な文字が入力されました。"));
			return S_FALSE;
		}

		// 文字キャッシュ
		fonttex.gabage();
		const basic_string<_TCHAR> &str = impl_->os.str();
		const _TCHAR *text = str.c_str();
		Pointer<Font> font;
		graphic::FontGlyph glyph;
		// まずアスキーから。２回に分けるのは無駄だけど…
		while( i < size ) {
			int c, n;
			n = getChar(c, text);
			for(AttrMap::iterator attri = impl_->attrs_.lower_bound(i); attri != impl_->attrs_.upper_bound(i+ios::pos_type(n-1)); attri++) {
				if(attri->second.type == Attr::FONT) {
					font = attri->second.font.get();
				}
				else if(attri->second.type == Attr::BACKCOLOR) {
					glyph = fonttex.find(font, ' ');
					if(!glyph) fonttex.cacheAscii(font);
				}
			}
			if( _istascii(c) ) {
				if(!fonttex.isCached(font, c)) fonttex.cacheAscii(font);
			}
			text += n;
			i += n;
		}
		// 巻き戻し
		i = 0;
		text = str.c_str();
		bool began = false;
		while( i < size ) {
			int c, n;
			n = getChar(c, text);
			for(AttrMap::iterator attri = impl_->attrs_.lower_bound(i); attri != impl_->attrs_.upper_bound(i+ios::pos_type(n-1)); attri++) {
				if(attri->second.type == Attr::FONT) {
					font = attri->second.font.get();
				}
			}
			if( !_istascii(c) ) {
				if(!fonttex.isCached(font, c)) {
					if(!began) {
						fonttex.begin();
						began = true;
					}
					fonttex.cache(font, c);
				}
			}
			text += n;
			i += n;
		}
		if(began) fonttex.end();

		// 巻き戻し
		i = 0;
		font = 0;
		text = str.c_str();
		// 描画開始
		if(spr) spr->begin(fonttex, false);
		float left = 0, x = 0, y = 0; Color32 color(0, 0, 0), backcolor(0, 0, 0, 0);
		uint line_height = 0, default_line_height = 0, space_size = 0;
		while( i <= size ) {
			int c, n;
			if(i < size) n = getChar(c, text);
			else n = 1;
			for(AttrMap::iterator attri = impl_->attrs_.lower_bound(i); attri != impl_->attrs_.upper_bound(i+ios::pos_type(n-1)); attri++) {
				if(attri->second.type == Attr::POSITION) {
					left = x = attri->second.pos.x; y = attri->second.pos.y;
				}
				else if(attri->second.type == Attr::COLOR) {
					color = attri->second.color;
				}
				else if(attri->second.type == Attr::BACKCOLOR) {
					backcolor = attri->second.color;
				}
				else if(attri->second.type == Attr::FONT) {
					font = attri->second.font.get();
					glyph = fonttex.find(font, ' ');
					if(glyph) {
						default_line_height = static_cast<uint>(glyph.size.y);
						space_size = static_cast<uint>(glyph.size.x);
					}
				}
			}

			if(lastpos && !(i < size+ofs)) break;
			if(!(i < size)) break;

			text += n;
			i += n;

			if( c == _T('\n') || c == _T('\r') ) {
				x = left; y += line_height; line_height = default_line_height;
				continue;
			}
			else if( c == _T('\t') ) {
				x = left+graphic::toTabbed(x-left, space_size*4);
				continue;
			}
			if(isPrintChar(c)) {
				if(font) {
					glyph = fonttex.find(font, c);
					if(glyph) {
						if(line_height < glyph.size.y) line_height = static_cast<uint>(glyph.size.y);
						if(spr) {
							if(backcolor.a != 0) {
								graphic::FontGlyph _glyph = fonttex.find(font, ' ');
								if(_glyph) {
									graphic::SpriteDesc desc;
									desc.setPos(RectfC(x-1.5f, y-0.5f, x+glyph.size.x-0.5f+1, y+line_height-0.5f));
									desc.setUV(_glyph.uv);
									desc.setColor(backcolor);
									desc.setHilight(Color32(0,0,0));
									spr->draw(desc);
								}
							}
							if(!isSpaceChar(c)) {
								graphic::SpriteDesc desc;
								desc.setPos(RectfC(x-0.5f, y-0.5f, x+glyph.size.x-0.5f, y+glyph.size.y-0.5f));
								desc.setUV(glyph.uv);
								desc.setColor(color);
								desc.setHilight(Color32(0,0,0));
								spr->draw(desc);
							}
						}
					}
					x += glyph.size.x;
				}
				else PRNN(_T("フォントが設定されていません！"));
			}
		}
		if(lastpos) {
			lastpos->x = x;
			lastpos->y = y;
		}
		if(spr) {
			spr->end();
		}
		return S_OK;
	}

}} // namespace gctp::graphic
