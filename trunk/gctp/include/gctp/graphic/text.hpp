#ifndef _GCTP_GRAPHIC_TEXT_HPP_
#define _GCTP_GRAPHIC_TEXT_HPP_
/**@file
 * GameCatapult �����\���X�g���[���N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 1:42:52
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <iosfwd> // for std::ostream
#include <gctp/object.hpp>
#include <gctp/class.hpp>
#include <gctp/color.hpp>
#include <gctp/vector.hpp>
#include <boost/scoped_ptr.hpp>

namespace gctp {
	class Font;
	namespace graphic {
		class SpriteDescVector;
		class SpriteBuffer;
		class FontTexture;
		class FontTextureSet;
		namespace detail {
			class TextImpl;
		}
	}
}

namespace gctp { namespace graphic {

	/** �\�������X�g���[��
	 *
	 * @todo graphic::SpriteBuffer �� graphic::FontTexture ���A����ɍ��悤�ɂ���
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 9:14:27
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Text : public Object {
	public:
		Text();
		~Text();

		/// �g�p����X�v���C�g�o�b�t�@�ƃt�H���g�e�N�X�`�����w�肵�ĕ`��
		HRslt draw(SpriteBuffer &spr, FontTexture &fonttex) const;
		/// �g�p����X�v���C�g�o�b�t�@�ƃt�H���g�e�N�X�`���Z�b�g���w�肵�ĕ`��
		HRslt draw(SpriteBuffer &spr, FontTextureSet &fonttex) const;
		/// ���ڕ`�悹���ASpriteDescVector�ɃX�v���C�g��`�𗭂ߍ���
		HRslt draw(SpriteDescVector &descvec, FontTexture &fonttex) const;
		/// ���ڕ`�悹���ASpriteDescVector�ɃX�v���C�g��`�𗭂ߍ���
		HRslt draw(SpriteDescVector &descvec, FontTextureSet &fonttex) const;

		Text &setPos(float x, float y, int ofs = 0);
		Text &setColor(Color32 color, int ofs = 0);
		Text &setBackColor(Color32 color, int ofs = 0);
		Text &setFont(const Handle<Font> &font, int ofs = 0);

		Point2f getPos(graphic::FontTexture &fonttex, int ofs = 0);
		Point2f getPos(graphic::FontTextureSet &fonttex, int ofs = 0);
		void reset();

		void setClip(const Rect &rc);
		void setLayoutRectangle(const Rect &rc);
		std::basic_ostream<_TCHAR> &out();

	GCTP_DECLARE_CLASS

	private:
		HRslt proccess(graphic::SpriteBuffer *spr, graphic::FontTexture &font, SpriteDescVector *descvec, Point2f *lastpos, int ofs) const;
		HRslt proccess(graphic::SpriteBuffer *spr, graphic::FontTextureSet &font, SpriteDescVector *descvec, Point2f *lastpos, int ofs) const;
		boost::scoped_ptr<detail::TextImpl> impl_;
		RectC clip_;
		RectC layout_;
	};

}} //namespace gctp::graphic

#endif //_GCTP_GRAPHIC_TEXT_HPP_