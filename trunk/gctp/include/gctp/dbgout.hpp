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
#define GCTP_DBGOUT	1
#endif
#ifndef GCTP_LOGFILE
#define GCTP_LOGFILE	1
#endif
#ifndef GCTP_DBGTRACE
#define GCTP_DBGTRACE	1
#endif

#if GCTP_LOGFILE
# include <fstream>
#endif
#if GCTP_DBGOUT
# include <ostream>
#endif

namespace gctp {

#if GCTP_LOGFILE
# ifdef UNICODE
	extern std::wofstream logfile;
# else
	extern std::ofstream logfile;
# endif
#endif

#if GCTP_DBGOUT
# ifdef UNICODE
	extern std::wostream dbgout;
# else
	extern std::ostream dbgout;
# endif
#endif

} // namespace gctp

//------------------------------------
// Debug Output Macros
//------------------------------------
#define LOCATION	__FILE__<<"("<<__LINE__<<") : "

#if GCTP_DBGOUT & GCTP_LOGFILE
# define PRN(_S)		(gctp::logfile<<_S, gctp::logfile.flush(), gctp::dbgout<<_S)
# define PRNN(_S)	PRN(_S<<std::endl)
# define LOG(_S)		(gctp::logfile << _S, gctp::logfile.flush())
# define LOGN(_S)	LOG(_S<<std::endl)
#elif GCTP_DBGOUT
# define PRN(_S)		(gctp::dbgout << _S)
# define PRNN(_S)	PRN(_S<<std::endl)
# define LOG(_S)
# define LOGN(_S)
#elif GCTP_LOGFILE
# define PRN(_S)
# define PRNN(_S)
# define LOG(_S)		(gctp::logfile<<_S, gctp::logfile.flushpp())
# define LOGN(_S)	LOG(_S<<std::endl)
#else
# define PRN(_S)
# define PRNN(_S)
# define LOG(_S)
# define LOGN(_S)
#endif

#if GCTP_DBGTRACE
# define GCTP_TRACE(_S)	PRN(LOCATION<<_S<<std::endl)
# define GCTP_TRACE_MSGBOX(_S)	(PRN(LOCATION<<_S<<std::endl), ::MessageBox(NULL, _S, 0, MB_OK))
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