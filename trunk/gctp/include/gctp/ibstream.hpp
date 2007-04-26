#ifndef _GCTP_IBSTREAM_HPP_
#define _GCTP_IBSTREAM_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapultバイナリストリーム入力フォーマット
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 23:08:58
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <ios>

namespace gctp {

	/** バイナリストリーム入力フォーマットクラス
	 *
	 * ostreamなどと比べて文字列の扱いが違う。読み取り時は改行までではなくヌル文字まで、
	 * 書き込み時はヌル文字も書き込む(ostreamはヌル文字は書かない)。
	 * 
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/04 23:23:42
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class ibstream : virtual public std::ios {
		typedef ibstream				_Self;
		typedef std::char_traits<char>	_Traits;
	public:							// Types
		typedef char					char_type;
		typedef _Traits::int_type		int_type;
		typedef _Traits::pos_type		pos_type;
		typedef _Traits::off_type		off_type;
		typedef _Traits					traits_type;

	public:							// Constructor and destructor.
		explicit ibstream(std::streambuf *__buf) : _M_gcount(0) { if(__buf) this->init(__buf); }
		~ibstream() {}

	public:							// Hooks for manipulators.
		typedef std::ios& (*__ios_fn)(std::ios&);
		typedef std::ios_base& (*__ios_base_fn)(std::ios_base&);
		typedef _Self& (*__ostream_fn)(_Self&);
		_Self &operator>> (__ostream_fn __f) { return __f(*this); }
		_Self &operator>> (__ios_base_fn __f) { __f(*this); return *this; }
		_Self &operator>> (__ios_fn __ff) { __ff(*this); return *this; }

	public:							// Unformatted output.
		int_type get() {
			_M_gcount = 1;
			return rdbuf()->sbumpc();
		}
		_Self& read(void* __s, std::streamsize __n) {
			_M_gcount = rdbuf()->sgetn(reinterpret_cast<char *>(__s), __n);
			return *this;
		}
		std::streamsize gcount() const { return _M_gcount; }
	private:
		// for operator >> std::string
		char getchar() {
			char c;
			read(&c, sizeof(c));
			return c;
		}
		// for operator >> std::wstring
		wchar_t getwchar() {
			wchar_t c;
			read(&c, sizeof(c));
			return c;
		}
		std::streamsize _M_gcount;

	public:							// Formatted output.
		// Formatted output from a streambuf.
		_Self& operator>>(std::streambuf *__buf);
		_Self& operator>>(char &__x) { return read(&__x, sizeof(__x)); }
		_Self& operator>>(unsigned char &__x) { return read((void *)&__x, sizeof(__x)); }
		_Self& operator>>(int &__x) { return read((void *)&__x, sizeof(__x)); }
		_Self& operator>>(unsigned int &__x) { return read((void *)&__x, sizeof(__x)); }
		_Self& operator>>(short &__x) { return read((void *)&__x, sizeof(__x)); }
		_Self& operator>>(unsigned short &__x) { return read((void *)&__x, sizeof(__x)); }
		_Self& operator>>(long &__x) { return read((void *)&__x, sizeof(__x)); }
		_Self& operator>>(unsigned long &__x) { return read((void *)&__x, sizeof(__x)); }
		#ifdef _STLP_LONG_LONG
		_Self& operator>> (_STLP_LONG_LONG &__x) { return read((void *)&__x, sizeof(__x)); }
		_Self& operator>> (unsigned _STLP_LONG_LONG &__x) { return read(&__x, sizeof(__x)); }
		#endif 
		_Self& operator>>(float &__x)	{ return read(&__x, sizeof(__x)); }
		_Self& operator>>(double &__x)	{ return read(&__x, sizeof(__x)); }
		_Self& operator>>(std::string &s) {
			s = ""; for(char c = getchar(); good() && c != '\0'; c = getchar()) s += c;
			return *this;
		}
		_Self& operator>>(std::wstring &s) {
			s = L""; for(wchar_t c = getwchar(); good() && c != L'\0'; c = getwchar()) s += c;
			return *this;
		}
		# ifndef _STLP_NO_LONG_DOUBLE
		_Self& operator>>(long double &__x) { return read(&__x, sizeof(__x)); }
		# endif
		_Self& operator>>(void* __x) { return read(&__x, sizeof(__x)); }
		# ifndef _STLP_NO_BOOL
		_Self& operator>>(bool &__x) { return read(&__x, sizeof(__x)); }
		# endif

	public:                         // Buffer positioning and manipulation.
		_Self& sync() {
			if (this->rdbuf())
			if (this->rdbuf()->pubsync() == -1)
				this->setstate(ios_base::badbit);
			return *this;
		}

		pos_type tellg() {
			return this->rdbuf() && !this->fail()
				? this->rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::in)
				: pos_type(-1);
		}

		_Self& seekg(pos_type __pos) {
			if (this->rdbuf() && !this->fail())
				this->rdbuf()->pubseekpos(__pos, std::ios_base::in);
			return *this;
		}

		_Self& seekg(off_type __off, std::ios_base::seekdir __dir) {
			if (this->rdbuf() && !this->fail())
				this->rdbuf()->pubseekoff(__off, __dir, std::ios_base::in);
			return *this;
		}
	};

} // namespace gctp

#endif //_GCTP_IBSTREAM_HPP_