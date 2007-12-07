#ifndef _GCTP_BUFFER_HPP_
#define _GCTP_BUFFER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult バッファクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:28:35
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/buffwd.hpp>
#include <gctp/class.hpp>
#include <iosfwd>
#include <string>

namespace gctp {

	class Serializer;

	/** 汎用バッファクラス
	 *
	 * メモリ実体はstd::stringで実装
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 14:07:10
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Buffer : public Object {
	public:
		/// バッファを確保せずに構築
		Buffer() {}
		/// バッファのサイズを指定して構築
		Buffer(std::size_t size) { buf_.resize(size); }
		virtual void serialize(Serializer &);
		/// バッファサイズを返す
		std::size_t size() const { return buf_.length(); }
		/// バッファサイズを変更
		void resize(std::size_t size) { buf_.resize(size); }
		/// バッファを取得
		char *bytes()  { return const_cast<char *>(buf_.c_str()); }
		/// バッファを取得
		const char *bytes() const { return buf_.c_str(); }
		/// バッファを取得
		void *buf() { return bytes(); }
		/// バッファを取得
		const void *buf() const { return bytes(); }
		/// バッファを取得
		void *operator()() { return buf(); }
		/// バッファを取得
		const void *operator()() const  { return buf(); }

	GCTP_DECLARE_CLASS
	private:
		std::string buf_;
	};

	template<class _Ch, class _Tr>
	std::basic_ostream<_Ch, _Tr> &operator<<(std::basic_ostream<_Ch, _Tr> &lhs, Buffer &rhs)
	{
		return lhs.write(rhs.buf(), static_cast<std::streamsize>(rhs.size()));
	}

} // namespace gctp

#endif //_GCTP_BUFFER_HPP_