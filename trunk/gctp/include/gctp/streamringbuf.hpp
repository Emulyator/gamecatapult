#ifndef _GCTP_STREAMRINGBUF_HPP_
#define _GCTP_STREAMRINGBUF_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ストリームリングバッファ
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/10 7:59:30
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <streambuf>
#include <gctp/staticringbuffer.hpp>

namespace gctp {

	/// リングストリームクラス
	template<int _Size, typename _CharType, class _Traits = std::char_traits<_CharType> >
	class StreamRingBuf : public std::basic_streambuf<_CharType, _Traits> {
	public:
		enum {
			BUF_LEN = _Size
		};
		StreamRingBuf() : std::basic_streambuf<_CharType, _Traits>(), total_size_(0)
		{
		}

		StaticRingBuffer<_CharType, BUF_LEN> buffer;

		std::size_t totalSize() { return total_size_; }

	protected:
		virtual int_type overflow(int_type ch)
		{
			if(ch == traits_type::eof()) return traits_type::eof();
			buffer.push((_CharType)ch);
			total_size_++;
			return traits_type::not_eof(ch);
		}
	private:
		std::size_t total_size_;
	};

} // namespace gctp

#endif //_GCTP_STREAMRINGBUF_HPP_