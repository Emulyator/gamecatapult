#ifndef _GCTP_URI_HPP_
#define _GCTP_URI_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���\�[�X����̓N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:33:23
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <string>
#include <iosfwd>

namespace gctp {

	/** GameCatapult ���\�[�X���N���X
	 *
	 * ���\�[�X���i�ꉞURI���ɋL�q�j�̉�̓��[�e�B���e�B�[�B
	 * �g���q�����𔲂��o������A�p�X�ƃ��[�t�ɕ���������B
	 *
	 * �}���`�o�C�g�̔����isleadbyte���g���Ă���̂ŁAsetlocale���Ă����K�v����B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:20:37
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class URI {
	public:
		URI(const char *rn) : rn_(rn) { convertBackSlashToSlash(); }
		URI(const std::string &rn) : rn_(rn) { convertBackSlashToSlash(); }

		/** �X�L�[���𒊏o
		 *
		 * c:/xxx/yyy/zzz.ee.ext ��������Ac ��
		 * http://xxx/yyy/zzz.ext ��������Ahttp ��
		 * �A���Ă���B
		 */
		inline std::string scheme() const
		{
			std::string::size_type end = rn_.find_first_of(':');
			if(end != std::string::npos) return rn_.substr(0, end);
			return "";
		}

		/** �p�X�𒊏o
		 *
		 * c:/xxx/yyy/zzz.ee.ext ��������Ac:/xxx/yyy ��
		 * http://xxx/yyy/zzz.ext ��������Ahttp://xxx/yyy ��
		 * �A���Ă���B
		 */
		inline std::string path() const
		{
			std::string::size_type end = rn_.find_last_of('/');
			if(end != std::string::npos) return rn_.substr(0, end);
			return "";
		}

		/** �f�B���N�g���𒊏o
		 *
		 * c:/xxx/yyy/zzz.ee.ext ��������A/xxx/yyy ��
		 * http://xxx/yyy/zzz.ext ��������A//xxx/yyy ��
		 * �A���Ă���B
		 */
		inline std::string directory() const
		{
			std::string::size_type begin = rn_.find_first_of(':');
			if(begin == std::string::npos) begin = 0;
			else begin++;
			std::string::size_type end = rn_.find_last_of('/');
			if(end != std::string::npos) return rn_.substr(begin, end-begin);
			return "";
		}

		/** ���[�t�𒊏o
		 *
		 * c:/xxx/yyy/zzz.ee.ext��������Azzz.ee.ext ��
		 * http://xxx/yyy/zzz.ext ��������Azzz.ext ��
		 * �A���Ă���B
		 */
		inline std::string leaf() const
		{
			std::string::size_type begin = rn_.find_last_of('/');
			if(begin == std::string::npos) {
				begin = rn_.find_first_of(':');
				if(begin == std::string::npos) return rn_;
				begin++;
			}
			else begin++;
			return rn_.substr(begin);
		}

		/** �x�[�X���𒊏o
		 *
		 * c:/xxx/yyy/zzz.ee.ext ��������Azzz ��
		 * http://xxx/yyy/zzz.ext ��������Azzz ��
		 * �A���Ă���B
		 */
		inline std::string basename() const
		{
			std::string::size_type begin = rn_.find_last_of('/');
			if(begin == std::string::npos) {
				begin = rn_.find_first_of(':');
				if(begin == std::string::npos) begin = 0;
				else begin++;
			}
			else begin++;

			std::string::size_type end = rn_.find_first_of('.', begin);
			if(begin != std::string::npos) {
				if(end != std::string::npos) return rn_.substr(begin, end-begin);
				else return rn_.substr(begin);
			}
			return "";
		}

		/** �g���q�𒊏o
		 *
		 * c:/xxx/yyy/zzz.ee.ext ��������Aee.ext ��
		 * http://xxx/yyy/zzz.ext ��������Aext ��
		 * �A���Ă���B
		 */
		inline std::string extension() const
		{
			std::string::size_type lower = rn_.find_last_of('/');
			std::string::size_type begin = rn_.find_last_of('.');
			while(begin != std::string::npos && begin > 0) {
				std::string::size_type _next = rn_.find_last_of('.', begin-1);
				if(_next == std::string::npos || (lower != std::string::npos && _next<lower)) return rn_.substr(begin+1);
				begin = _next;
			}
			return "";
		}

		/// ���X�g�����O��Ԃ�
		inline const std::string &raw() const
		{
			return rn_;
		}

		/// ���ׂĂ��������ɕϊ�
		inline void convertLower()
		{
			for(std::string::size_type i = 0; i < rn_.size(); i++) {
				if(isleadbyte(rn_[i])) i++;
				else rn_[i] = tolower(rn_[i]);
			}
		}

	private:
		void convertBackSlashToSlash()
		{
			for(std::string::size_type i = 0; i < rn_.size(); i++) {
				if(isleadbyte(rn_[i])) i++;
				else if(rn_[i] == '\\') rn_[i] = '/';
			}
		}
		std::string rn_;
	};
	
	template<class E, class T>
	std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, URI const & uri)
	{
		return os << uri.raw();
	}

} //namespace gctp

#endif //_GCTP_URI_HPP_