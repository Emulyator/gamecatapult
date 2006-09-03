#ifndef _GCTP_BBSTREAM_HPP_
#define _GCTP_BBSTREAM_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult バイナリバッファストリームクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 23:10:02
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <strstream>
#include <gctp/iobstream.hpp>

namespace gctp {

	/** 入力バイナリバッファストリームクラス
	 *
	 * ostreamなどと比べて文字列の扱いが違う。読み取り時は改行までではなくヌル文字まで、
	 * 書き込み時はヌル文字も書き込む(ostreamはヌル文字は書かない)。
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class ibbstream : public ibstream
	{
	public:                         // Constructors, destructor.
		ibbstream() : /*std::ios(),*/ ibstream(0), _M_buf() {
			this->init(&_M_buf);
		}
		ibbstream(void* __s, std::streamsize __n) 
		 : /*ios(),*/ ibstream(0), _M_buf(reinterpret_cast<char*>(__s), __n) {
			this->init(&_M_buf);
		}
		ibbstream(const void* __s, std::streamsize __n) 
		 : /*ios(),*/ ibstream(0), _M_buf(reinterpret_cast<const char*>(__s), __n) {
			this->init(&_M_buf);
		}
		~ibbstream() {}

	public:                         // File and buffer operations.
		std::strstreambuf* rdbuf() const { return const_cast<std::strstreambuf*>(&_M_buf); }

		void *buf() { return _M_buf.str(); }

	private:
		std::strstreambuf _M_buf;
	};

	/** 出力バイナリバッファストリームクラス
	 *
	 * ostreamなどと比べて文字列の扱いが違う。読み取り時は改行までではなくヌル文字まで、
	 * 書き込み時はヌル文字も書き込む(ostreamはヌル文字は書かない)。
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class obbstream : public obstream
	{
	public:                         // Constructors, destructor.
		obbstream() : /*std::ios(),*/ obstream(0), _M_buf() {
			this->init(&_M_buf);
		}
		obbstream(void* __s, std::streamsize __n, std::ios::openmode mode = std::ios::out)
		 : /*ios(),*/ obstream(0),
		  _M_buf(reinterpret_cast<char*>(__s), __n, mode & std::ios::app ? (reinterpret_cast<char*>(__s)) + strlen(reinterpret_cast<char*>(__s)) : reinterpret_cast<char*>(__s)) {
			this->init(&_M_buf);
		}
		~obbstream() {}

	public:                         // File and buffer operations.
		std::strstreambuf* rdbuf() const { return const_cast<std::strstreambuf*>(&_M_buf); }

		void freeze(bool frz = true) { _M_buf.freeze(frz); }

		void *buf() { return _M_buf.str(); }

		std::streamsize pcount() const { return _M_buf.pcount(); }

	private:
		std::strstreambuf _M_buf;
	};

	/** 入出力バイナリバッファストリームクラス
	 *
	 * ostreamなどと比べて文字列の扱いが違う。読み取り時は改行までではなくヌル文字まで、
	 * 書き込み時はヌル文字も書き込む(ostreamはヌル文字は書かない)。
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class bbstream : public iobstream
	{
		typedef std::char_traits<char>	_Traits;
	public:                         // Types
		typedef char					char_type;
		typedef	_Traits::int_type		int_type;
		typedef _Traits::pos_type		pos_type;
		typedef _Traits::off_type		off_type;
		typedef std::char_traits<char>	traits_type;

	public:                         // Constructors, destructor.
		bbstream() : /*std::ios(),*/ iobstream(0), _M_buf() {
			this->init(&_M_buf);
		}
		bbstream(void* __s, std::streamsize __n, std::ios::openmode mode = std::ios::out | std::ios::in)
		 : /*ios(),*/ iobstream(0),
		   _M_buf(reinterpret_cast<char*>(__s), __n, mode & std::ios::app ? (reinterpret_cast<char*>(__s)) + strlen(reinterpret_cast<char*>(__s)) : reinterpret_cast<char*>(__s)) {
			this->init(&_M_buf);
		}
		~bbstream() {}

	public:                         // File and buffer operations.
		std::strstreambuf* rdbuf() const { return const_cast<std::strstreambuf*>(&_M_buf); }

		void freeze(bool frz = true) { _M_buf.freeze(frz); }

		void *buf() { return _M_buf.str(); }

		std::streamsize pcount() const { return _M_buf.pcount(); }

	private:
		std::strstreambuf _M_buf;
	};

} // namespace gctp

#endif //_GCTP_BBSTREAM_HPP_