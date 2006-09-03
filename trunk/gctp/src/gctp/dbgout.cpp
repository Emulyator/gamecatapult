/** @file
 * GameCatapult デバッグ出力支援マクロ・関数
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:09:55
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/dbgout.hpp>
#include <mbctype.h>

using namespace std;

namespace gctp {
	/// デバッガログストリームクラス
#ifdef UNICODE
	class debuggeroutbuf : public wstreambuf {
	public:
		debuggeroutbuf() : wstreambuf() {
			setbuf(__buf, sizeof(__buf)-1);
			__buf[sizeof(__buf)-1] = '\0';
		}
		~debuggeroutbuf() {
			if(pptr() != pbase()) sync();
		}
	protected:
		wstreambuf *setbuf(wchar_t *s, streamsize n) {
			if(s && n) {
				setp(s, s+n);
				return this;
			}
			return 0;
		}
		int sync() {
			output();
			setbuf(__buf, sizeof(__buf)-1);
			return wstreambuf::sync();
		}
		int overflow(int ch) {
			*epptr() = ch;
			*epptr() = '\0';
			sync();
			sputc(ch);
			return ch;
		}
		void output() {
			if(pptr() != epptr()) *pptr() = '\0';
			OutputDebugString(pbase());
		}
	private:
		wchar_t __buf[256];
	};
#else
	class debuggeroutbuf
		: public streambuf {
	public:
		debuggeroutbuf() : streambuf() {
			setbuf(__buf, sizeof(__buf)-1);
			__buf[sizeof(__buf)-1] = '\0';
		}
		~debuggeroutbuf() {
			if(pptr() != pbase()) sync();
		}
	protected:
		streambuf *setbuf(char *s, streamsize n) {
			if(s && n) {
				setp(s, s+n);
				return this;
			}
			return 0;
		}
		int sync() {
			output();
			setbuf(__buf, sizeof(__buf)-1);
			return streambuf::sync();
		}
		int overflow(int ch) {
			*epptr() = ch;
			// マルチバイト文字に対応
			if(-1 == _ismbstrail((const unsigned char *)pbase(), (const unsigned char *)epptr())) {
				char *lbp;
				for(lbp = epptr()-1; lbp > pbase(); lbp--) {
					if(-1 == _ismbslead((const unsigned char *)pbase(), (const unsigned char *)lbp)) {
						break;
					}
				}
				int _ch = *lbp; *lbp = '\0';
				sync();
				sputc(_ch);
				for(lbp++; lbp <= epptr(); lbp++) {
					sputc(*lbp);
				}
				*epptr() = '\0';
			}
			else {
				*epptr() = '\0';
				sync();
				sputc(ch);
			}
			return ch;
		}
		void output() {
			if(pptr() != epptr()) *pptr() = '\0';
			OutputDebugString(pbase());
		}
	private:
		char __buf[256];
	};
	//class_end
#endif

#ifdef UNICODE
	static debuggeroutbuf _dbgout_buf;
	wostream dbgout(&_dbgout_buf);		// デバッガアウトプットストリーム

# if GCTP_LOGFILE
	wofstream logfile("log.txt");		// ログファイルストリーム
# endif
#else
	static debuggeroutbuf _dbgout_buf;
	ostream dbgout(&_dbgout_buf);		// デバッガアウトプットストリーム

# if GCTP_LOGFILE
	ofstream logfile("log.txt");		// ログファイルストリーム
# endif
#endif

};	// namespace gctp
