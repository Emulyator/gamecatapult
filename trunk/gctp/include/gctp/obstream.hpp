#ifndef _GCTP_OBSTREAM_HPP_
#define _GCTP_OBSTREAM_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapultバイナリストリーム出力フォーマット
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 23:09:26
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <ios>

namespace gctp {

	/** バイナリストリーム出力フォーマットクラス
	 *
	 * ostreamなどと比べて文字列の扱いが違う。読み取り時は改行までではなくヌル文字まで、
	 * 書き込み時はヌル文字も書き込む(ostreamはヌル文字は書かない)。
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class obstream : virtual public std::ios {
		typedef obstream				_Self;
		typedef std::char_traits<char>	_Traits;
	public:							// Types
		typedef char					char_type;
		typedef _Traits::int_type		int_type;
		typedef _Traits::pos_type		pos_type;
		typedef _Traits::off_type		off_type;
		typedef _Traits					traits_type;

	public:							// Constructor and destructor.
		explicit obstream(std::streambuf *__buf) { if(__buf) this->init(__buf); }
		~obstream() {}

	public:							// Hooks for manipulators.
		typedef std::ios& (*__ios_fn)(std::ios&);
		typedef std::ios_base& (*__ios_base_fn)(std::ios_base&);
		typedef _Self& (*__ostream_fn)(_Self&);
		_Self &operator<< (__ostream_fn __f) { return __f(*this); }
		_Self &operator<< (__ios_base_fn __f) { __f(*this); return *this; }
		_Self &operator<< (__ios_fn __ff) { __ff(*this); return *this; }

	public:							// Unformatted output.
		_Self& write(const void* __s, std::streamsize __n) {
			rdbuf()->sputn(reinterpret_cast<const char *>(__s), __n);
			return *this;
		}

	public:							// Formatted output.
		// Formatted output from a streambuf.
		_Self& operator<<(std::streambuf* __buf);
		_Self& operator<<(const char __x) { return write(&__x, sizeof(__x)); }
		_Self& operator<<(const unsigned char __x) { return write((void *)&__x, sizeof(__x)); }
		_Self& operator<<(const wchar_t __x) { return write(&__x, sizeof(__x)); }
		_Self& operator<<(const int __x) { return write((void *)&__x, sizeof(__x)); }
		_Self& operator<<(const unsigned int __x) { return write((void *)&__x, sizeof(__x)); }
		_Self& operator<<(const short __x) { return write((void *)&__x, sizeof(__x)); }
		_Self& operator<<(const unsigned short __x) { return write((void *)&__x, sizeof(__x)); }
		_Self& operator<<(const long __x) { return write((void *)&__x, sizeof(__x)); }
		_Self& operator<<(const unsigned long __x) { return write((void *)&__x, sizeof(__x)); }
		#ifdef _STLP_LONG_LONG
		_Self& operator<< (_STLP_LONG_LONG __x) { return write((void *)&__x, sizeof(__x)); }
		_Self& operator<< (unsigned _STLP_LONG_LONG __x) { return write(&__x, sizeof(__x)); }
		#endif 
		_Self& operator<<(const float __x)	{ return write(&__x, sizeof(__x)); }
		_Self& operator<<(const double __x)	{ return write(&__x, sizeof(__x)); }
		_Self& operator<<(const char *s)	{ return write(s, static_cast<std::streamsize>(strlen(s)))<<'\0'; }
		_Self& operator<<(const std::string &s) { return write(s.c_str(), static_cast<std::streamsize>(strlen(s.c_str())))<<'\0'; }
		_Self& operator<<(const wchar_t *s)	{ return write(s, static_cast<std::streamsize>(wcslen(s)*sizeof(wchar_t)))<<L'\0'; }
		_Self& operator<<(const std::wstring &s) { return write(s.c_str(), static_cast<std::streamsize>(wcslen(s.c_str())*sizeof(wchar_t)))<<L'\0'; }
		# ifndef _STLP_NO_LONG_DOUBLE
		_Self& operator<<(long double __x) { return write(&__x, sizeof(__x)); }
		# endif
		_Self& operator<<(const void* __x) { return write(&__x, sizeof(__x)); }
		# ifndef _STLP_NO_BOOL
		_Self& operator<<(bool __x) { return write(&__x, sizeof(__x)); }
		# endif

	public:                         // Buffer positioning and manipulation.
		_Self& flush() {
			if (this->rdbuf())
			if (this->rdbuf()->pubsync() == -1)
				this->setstate(ios_base::badbit);
			return *this;
		}

		pos_type tellp() {
			return this->rdbuf() && !this->fail()
				? this->rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::out)
				: pos_type(-1);
		}

		_Self& seekp(pos_type __pos) {
			if (this->rdbuf() && !this->fail())
				this->rdbuf()->pubseekpos(__pos, std::ios_base::out);
			return *this;
		}

		_Self& seekp(off_type __off, std::ios_base::seekdir __dir) {
			if (this->rdbuf() && !this->fail())
				this->rdbuf()->pubseekoff(__off, __dir, std::ios_base::out);
			return *this;
		}
	};

} // namespace gctp

#endif //_GCTP_OBSTREAM_HPP_