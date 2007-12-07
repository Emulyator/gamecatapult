#ifndef _GCTP_BUFFER_HPP_
#define _GCTP_BUFFER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �o�b�t�@�N���X
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

	/** �ėp�o�b�t�@�N���X
	 *
	 * ���������̂�std::string�Ŏ���
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 14:07:10
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Buffer : public Object {
	public:
		/// �o�b�t�@���m�ۂ����ɍ\�z
		Buffer() {}
		/// �o�b�t�@�̃T�C�Y���w�肵�č\�z
		Buffer(std::size_t size) { buf_.resize(size); }
		virtual void serialize(Serializer &);
		/// �o�b�t�@�T�C�Y��Ԃ�
		std::size_t size() const { return buf_.length(); }
		/// �o�b�t�@�T�C�Y��ύX
		void resize(std::size_t size) { buf_.resize(size); }
		/// �o�b�t�@���擾
		char *bytes()  { return const_cast<char *>(buf_.c_str()); }
		/// �o�b�t�@���擾
		const char *bytes() const { return buf_.c_str(); }
		/// �o�b�t�@���擾
		void *buf() { return bytes(); }
		/// �o�b�t�@���擾
		const void *buf() const { return bytes(); }
		/// �o�b�t�@���擾
		void *operator()() { return buf(); }
		/// �o�b�t�@���擾
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