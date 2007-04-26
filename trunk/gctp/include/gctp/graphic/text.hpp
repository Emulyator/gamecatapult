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
		class SpriteBuffer;
		class FontTexture;
		class FontTextureSet;
		namespace detail {
			class TextDetail;
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

		/// �X�v���C�g�o�b�t�@�ƃt�H���g�e�N�X�`���������Ŏ擾
		HRslt draw() const;
		/// �g�p����X�v���C�g�o�b�t�@�ƃt�H���g�e�N�X�`�����w�肵�ĕ`��
		HRslt draw(graphic::SpriteBuffer &spr, graphic::FontTexture &font) const;

		HRslt draw(graphic::SpriteBuffer &spr, graphic::FontTextureSet &font) const;

		Text &setPos(float x, float y, int ofs = 0);
		Text &setClumpPos(float x, float y, int ofs = 0);
		Text &setCenterdPos(float x, float y, int ofs = 0);
		Text &setColor(Color32 color, int ofs = 0);
		Text &setBackColor(Color32 color, int ofs = 0);
		Text &setFont(const Handle<Font> &font, int ofs = 0);
		Text &setFixedPitch(bool yes, int pitch = 0, int ofs = 0);

		Point2f getPos(graphic::FontTexture &font, int ofs = 0);
		Point2f getPos(int ofs = 0);

		std::basic_ostream<_TCHAR> &out();

	GCTP_DECLARE_CLASS

	private:
		HRslt proccess(graphic::SpriteBuffer *spr, graphic::FontTexture &font, Point2f *lastpos, int ofs) const;
		boost::scoped_ptr<detail::TextDetail> detail_;
	};

}} //namespace gctp::graphic

#endif //_GCTP_GRAPHIC_TEXT_HPP_