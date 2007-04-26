#ifndef _GCTP_GRAPHIC_FONTTEXTURE_HPP_
#define _GCTP_GRAPHIC_FONTTEXTURE_HPP_
/**@file
 * GameCatapult �t�H���g�e�N�X�`�����\�[�X�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 1:42:52
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic/texture.hpp>
#include <gctp/types.hpp>
#include <gctp/vector.hpp>
#include <boost/scoped_ptr.hpp>

namespace gctp {
	
	class Font;

	namespace graphic {
		
		class SpriteBuffer;

		namespace detail {
			struct AsciiAttr;
			class DrawContext;
			class FontTextureDetail;
			class FontTextureSetDetail;
		}
	}
}

namespace gctp { namespace graphic {

	/** �����ɑΉ�����UV���
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 2:54:10
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct FontGlyph {
		Rectf	uv;
		Size2f	size;
		// @{
		/** �L�����H
		 *
		 * FontTeture::find�ŁA�u�܂��L���b�V������Ă��Ȃ��v���������ْl�������Ă��Ȃ����ǂ����`�F�b�N
		 * ���łɃL���b�V������Ă���Ȃ�Atrue���Ԃ�
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/23 4:51:52
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		operator bool() const { return size.x >= 0 && size.y >= 0; }
		bool operator !() const { return size.x < 0 || size.y < 0; }
		// @}
	};

	/** �g�p���镶���݂̂������_�����O���ăL���b�V������t�H���g�e�N�X�`��
	 *
	 * �ő�e�N�X�`���T�C�Y��1024�ȉ��ł���΍ő�e�N�X�`���T�C�Y���A�����łȂ����
	 * 1024*1024�Ő��삵�A�e�N�X�`�����c��64�������A1bit�t���O�ŋ󂫂�̈�Ǘ�����B
	 * 1024�ł�16*16���ŏ��̈�A512*512�ł�8*8�A256�ł�4*4�ɂȂ�B�ꖇ�ōő�4096��������i�[�ł���v�Z�B
	 * �A�h���X�͂P�����̏����ŕ\����A���D��g���o�[�X�����Ƃ���quadtree�ł̗̈��\���B�i���ꂪalloc_�̒l�j
	 * 0�̓g�b�v�m�[�h�A1,2,3,4�����ꂼ��S�������ꂽ�̈�Ɂc�ƌ�����ɑΉ�����B
	 * �ő�64�����́A���x�����Ō�����6���x���܂ŁA�ƌ������ƁB
	 * �e�N�X�`���̃T�C�Y�܂ł̓����_�ł��Ă����킯�����A���ꂾ�ƂP�����łP�e�N�N�`�������Ă��܂��̂ŁA
	 * ������x����ݒ�ł���B�i���݂͌Œ��3�B1024�̂Ƃ��A192�h�b�g)�B
	 *
	 * @todo �̈���g���܂킹��悤�ɂ���i���͗��ߍ��ވ���j
	 * @todo �����Ń����_�ł��Ȃ��ꍇ�́A�g��\������悤�ɂ���
	 * @todo �����T�C�Y�̏�����ς�
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/18 2:46:24
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class FontTexture : public Texture {
	public:
		enum { LEVEL = 6 };

		/// �R���X�g���N�^
		FontTexture();

		/// �ŏ��̈�̃T�C�Y
		uint cellSize() const
		{
			return size().x>>LEVEL;
		}
		/// �ő�̈�̃T�C�Y
		uint maxCellSize() const
		{
			return size().x>>(LEVEL/2);
		}

		/// �s�N�Z���P�ʂ��u���b�N�P�ʂ�
		uint toBlockSize(uint pixelsize)
		{
			return (pixelsize+cellSize()-1)/cellSize();
		}

		/// �e�N�X�`���̈�̃Z�b�g�A�b�v�icreateOnDB���p�j
		HRslt setUp(const _TCHAR *name) { return setUp(); }

		/// �e�N�X�`���̈�̃Z�b�g�A�b�v
		HRslt setUp();

		bool isCached(const Handle<Font> font, int c) const;

		FontGlyph find(const Handle<Font> font, int c) const;

		/// Ascii��������x�ɃL���b�V��
		void cacheAscii(const Handle<Font> font);

		/// �����L���b�V���J�n
		void begin();

		/// �������L���b�V��
		void cache(const Handle<Font> font, int c);

		/// �����L���b�V���I��
		void end();

		/// ���ׂĊJ��
		void clear();

		/// �A�X�L�[���������܂�ŏ��̕����v�Z
		static uint getSmallestWidth(const Font &font, uint unit);

		/// �̈�̃K�x�[�W
		void gabage();

	GCTP_DECLARE_CLASS

	private:
		HRslt setUpAscii(detail::AsciiAttr &attr, const Font &font, ulong width, ulong height, int ofsx = 0, int ofsy = 0);
		int alloc(int level);
		std::auto_ptr<detail::DrawContext> cntx_;
		boost::scoped_ptr<detail::FontTextureDetail> detail_;
		//float scale_;
	};

	/// FontTexture���g���������\��
	void drawText(SpriteBuffer &spr, FontTexture &font, const Handle<Font> cfont, const Point2f &pos, Color32 color, const _TCHAR *_text);

	/// �^�u���E�ɐ���
	inline float toTabbed(float x, int tabwidth) { return static_cast<float>(static_cast<int>(x)/tabwidth*tabwidth+tabwidth); }

	/// �����񒆂̎w�蕶���̈ʒu���擾
	Point2f getTextPos(const FontTexture &font, const Handle<Font> cfont, const _TCHAR *text, uint idx);

	/** �����̃t�H���g�e�N�X�`�����܂Ƃ߂ĊǗ�����
	 *
	 * ���ݎg���Ă��Ȃ�(�ł��ĂȂ�����)
	 * @todo ����𓮍삳����
	 * @todo �ƌ������A����K�v���H
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 11:20:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class FontTextureSet : public Object {
	public:
		/// �R���X�g���N�^
		FontTextureSet();
		/// �t�H���g��ǉ�
		HRslt add(const Handle<Font> font);
		/// �t�H���g�e�N�X�`�����擾
		Handle<FontTexture> get(const Handle<Font> font);
	GCTP_DECLARE_FACTORY;
	private:
		boost::scoped_ptr<detail::FontTextureDetail> detail_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_FONTTEXTURE_HPP_