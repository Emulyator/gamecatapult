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
#ifndef GCTP_LOGFILE
#ifndef NDEBUG
#define GCTP_LOGFILE	1
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

	void insertDbgoutSyncStream(std::basic_streambuf<_TCHAR> *sync_buf);
	void eraseDbgoutSyncStream(std::basic_streambuf<_TCHAR> *sync_buf);
	extern std::basic_ostream<_TCHAR> dbgout;
	extern std::basic_ofstream<_TCHAR> logfile;

} // namespace gctp

//------------------------------------
// Debug Output Macros
//------------------------------------
#define GCTP_LOCATION	__FILE__<<_T("(")<<__LINE__<<_T(") : ")

#if GCTP_DBGOUT & GCTP_LOGFILE
# define GCTP_LOGFILE_OPEN(_S)	(gctp::logfile.open(_S))
# define GCTP_LOGFILE_CLOSE()	(gctp::logfile.close())
# define PRN(_S)	(gctp::logfile<<_S, gctp::dbgout<<_S)
# define PRNN(_S)	PRN(_S<<std::endl)
# define LOG(_S)	(gctp::logfile << _S)
# define LOGN(_S)	LOG(_S<<std::endl)
#elif GCTP_DBGOUT
# define GCTP_LOGFILE_OPEN(_S)
# define GCTP_LOGFILE_CLOSE()
# define PRN(_S)	(gctp::dbgout << _S)
# define PRNN(_S)	PRN(_S<<std::endl)
# define LOG(_S)
# define LOGN(_S)
#elif GCTP_LOGFILE
# define GCTP_LOGFILE_OPEN(_S)	(gctp::logfile.open(_S))
# define GCTP_LOGFILE_CLOSE()	(gctp::logfile.close())
# define PRN(_S)
# define PRNN(_S)
# define LOG(_S)	(gctp::logfile<<_S, gctp::logfile.flushpp())
# define LOGN(_S)	LOG(_S<<std::endl)
#else
# define GCTP_LOGFILE_OPEN(_S)
# define GCTP_LOGFILE_CLOSE()
# define PRN(_S)
# define PRNN(_S)
# define LOG(_S)
# define LOGN(_S)
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