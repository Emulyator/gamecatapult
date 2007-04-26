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
				struct FixPitch {
					bool fixed;
					short pitch;
				};
				enum Type {
					POSITION,
					COLOR,
					BACKCOLOR,
					FIXPITCH,
					PITCHWIDTH,
					FONT,
					CURSOR,
				} type;

				Attr() {}
				Attr(float x, float y) : type(POSITION) { pos.x = x; pos.y = y; }
				explicit Attr(D3DCOLOR _color) : type(COLOR), color(_color) {}
				explicit Attr(D3DCOLOR _color, bool dummy) : type(BACKCOLOR), color(_color) {}
				explicit Attr(const Handle<Font> &_font) : type(FONT), font(_font) {}
				explicit Attr(bool fixed, short pitch) : type(FIXPITCH) { fixpitch.fixed = fixed; fixpitch.pitch = pitch; }
				explicit Attr(Type _type) : type(_type) {}

				union {
					Pos pos;
					D3DCOLOR color;
					FixPitch fixpitch;
					unsigned char pitch_width;
				};
				Handle<Font> font;
			};
			typedef multimap<string::size_type, Attr> AttrMap;
		}

		class TextDetail {
		public:
			TextDetail()
			{
				default_font_ = gctp::createOnDB<Font>(_T(",10,BOLD"));
				gctp::db().set(_T("defaultfont"), default_font_);
			}

			basic_string<_TCHAR>::size_type position(int ofs)
			{
				int n = static_cast<int>(os.tellp())+ofs;
				if(n > 0) return n;
				return 0;
			}

			basic_ostringstream<_TCHAR> os;
			AttrMap attrs_;
			Pointer<Font> default_font_;
			Pointer<graphic::FontTexture> default_texture_;
			Pointer<graphic::SpriteBuffer> default_sprite_;
		};

	}

	GCTP_IMPLEMENT_CLASS_NS(gctp, Text, Object);
	
	using namespace detail;

	Text::Text() : detail_(new detail::TextDetail)
	{
	}

	Text::~Text()
	{
	}

	Text &Text::setPos(float x, float y, int ofs) {
		detail_->attrs_.insert(std::make_pair(detail_->position(ofs), Attr(x, y)));
		return *this;
	}

	Text &Text::setColor(Color32 color, int ofs) {
		detail_->attrs_.insert(std::make_pair(detail_->position(ofs), Attr(color)));
		return *this;
	}

	Text &Text::setBackColor(Color32 color, int ofs) {
		detail_->attrs_.insert(std::make_pair(detail_->position(ofs), Attr(color, true)));
		return *this;
	}

	Text &Text::setFont(const Handle<Font> &font, int ofs) {
		detail_->attrs_.insert(std::make_pair(detail_->position(ofs), Attr(font)));
		return *this;
	}

	Text &Text::setClumpPos(float x, float y, int ofs)
	{
		D3DVIEWPORT9 vp = graphic::device().impl().getViewPort();
		detail_->attrs_.insert(std::make_pair(detail_->position(ofs), Attr(x>0?x:vp.Width+x, y>0?y:vp.Height+y)));
		return *this;
	}

	Text &Text::setCenterdPos(float x, float y, int ofs)
	{
		D3DVIEWPORT9 vp = graphic::device().impl().getViewPort();
		detail_->attrs_.insert(std::make_pair(detail_->position(ofs), Attr(vp.Width/2+x, vp.Height/2+y)));
		return *this;
	}

	Text &Text::setFixedPitch(bool yes, int pitch, int ofs)
	{
		detail_->attrs_.insert(std::make_pair(detail_->position(ofs), Attr(yes, pitch)));
		return *this;
	}

	std::basic_ostream<_TCHAR> &Text::out() { return detail_->os; }

	HRslt Text::draw() const
	{
		if(!detail_->default_sprite_) detail_->default_sprite_ = graphic::createOnDB<graphic::SpriteBuffer>(_T("SPRITEBUFFER"));
		if(!detail_->default_texture_) detail_->default_texture_ = graphic::createOnDB<graphic::FontTexture>(_T("FONTTEXTURE"));
		return proccess(const_cast<graphic::SpriteBuffer *>(detail_->default_sprite_.get()), *detail_->default_texture_, NULL, 0);
	}

	HRslt Text::draw(graphic::SpriteBuffer &spr, graphic::FontTexture &font) const
	{
		return proccess(&spr, font, NULL, 0);
	}

	Point2f Text::getPos(graphic::FontTexture &font, int ofs)
	{
		Point2f ret;
		proccess(NULL, font, &ret, ofs);
		return ret;
	}

	Point2f Text::getPos(int ofs)
	{
		if(!detail_->default_texture_) detail_->default_texture_ = graphic::createOnDB<graphic::FontTexture>(_T("FONTTEXTURE"));
		Point2f ret;
		proccess(NULL, *detail_->default_texture_, &ret, ofs);
		return ret;
	}

	HRslt Text::proccess(graphic::SpriteBuffer *spr, graphic::FontTexture &font, Point2f *lastpos, int ofs) const
	{
		Pointer<Font> _font = detail_->default_font_;
		basic_string<_TCHAR>::size_type i = 0, size = detail_->os.tellp(), len = detail_->os.str().length();
		if(size > len) {
			GCTP_TRACE("Text::draw: \"! size <= len\" ("<<size<<_T("): 何も文字が書き込まれないままdrawが呼ばれたか、不正な文字が入力されました。"));
			return S_FALSE;
		}
		const basic_string<_TCHAR> &str = detail_->os.str();
		const _TCHAR *text = str.c_str();
		graphic::FontGlyph glyph;

		font.gabage();

		// 文字キャッシュ
		// まずアスキーから。２回に分けるのは無駄だけど…
		while( i < size ) {
			int c, n;
			n = getChar(c, text);
			for(AttrMap::iterator attri = detail_->attrs_.lower_bound(i); attri != detail_->attrs_.upper_bound(i+ios::pos_type(n-1)); attri++) {
				if(attri->second.type == Attr::FONT) {
					_font = attri->second.font.get();
				}
				else if(attri->second.type == Attr::BACKCOLOR) {
					glyph = font.find(_font, ' ');
					if(!glyph) font.cacheAscii(_font);
				}
			}
			if( _istascii(c) ) {
				if(!font.isCached(_font, c)) font.cacheAscii(_font);
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
			for(AttrMap::iterator attri = detail_->attrs_.lower_bound(i); attri != detail_->attrs_.upper_bound(i+ios::pos_type(n-1)); attri++) {
				if(attri->second.type == Attr::FONT) {
					_font = attri->second.font.get();
				}
			}
			if( !_istascii(c) ) {
				if(!font.isCached(_font, c)) {
					if(!began) {
						font.begin();
						began = true;
					}
					font.cache(_font, c);
				}
			}
			text += n;
			i += n;
		}
		if(began) font.end();

		// 巻き戻し
		i = 0;
		text = str.c_str();
		// 描画開始
		if(spr) spr->begin(font, false);
		float left = 0, x = 0, y = 0; Color32 color(0, 0, 0), backcolor(0, 0, 0, 0);
		uint line_height, default_line_height;
		glyph = font.find(_font, ' '); line_height = default_line_height = static_cast<uint>(glyph.size.y);
		uint space_size = static_cast<uint>(glyph.size.x);
		bool fixpitch = false; int fixed_pitch = static_cast<int>(ceilf(_font->maxWidth()*1.1f/2.0f));
		while( i <= size ) {
			int c, n;
			if(i < size) n = getChar(c, text);
			else n = 1;
			for(AttrMap::iterator attri = detail_->attrs_.lower_bound(i); attri != detail_->attrs_.upper_bound(i+ios::pos_type(n-1)); attri++) {
				if(attri->second.type == Attr::POSITION) {
					left = x = attri->second.pos.x; y = attri->second.pos.y;
				}
				else if(attri->second.type == Attr::COLOR) {
					color = attri->second.color;
				}
				else if(attri->second.type == Attr::BACKCOLOR) {
					backcolor = attri->second.color;
				}
				else if(attri->second.type == Attr::FIXPITCH) {
					fixpitch = attri->second.fixpitch.fixed;
					if(attri->second.fixpitch.pitch == 0) {
						if(_font) fixed_pitch = static_cast<int>(ceilf(_font->maxWidth()*1.1f/2.0f));
					}
					else fixed_pitch = attri->second.fixpitch.pitch;
				}
				else if(attri->second.type == Attr::FONT) {
					_font = attri->second.font.get();
					glyph = font.find(_font, ' ');
					if(glyph) {
						line_height = default_line_height = static_cast<uint>(glyph.size.y);
						uint space_size = static_cast<uint>(glyph.size.x);
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
				if(fixpitch) x = left+graphic::toTabbed(x-left, fixed_pitch*4);
				else x = left+graphic::toTabbed(x-left, space_size*4);
				continue;
			}
			if(isPrintChar(c)) {
				if(_font) {
					glyph = font.find(_font, c);
					if(glyph) {
						if(line_height < glyph.size.y) line_height = static_cast<uint>(glyph.size.y);
						if(spr) {
							if(backcolor != Color32(0,0,0,0)) {
								graphic::FontGlyph _glyph = font.find(_font, ' ');
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
					if(fixpitch) x += fixed_pitch*n;
					else x += glyph.size.x;
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
			// sprがNULLで呼び出されるというのは事前検査だから、
			// バッファを巻き戻してはまずい
			detail_->os.seekp(0);
			detail_->attrs_.clear();
		}
		return S_OK;
	}

}} // namespace gctp::graphic
