/** @file
 * GameCatapult �t�H���g���\�[�X�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 22:56:37
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/types.hpp>
#include <gctp/font.hpp>
#include <gctp/tcsv.hpp>
#include <gctp/dbgout.hpp>
#include "SmartWin.h"

using namespace std;

namespace gctp {

	/** �t�H���g�쐬
	 *
	 * height�̓s�N�Z���T�C�Y�Ŏw��B�}�b�v���[�h��MM_TEXT��O��ɂ��Ă���B
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 2:12:19
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Font::setUp(
		const _TCHAR *facename /**< �t�H���g��*/,
		uint          height   /**< �T�C�Y*/,
		uint32_t      style    /**< �X�^�C��(BOLD,ITALIC)*/
	) {
		if(style & OUTLINE) exstyle_ = EX_OUTLINE;
		else if(style & SHADOW) exstyle_ = EX_SHADOW;
		else exstyle_ = EX_NONE;
		height_ = height;
		// �_���T�C�Y�ɕϊ��B�_���f�o�C�X�ł̏c�̃s�N�Z��/�C���` �ł����ăf�t�H���g��dpi�ł���72�Ŋ����Ă��ƁA
		// �C���`�ł̍����̒l���o�Ă���A�ƁB-�ɂ���ƁA�����Z���̍�������������f�B���O
		// (�A�N�Z���g�L���Ȃǂ̂��߂̃X�y�[�X) �̍������������
		int  _height   = -(int)cellsize(height);
		int  _weight   = (style & BOLD)   ? FW_BOLD : FW_NORMAL;
		cellsize_ = -_height;
		// �t�H���g����BANTIALIASED_QUALITY���w�肵�Ă��邪�A�A���`�������邩�ǂ����̓V�X�e���Ɉˑ�����̂�
		// �ۏ؂���Ȃ��B
		font_ = SmartWin::FontPtr(new SmartWin::Font(facename
			, _height, 0, _weight, DEFAULT_CHARSET
			, (style & ITALIC) ? true : false
			, (style & UNDERLINE) ? true : false
			, (style & STRIKEOUT) ? true : false
			, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
			, ANTIALIASED_QUALITY, (style & FIXEDPITCH) ? FIXED_PITCH : VARIABLE_PITCH));
		return font_ ? S_OK : E_FAIL;
	}

	/** ���ׂĕ�����Ńp�����[�^��n���o�[�W�����B
	 *
	 * �J���}��؂�ŁA�p�����[�^��n���B
	 * "Lucida Console,18,NORMAL" "MS UI Gothic,10,BOLD|ITALIC"�̂悤�ɁB
	 *
	 * gctp::createOnDB�Ő��삷��ꍇ�A���������g����
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 23:07:15
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Font::setUp(const _TCHAR *name)
	{
		SmartUtil::tstring fontname(name);
		uint height;
		uint32_t style;
		if(!stringToParam(name, fontname, height, style)) return E_INVALIDARG;
		return setUp(fontname.c_str(), height, style);
	}

	/** ���O�Ƀt�H���g�T�C�Y����Z���T�C�Y���m�F
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 6:32:51
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	uint Font::cellsize(uint height)
	{
		HDC hdc = ::CreateCompatibleDC(0);
		::SetMapMode(hdc, MM_TEXT);
		uint ret = ::MulDiv(height, ::GetDeviceCaps(hdc, LOGPIXELSY), 72);
		::DeleteDC(hdc);
		return ret;
	}

	int Font::maxWidth() const
	{
		HDC hdc = ::CreateCompatibleDC(0);
		::SetMapMode(hdc, MM_TEXT);

		SelectFont(hdc, font_->getHandle()); // �}�N���Ȃ̂�

		::TEXTMETRIC tm; // ���݂̃t�H���g���
		::GetTextMetrics(hdc, &tm);
		::DeleteDC(hdc);
		// �e�����A�ܕ����̏ꍇ�̕␳
		if(exstyle_ == EX_OUTLINE) tm.tmMaxCharWidth += 2;
		else if(exstyle_ == EX_SHADOW) tm.tmMaxCharWidth += 1;
		return tm.tmMaxCharWidth;
	}

	/** �p�����[�^����setUp�ɓn��������p�����[�^�����⏕�֐�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 1:46:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	SmartUtil::tstring Font::paramToString(const _TCHAR *fontname, uint height, uint32_t style)
	{
		CSVRowBase< _TCHAR, std::char_traits<_TCHAR> > csv;
		csv.push_back(fontname);
		csv.push_back(boost::lexical_cast<SmartUtil::tstring>(height));
		csv.push_back(boost::lexical_cast<SmartUtil::tstring>(style));
#ifdef _UNICODE
		wstringstream ioss;
#else
		stringstream ioss;
#endif
		ioss << csv;
		return ioss.str();
	}

	/** ������p�����[�^����A�e�p�����[�^�ɕϊ�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 1:46:35
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool Font::stringToParam(const _TCHAR *param, SmartUtil::tstring &fontname, uint &height, uint32_t &style)
	{
		basic_stringstream<_TCHAR> ioss;
		ioss << param;
		TCSVRow csv;
		ioss >> csv;
		if(csv.size() != 3) return false;
		fontname = csv[0];
		height = boost::lexical_cast<ulong>(csv[1]);
		style = 0;
		if(string::npos!=csv[2].find(_T("BOLD"))) style |= BOLD;
		if(string::npos!=csv[2].find(_T("ITALIC"))) style |= ITALIC;
		if(string::npos!=csv[2].find(_T("UNDERLINE"))) style |= UNDERLINE;
		if(string::npos!=csv[2].find(_T("STRIKEOUT"))) style |= STRIKEOUT;
		if(string::npos!=csv[2].find(_T("FIXEDPITCH"))) style |= FIXEDPITCH;
		if(string::npos!=csv[2].find(_T("SHADOW"))) style |= SHADOW;
		if(string::npos!=csv[2].find(_T("OUTLINE"))) style |= OUTLINE;
		return true;
	}

} // namespace gctp
