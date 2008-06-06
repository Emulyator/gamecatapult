#ifndef _GCTP_DBGOUT_HPP_
#define _GCTP_DBGOUT_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult デバッグ出力支援マクロ・関数ヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/10 7:59:30
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#ifndef GCTP_DBGOUT
#ifndef NDEBUG
#define GCTP_DBGOUT	1
#endif
#endif
#ifndef GCTP_DBGTRACE
#ifndef NDEBUG
#define GCTP_DBGTRACE	1
#endif
#endif

#include <tchar.h>
#include <fstream>
#include <ostream>

namespace gctp {

	/** 同期ストリームを挿入
	 *
	 * 以下のようにすることで、デバッグ出力をファイルに落とせる
	 * @code
	std::basic_ofstream<_TCHAR> g_logfilebuf;
	gctp::insertDbgoutSyncStream(&g_logfilebuf);
	g_logfilebuf.open("log.txt");
	 * @endcode
	 */
	void insertDbgoutSyncStream(std::basic_streambuf<_TCHAR> *sync_buf);
	/// 指定の同期ストリームを同期対象からはずす
	void eraseDbgoutSyncStream(std::basic_streambuf<_TCHAR> *sync_buf);

	/// デバッグ用出力ストリーム
	extern std::basic_ostream<_TCHAR> dbgout;

} // namespace gctp

//------------------------------------
// Debug Output Macros
//------------------------------------
#define GCTP_LOCATION	__FILE__<<_T("(")<<__LINE__<<_T(") : ")

#define GCTP_ERRORINFO(_S)	(gctp::dbgout<<_T("Error in ")<<GCTP_LOCATION<<_S<<std::endl)

#if GCTP_DBGOUT
# define PRN(_S)	(gctp::dbgout << _S)
# define PRNN(_S)	PRN(_S<<std::endl)
#else
# define PRN(_S)
# define PRNN(_S)
#endif

#if GCTP_DBGTRACE
# define GCTP_TRACE(_S)	PRNN(GCTP_LOCATION<<_S)
# define GCTP_TRACE_MSGBOX(_S)	(PRNN(GCTP_LOCATION<<_S), ::MessageBox(NULL, _S, 0, MB_OK))
#else
# define GCTP_TRACE(_S)
# define GCTP_TRACE_MSGBOX(_S)
#endif

#ifdef GCTP_DEBUG
#include <assert.h>
#define GCTP_ASSERT(x) assert(x)
#else
#define GCTP_ASSERT(x)
#endif

#endif //_GCTP_DBGOUT_HPP_