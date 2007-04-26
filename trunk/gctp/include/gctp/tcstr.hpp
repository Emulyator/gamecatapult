#ifndef _GCTP_TCSTR_HPP_
#define _GCTP_TCSTR_HPP_
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
#ifdef UNICODE
#include <gctp/wcstr.hpp>
#else
#include <gctp/cstr.hpp>
#endif

namespace gctp {

#ifdef UNICODE
	typedef WLStr TLStr;
	typedef WCStr TCStr;
	typedef TempWCStr TempTCStr;
#else
	typedef LStr TLStr;
	typedef CStr TCStr;
	typedef TempCStr TempTCStr;
#endif

} // namespace gctp

#endif //_GCTP_TCSTR_HPP_
