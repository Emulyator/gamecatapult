#ifndef _GCTP_BFSTREAM_HPP_
#define _GCTP_BFSTREAM_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult バイナリファイルストリームクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 23:10:11
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/iobstream.hpp>
#include <fstream>

namespace gctp {

	/** 入力バイナリファイルストリームクラス
	 *
	 * ostreamなどと比べて文字列の扱いが違う。読み取り時は改行までではなくヌル文字まで、
	 * 書き込み時はヌル文字も書き込む(ostreamはヌル文字は書かない)。
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class ibfstream : public ibstream
	{
		typedef std::char_traits<char>	_Traits;
	public:                         // Types
		typedef char					char_type;
		typedef	_Traits::int_type		int_type;
		typedef _Traits::pos_type		pos_type;
		typedef _Traits::off_type		off_type;
		typedef std::char_traits<char>	traits_type;

	public:                         // Constructors, destructor.
		ibfstream() : /*std::ios(),*/ ibstream(0), _M_buf() {
			this->init(&_M_buf);
		}
		explicit ibfstream(const char* __s, std::ios_base::openmode __mod = std::ios_base::in) 
		 : /*ios(),*/ ibstream(0), _M_buf() {
			this->init(&_M_buf);
			if (!_M_buf.open(__s, __mod | std::ios_base::in | std::ios::binary))
			this->setstate(std::ios_base::failbit);
		}

#     if (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)) && (defined(_STLP_OWN_IOSTREAMS) || defined(__STL_OWN_IOSTREAMS))
		# ifndef _STLP_NO_EXTENSIONS
		explicit ibfstream(int __id, std::ios_base::openmode __mod = std::ios_base::in) 
		 : /*ios(),*/ ibstream(0), _M_buf() {
			this->init(&_M_buf);
			if (!_M_buf.open(__id, __mod | std::ios_base::in | std::ios::binary))
			this->setstate(std::ios_base::failbit);
		}
		ibfstream(const char* __s, std::ios_base::openmode __m, long __protection)
		 : /*ios(),*/ ibstream(0), _M_buf() {
			this->init(&_M_buf);
			if (!_M_buf.open(__s, __m | std::ios_base::in | std::ios::binary, __protection))
				this->setstate(std::ios_base::failbit);
		}
		# endif
#     endif

		~ibfstream() {}

	public:                         // File and buffer operations.
		std::filebuf* rdbuf() const { return const_cast<std::filebuf*>(&_M_buf); }

		bool is_open() {
			return this->rdbuf()->is_open();
		}

		void open(const char* __s, std::ios_base::openmode __mod= std::ios_base::in) {
			if (!this->rdbuf()->open(__s, __mod | std::ios_base::in | std::ios::binary))
				this->setstate(std::ios_base::failbit);
		}

		void close() {
			if (!this->rdbuf()->close())
				this->setstate(std::ios_base::failbit);
		}

	private:
		std::filebuf _M_buf;
	};

	/** 出力バイナリファイルストリームクラス
	 *
	 * ostreamなどと比べて文字列の扱いが違う。読み取り時は改行までではなくヌル文字まで、
	 * 書き込み時はヌル文字も書き込む(ostreamはヌル文字は書かない)。
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class obfstream : public obstream
	{
		typedef std::char_traits<char>	_Traits;
	public:                         // Types
		typedef char					char_type;
		typedef	/*typename*/ _Traits::int_type		int_type;
		typedef _Traits::pos_type		pos_type;
		typedef _Traits::off_type		off_type;
		typedef std::char_traits<char>	traits_type;

	public:                         // Constructors, destructor.
		obfstream() : /*std::ios(),*/ obstream(0), _M_buf() {
			this->init(&_M_buf);
		}
		explicit obfstream(const char* __s, std::ios_base::openmode __mod = std::ios_base::out) 
		 : /*ios(),*/ obstream(0), _M_buf() {
			this->init(&_M_buf);
			if (!_M_buf.open(__s, __mod | std::ios_base::out | std::ios::binary))
			this->setstate(std::ios_base::failbit);
		}

#     if (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)) && (defined(_STLP_OWN_IOSTREAMS) || defined(__STL_OWN_IOSTREAMS))
		# ifndef _STLP_NO_EXTENSIONS
		explicit obfstream(int __id, std::ios_base::openmode __mod = std::ios_base::out) 
		 : /*ios(),*/ obstream(0), _M_buf() {
			this->init(&_M_buf);
			if (!_M_buf.open(__id, __mod | std::ios_base::out | std::ios::binary))
			this->setstate(std::ios_base::failbit);
		}
		obfstream(const char* __s, std::ios_base::openmode __m, long __protection)
		 : /*ios(),*/ obstream(0), _M_buf() {
			this->init(&_M_buf);
			if (!_M_buf.open(__s, __m | std::ios_base::out | std::ios::binary, __protection))
				this->setstate(std::ios_base::failbit);
		}
		# endif
#endif

		~obfstream() {}

	public:                         // File and buffer operations.
		std::filebuf* rdbuf() const { return const_cast<std::filebuf*>(&_M_buf); }

		bool is_open() {
			return this->rdbuf()->is_open();
		}

		void open(const char* __s, std::ios_base::openmode __mod= std::ios_base::out) {
			if (!this->rdbuf()->open(__s, __mod | std::ios_base::out | std::ios::binary))
				this->setstate(std::ios_base::failbit);
		}

		void close() {
			if (!this->rdbuf()->close())
				this->setstate(std::ios_base::failbit);
		}

	private:
		std::filebuf _M_buf;
	};

	/** 入出力バイナリファイルストリームクラス
	 *
	 * ostreamなどと比べて文字列の扱いが違う。読み取り時は改行までではなくヌル文字まで、
	 * 書き込み時はヌル文字も書き込む(ostreamはヌル文字は書かない)。
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class bfstream : public iobstream
	{
		typedef std::char_traits<char>	_Traits;
	public:                         // Types
		typedef char					char_type;
		typedef	_Traits::int_type		int_type;
		typedef _Traits::pos_type		pos_type;
		typedef _Traits::off_type		off_type;
		typedef std::char_traits<char>	traits_type;

	public:                         // Constructors, destructor.
		bfstream() : /*std::ios(),*/ iobstream(0), _M_buf() {
			this->init(&_M_buf);
		}
		explicit bfstream(const char* __s, std::ios_base::openmode __mod = std::ios_base::in | std::ios_base::out) 
		 : /*ios(),*/ iobstream(0), _M_buf() {
			this->init(&_M_buf);
			if (!_M_buf.open(__s, __mod | std::ios_base::in | ios_base::out | std::ios::binary))
			this->setstate(std::ios_base::failbit);
		}

#     if (defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)) && (defined(_STLP_OWN_IOSTREAMS) || defined(__STL_OWN_IOSTREAMS))
		# ifndef _STLP_NO_EXTENSIONS
		 explicit bfstream(int __id, std::ios_base::openmode __mod = std::ios_base::in | std::ios_base::out) 
		 : /*ios(),*/ iobstream(0), _M_buf() {
			this->init(&_M_buf);
			if (!_M_buf.open(__id, __mod | std::ios_base::in | ios_base::out | std::ios::binary))
			this->setstate(std::ios_base::failbit);
		}
		bfstream(const char* __s, std::ios_base::openmode __m, long __protection)
		 : /*ios(),*/ iobstream(0), _M_buf() {
			this->init(&_M_buf);
			if (!_M_buf.open(__s, __m | std::ios_base::in | ios_base::out | std::ios::binary, __protection))
				this->setstate(std::ios_base::failbit);
		}
		# endif
#endif

		~bfstream() {}

	public:                         // File and buffer operations.
		std::filebuf* rdbuf() const { return const_cast<std::filebuf*>(&_M_buf); }

		bool is_open() {
			return this->rdbuf()->is_open();
		}

		void open(const char* __s, std::ios_base::openmode __mod= std::ios_base::in | ios_base::out) {
			if (!this->rdbuf()->open(__s, __mod | std::ios_base::in | ios_base::out | std::ios::binary))
				this->setstate(std::ios_base::failbit);
		}

		void close() {
			if (!this->rdbuf()->close())
				this->setstate(std::ios_base::failbit);
		}

	private:
		std::filebuf _M_buf;
	};

} // namespace gctp

#endif //_GCTP_BFSTREAM_HPP_