#ifndef _GCTP_TARGMAP_HPP_
#define _GCTP_TARGMAP_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file
 *
 * ArgMap�N���X
 * �����ǂݎ��E�ێ��N���X
 *
 * ���C�h�����łƂ����łȂ��ł��AMS��_TCHAR�}�N�����ɐ؂�ւ���w�b�_
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 13:29:41
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#ifdef UNICODE
#include <gctp/wargmap.hpp>
#else
#include <gctp/argmap.hpp>
#endif

namespace gctp {

#ifdef UNICODE
	typedef WArgMap TArgMap;
#else
	typedef ArgMap ArgMap;
#endif

} // namespace gctp

#endif //_GCTP_TARGMAP_HPP_
