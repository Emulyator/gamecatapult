#ifndef _GCTP_TURI_HPP_
#define _GCTP_TURI_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���\�[�X����̓N���X�w�b�_�t�@�C��
 *
 * ���C�h�����łƂ����łȂ��ł��AMS��_TCHAR�}�N�����ɐ؂�ւ���w�b�_
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:33:23
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#ifdef UNICODE
#include <gctp/wuri.hpp>
#else
#include <gctp/uri.hpp>
#endif

namespace gctp {

#ifdef UNICODE
	typedef WURI TURI;
#else
	typedef URI TURI;
#endif

} // namespace gctp

#endif //_GCTP_TURI_HPP_
