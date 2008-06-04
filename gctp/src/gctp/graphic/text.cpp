/**@file
 * GameCatapult �����\���X�g���[���N���X
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
#include <set>
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
					FONT,
					CURSOR,
					ALIGNMENT,
				} type;

				Attr() {}
				Attr(float x, float y) : type(POSITION) { pos.x = x; pos.y = y; }
				Attr(Text::Alignment _alignment) : type(ALIGNMENT) { alignment = _alignment; }
				explicit Attr(D3DCOLOR _color) : type(COLOR), color(_color) {}
				explicit Attr(const Handle<Font> &_font) : type(FONT), font(_font) {}
				explicit Attr(Type _type) : type(_type) {}

				union {
					Pos pos;
					D3DCOLOR color;
					Text::Alignment alignment;
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

		class ForbiddenCharSet {
		public:
			ForbiddenCharSet()
			{
				to_linetop_.insert(_T(','));
				to_linetop_.insert(_T(')'));
				to_linetop_.insert(_T(']'));
				to_linetop_.insert(_T('�p'));
				to_linetop_.insert(_T('�l'));
				to_linetop_.insert(_T('�r'));
				to_linetop_.insert(_T('�t'));
				to_linetop_.insert(_T('�v'));
				to_linetop_.insert(_T('�x'));
				to_linetop_.insert(_T('�z'));				
				to_linetop_.insert(_T('�f'));
				to_linetop_.insert(_T('�R'));
				to_linetop_.insert(_T('�S'));
				to_linetop_.insert(_T('�['));
				to_linetop_.insert(_T('�@'));
				to_linetop_.insert(_T('�B'));
				to_linetop_.insert(_T('�D'));
				to_linetop_.insert(_T('�F'));
				to_linetop_.insert(_T('�H'));
				to_linetop_.insert(_T('�b'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('��'));
				to_linetop_.insert(_T('�X'));
				to_linetop_.insert(_T('�]'));
				to_linetop_.insert(_T('?'));
				to_linetop_.insert(_T('!'));
				to_linetop_.insert(_T('�H'));
				to_linetop_.insert(_T('�I'));
				to_linetop_.insert(_T('�E'));
				to_linetop_.insert(_T(':'));
				to_linetop_.insert(_T(';'));
				to_linetop_.insert(_T('�A'));
				to_linetop_.insert(_T('�B'));
				to_linetop_.insert(_T('.'));

				to_lineend_.insert(_T('('));
				to_lineend_.insert(_T('['));
				to_lineend_.insert(_T('�o'));
				to_lineend_.insert(_T('�k'));
				to_lineend_.insert(_T('�q'));
				to_lineend_.insert(_T('�s'));
				to_lineend_.insert(_T('�u'));
				to_lineend_.insert(_T('�w'));
				to_lineend_.insert(_T('�y'));
				to_lineend_.insert(_T('�e'));

				to_separate_.insert(_T('�c'));
				to_separate_.insert(_T('�d'));
				to_separate_.insert(_T('�\'));
				to_separate_.insert(_T('��'));
			}

			bool toLineTop(_TCHAR c) const
			{
				return to_linetop_.find(c) != to_linetop_.end();
			}
			bool toLineEnd(_TCHAR c) const
			{
				return to_lineend_.find(c) != to_lineend_.end();
			}
			bool toSeparate(_TCHAR c) const
			{
				return to_separate_.find(c) != to_separate_.end();
			}

		private:
			std::set<_TCHAR> to_linetop_;
			std::set<_TCHAR> to_lineend_;
			std::set<_TCHAR> to_separate_;
		};

		const ForbiddenCharSet &forbidden()
		{
			static ForbiddenCharSet ret;
			return ret;
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, graphic, Text, Object);
	
	using namespace detail;

	Text::Text() : impl_(new TextImpl), clip_(0,0,0,0), layout_(0,0,0,0)
	{
	}

	Text::~Text()
	{
		//TextImpl�����J���Ă��Ȃ��̂ŁA��̃f�X�g���N�^��錾���Ȃ��Ǝg�p����TextImpl�̍폜�@���킩��Ȃ��Ƃ�����
	}

	Text &Text::setPos(float x, float y, int ofs)
	{
		impl_->attrs_.insert(std::make_pair(impl_->position(ofs), Attr(x, y)));
		return *this;
	}

	Text &Text::setColor(Color32 color, int ofs)
	{
		impl_->attrs_.insert(std::make_pair(impl_->position(ofs), Attr(color)));
		return *this;
	}

	Text &Text::setFont(const Handle<Font> &font, int ofs)
	{
		impl_->attrs_.insert(std::make_pair(impl_->position(ofs), Attr(font)));
		return *this;
	}

	Text &Text::setAlignment(Alignment alignment, int ofs)
	{
		impl_->attrs_.insert(std::make_pair(impl_->position(ofs), alignment));
		return *this;
	}

	std::basic_ostream<_TCHAR> &Text::out() { return impl_->os; }

	HRslt Text::draw(graphic::SpriteBuffer &spr, graphic::FontTexture &fonttex) const
	{
		return proccess(&spr, fonttex, NULL, NULL, 0);
	}

	HRslt Text::draw(SpriteDescVector &descvec, FontTexture &fonttex) const
	{
		return proccess(NULL, fonttex, &descvec, NULL, 0);
	}

	Point2f Text::getPos(graphic::FontTexture &fonttex, int ofs)
	{
		Point2f ret;
		proccess(NULL, fonttex, NULL, &ret, ofs);
		return ret;
	}

	void Text::setClip(const Rect &rc)
	{
		clip_ = rc;
	}

	void Text::setLayoutRectangle(const Rect &rc)
	{
		layout_ = rc;
	}

	/// �������݃o�b�t�@�������߂�
	void Text::reset()
	{
		if(impl_->os.tellp() > 0) impl_->os.seekp(0);
		impl_->attrs_.clear();
	}

	HRslt Text::proccess(graphic::SpriteBuffer *spr, graphic::FontTexture &fonttex, SpriteDescVector *descvec, Point2f *lastpos, int ofs) const
	{
		basic_string<_TCHAR>::size_type i = 0, size = impl_->os.tellp(), len = impl_->os.str().length();
		if(size > len) {
			//GCTP_TRACE("Text::draw: \"! size <= len\" ("<<size<<_T("): �����������������܂�Ȃ��܂�draw���Ă΂ꂽ���A�s���ȕ��������͂���܂����B"));
			return S_FALSE;
		}

		// �����L���b�V��
		const basic_string<_TCHAR> &str = impl_->os.str();
		const _TCHAR *text = str.c_str();
		Pointer<Font> font;
		graphic::FontGlyph glyph;
		bool began = false;
		while( i < size ) {
			int c, n;
			n = getChar(c, text);
			if(n == 0) break;
			for(AttrMap::iterator attri = impl_->attrs_.lower_bound(i); attri != impl_->attrs_.upper_bound(i+ios::pos_type(n-1)); attri++) {
				if(attri->second.type == Attr::FONT) {
					font = attri->second.font.get();
				}
			}
			if(!fonttex.isCached(font, c)) {
				if(!began) {
					fonttex.begin();
					began = true;
				}
				fonttex.cache(font, c);
			}
			text += n;
			i += n;
		}
		if(began) fonttex.end();

		// �����߂�
		i = 0;
		font = 0;
		text = str.c_str();
		//const _TCHAR *linetop_text = text;
		//int linetop_i = i; // �܂����p���Ė����̂Łc
		// �`��J�n
		bool setvp = false;
		ViewPort vp_bu;
		if(clip_.width() > 0 || clip_.height() > 0) {
			setvp = true;
			if(spr) {
				Point2 screen = getScreenSize();
				vp_bu = getViewPort();
				ViewPort vp;
				vp.min_z = 0;
				vp.max_z = 1;
				if(clip_.width() > 0) {
					vp.x = clip_.left;
					vp.width = clip_.width();
				}
				else {
					vp.x = 0;
					vp.width = screen.x;
				}
				if(clip_.height() > 0) {
					vp.y = clip_.top;
					vp.height = clip_.height();
				}
				else {
					vp.y = 0;
					vp.height = screen.y;
				}
				setViewPort(vp);
			}
		}
		if(spr)	spr->begin(fonttex, false);
		int prev_c = 0, now_disp_count = 0;
		float x = (float)layout_.left, y = (float)layout_.top;
		Text::Alignment alignment = Text::LEFT;
		Color32 color(0, 0, 0);
		uint line_height = 0, default_line_height = 0, space_size = 0;
		bool loopback = false;
		while( i <= size ) {
			int c, n;
			if(i < size) n = getChar(c, text);
			else n = 1;
			if(n == 0) break;
			for(AttrMap::iterator attri = impl_->attrs_.lower_bound(i); attri != impl_->attrs_.upper_bound(i+ios::pos_type(n-1)); attri++) {
				if(attri->second.type == Attr::POSITION) {
					x = layout_.left+attri->second.pos.x; y = layout_.top+attri->second.pos.y;
					prev_c = 0;
				}
				else if(attri->second.type == Attr::COLOR) {
					color = attri->second.color;
				}
				else if(attri->second.type == Attr::FONT) {
					font = attri->second.font.get();
					glyph = fonttex.find(font, ' ');
					if(glyph) {
						default_line_height = static_cast<uint>(glyph.size.y);
						space_size = static_cast<uint>(glyph.size.x);
					}
				}
				else if(attri->second.type == Attr::ALIGNMENT) {
					alignment = attri->second.alignment;
				}
			}

			if(lastpos && !(i < size+ofs)) break;
			if(!(i < size)) break;

			text += n;
			i += n;

			if(c == _T('\n') || c == _T('\r')) {
				if(!loopback) {
					x = (float)layout_.left; y += line_height; line_height = default_line_height;
				}
				loopback = false;
				prev_c = c;
				continue;
			}
			else if( c == _T('\t') ) {
				x = layout_.left+toTabbed(x-layout_.left, space_size*4);
				prev_c = c;
				continue;
			}
			loopback = false;

			if(font && (i == n || prev_c == _T('\n')) && (alignment == Text::CENTER || alignment == Text::RIGHT)) {
				// ���̍s�̒�����}��
				basic_string<_TCHAR>::size_type _i = i;
				float width = 0;
				int n = 1, c = 0;
				const _TCHAR *_text = text;
				if(_i < size) n = getChar(c, _text);
				while( _i <= size && n > 0 && c != _T('\n')) {
					if(_i < size) n = getChar(c, _text);
					if(c == _T('\t')) {
						width += toTabbed(0, space_size*4);
					}
					else if(isPrintChar(c)) {
						glyph = fonttex.find(font, c);
						if(glyph) width += glyph.size.x;
					}
					_text += n;
					_i += n;
				}
				if(width < layout_.width()) {
					if(alignment == Text::CENTER) {
						x += (float)(layout_.width()-width)/2;
					}
					else {
						x += (float)(layout_.width()-width);
					}
				}
			}

			if(isPrintChar(c)) {
				if(font) {
					glyph = fonttex.find(font, c);
					if(glyph) {
						if(line_height < glyph.size.y) line_height = static_cast<uint>(glyph.size.y);
						if(layout_.width() > 0) {
							// �܂�Ԃ�����
							if(x > layout_.right+font->maxWidth()
							|| (x+glyph.size.x > layout_.right && !forbidden().toLineEnd((_TCHAR)prev_c) && !forbidden().toLineTop((_TCHAR)c) && !(forbidden().toSeparate((_TCHAR)c)&&prev_c==c))
							|| (forbidden().toLineEnd((_TCHAR)c) && x+glyph.size.x+font->maxWidth() > layout_.right)) {
								x = (float)layout_.left; y += line_height;
								loopback = true;
							}
						}
						if(spr || descvec) {
							if(!isSpaceChar(c) && (!setvp || clip_.isHit(RectC((uint_t)x, (uint_t)y, (uint_t)(x+glyph.size.x), (uint_t)(y+line_height))))) {
								if(font->exStyle()!=Font::EX_NONE) {
									// �e�����ܕ����̂Ƃ��̓��ʏ���
									if((c == _T('�\') // �S�p�_�b�V��
										|| c == _T('��') // �r��
										|| c == _T('�c') // �R�_���[�_
										) && prev_c == c)
									{
										// �A�����Ă����ꍇ�A�㑱�͍��P�h�b�g�����
										glyph.size.x -= 1;
										glyph.uv.left += 1.0f/fonttex.size().x;
										x -= 1;
									}
								}
								graphic::SpriteDesc desc;
								desc.setPos(RectfC(x, y, x+glyph.size.x, y+glyph.size.y));
								desc.setUV(glyph.uv);
								desc.setColor(color);
								desc.setHilight(Color32(0,0,0));
								if(spr) spr->draw(desc);
								if(descvec) descvec->descs.push_back(desc);
							}
						}
					}
					x += glyph.size.x;
					if(loopback) {
						line_height = default_line_height;
					}
				}
				else PRNN(_T("�t�H���g���ݒ肳��Ă��܂���I"));
			}
			prev_c = c;
		}
		if(lastpos) {
			lastpos->x = x;
			lastpos->y = y;
		}
		if(spr) {
			spr->end();
			if(setvp) {
				setViewPort(vp_bu);
			}
		}
		return S_OK;
	}

}} // namespace gctp::graphic
