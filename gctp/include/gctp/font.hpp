#ifndef _GCTP_FONT_HPP_
#define _GCTP_FONT_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �t�H���g���\�[�X�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 21:29:23
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointer.hpp>
#include "smartwin/font.h"

namespace gctp {

	/** �t�H���g���\�[�X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 0:36:12
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Font : public Object {
	public:
		/// �X�^�C���萔
		enum Style {
			NORMAL = 0			/**< �ʏ�*/,
			BOLD   = 1			/**< ����*/,
			ITALIC = 1<<1		/**< �Α�*/,
			UNDERLINE = 1<<2	/**< ����*/,
			STRIKEOUT = 1<<3	/**< �ł�������*/,
			FIXEDPITCH = 1<<4	/**< �Œ�s�b�`*/,
			SHADOW = 1<<5	/**< �e����(�ܕ����Ɠ����Ɏw�肵���ꍇ�ܕ������L���ɂȂ�)*/,
			OUTLINE = 1<<6	/**< �ܕ����i�֊s�t�j*/,
			_FORCE_DWORD_ = 0x7FFFFFFF,
		};
		enum ExStyle {
			EX_NONE,
			EX_SHADOW,
			EX_OUTLINE,
		};
		
		/// �t�H���g����
		HRslt setUp(const _TCHAR *fontname, uint height, uint32_t flag=NORMAL);

		/// �t�H���g����
		HRslt setUp(const _TCHAR *name);

		/// �t�H���g�n���h�����擾
		HFONT get() const { return font_->getHandle(); }

		/// SmartWin�t�H���g�I�u�W�F�N�g���擾
		SmartWin::FontPtr getFontPtr() const { return font_; }

		/// LOGFONT�\���̂𖄂߂�
		bool getLogFont(LOGFONT &logfont) const
		{
			return font_.get() && font_->getHandle() && (::GetObject(font_->getHandle(), sizeof(LOGFONT), &logfont) == sizeof(LOGFONT));
		}

		/// �����̍�����Ԃ��܂�
		uint height() const { return height_; }

		/// �����̍ő�̕���Ԃ��܂�
		int maxWidth() const;

		/// �����Z���̃T�C�Y��Ԃ��܂�
		uint cellsize() const {
			// �e�����A�ܕ����̏ꍇ�̕␳
			if(exstyle_ == EX_OUTLINE) return cellsize_ + 2;
			else if(exstyle_ == EX_SHADOW) return cellsize_ + 1;
			return cellsize_;
		}

		ExStyle exStyle() const { return exstyle_; }

		/// �t�H���g�̍�������A�����Z���T�C�Y���v�Z����
		static uint cellsize(uint height);

		/// �p�����[�^����setUp�ɓn��������p�����[�^�����⏕�֐�
		static SmartUtil::tstring paramToString(const _TCHAR *fontname, uint height, uint32_t style);

		/// ������p�����[�^����A�e�p�����[�^�ɕϊ�����⏕�֐�
		static bool stringToParam(const _TCHAR *param, SmartUtil::tstring &fontname, uint &height, uint32_t &style);

	private:
		ExStyle exstyle_;
		uint height_;
		uint cellsize_;
		SmartWin::FontPtr font_;
	};

} //namespace gctp

#endif //_GCTP_FONT_HPP_