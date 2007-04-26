#ifndef _GCTP_TWSTRINGMAP_HPP_
#define _GCTP_TWSTRINGMAP_HPP_
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
#include <gctp/wstringmap.hpp>
#else
#include <gctp/stringmap.hpp>
#endif

namespace gctp {

#ifdef UNICODE
	template<class _T>
	class TStaticStringMap : public WStaticStringMap<_T> {};
	template<class _T>
	class TStringMap : public WStringMap<_T> {};
#else
	template<class _T>
	class TStaticStringMap : public StaticStringMap<_T> {};
	template<class _T>
	class TStringMap : public StringMap<_T> {};
#endif

} // namespace gctp

#endif //_GCTP_TWSTRINGMAP_HPP_
