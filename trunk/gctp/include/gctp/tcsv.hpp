#ifndef _GCTP_TCSV_HPP_
#define _GCTP_TCSV_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �Œ蕶����N���X�w�b�_�t�@�C��
 *
 * �o�b�t�@�������ŕێ����Ȃ��A���邢�̓o�b�t�@�̐L�����s��Ȃ�������N���X�B���C�h������
 * 
 * ���C�h�����łƂ����łȂ��ł��AMS��_TCHAR�}�N�����ɐ؂�ւ���w�b�_
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 13:29:41
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/csv.hpp>

namespace gctp {

#ifdef UNICODE
	typedef WCSVRow TCSVRow;
	typedef WCSV TCSV;
#else
	typedef CSVRow TCSVRow;
	typedef CSV TCSV;
#endif

} // namespace gctp

#endif //_GCTP_TCSV_HPP_
