#ifndef _GCTP_CSTR_HPP_
#define _GCTP_CSTR_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �Œ蕶����N���X�w�b�_�t�@�C��
 *
 * �o�b�t�@�������ŕێ����Ȃ��A���邢�̓o�b�t�@�̐L�����s��Ȃ�������N���X�B
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 13:29:41
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <string>
#include <iosfwd>
#include <gctp/pointer.hpp>
#include <boost/utility.hpp>

// �Ƃ肠�����B�B�B
#pragma warning(disable : 4996)

namespace gctp {

	/** ���e����������N���X
	 *
	 * �o�b�t�@�������ŕێ����Ȃ�������N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 13:35:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class LStr {
	public:
		/// �f�t�H���g�R���X�g���N�^
		LStr() : p_(0) {}
		/// char *����ϊ�
		LStr(const char *src) : p_(src) {}
		/// std::string����ϊ�
		LStr(const std::string &src) : p_(src.c_str()) {}

		/// C�������Ԃ�
		const char *c_str() const { return p_; }
		/// C�������Ԃ�
		const char *data() const { return p_; }
		/// ������̒�����Ԃ�
		std::size_t length() const { if(p_) return strlen(p_); else return 0; }
		/// ������̒�����Ԃ�
		std::size_t size() const { if(p_) return strlen(p_); else return 0; }
		/// �k���|�C���^�܂��̓k���X�g�����O���H
		bool empty() const { if(p_) return *p_ == '\0'; else return true; }
		
		/// ��r
		bool operator==(const LStr &rhs) const { return strcmp(p_, rhs.p_) == 0; }
		/// ��r
		bool operator==(const char *rhs) const { return strcmp(p_, rhs) == 0; }
		/// ��r
		bool operator!=(const LStr &rhs) const { return strcmp(p_, rhs.p_) != 0; }
		/// ��r
		bool operator!=(const char *rhs) const { return strcmp(p_, rhs) != 0; }
		/// ��r
		bool operator<(const LStr &rhs) const { return strcmp(p_, rhs.p_) < 0; }
		/// ��r
		bool operator<(const char *rhs) const { return strcmp(p_, rhs) < 0; }
	protected:
		const char *p_;
	};

	template<class T>
	std::basic_ostream<char, T> & operator<< (std::basic_ostream<char, T> & os, LStr const & s)
	{
		return os << s.c_str();
	}

	namespace detail {

		/** �o�b�t�@�̐L�����s��Ȃ�������N���X
		 *
		 * strdup/malloc�ŗ̈�m�ۂ���͖̂�肩��
		 *
		 * Pointer�Ŏw����悤��Object�h����char *��ێ�������
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 14:07:10
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		class CStrImpl : public Object, public LStr {
		public:
			CStrImpl() : LStr(0), deletable_(false) {}
			CStrImpl(const wchar_t *src) : LStr(0), deletable_(false)
			{
				size_t s = wcstombs(0, src, 0);
				if(s >= 0 && s != (size_t)-1) {
					p_ = (const char *)malloc(s+1);
					if(p_) {
						size_t ws = wcstombs((char *)p_, src, s);
						if(ws >= 0 && ws != (size_t)-1) {
							((char *)p_)[s] = '\0';
							deletable_ = true;
						}
						else {
							free((void *)p_);
							p_ = 0;
						}
					}
				}
			}
			CStrImpl(const char *src) : LStr(strdup(src)), deletable_(true) {}
			CStrImpl(const std::string &src) : LStr(strdup(src.c_str())), deletable_(true) {}
			CStrImpl(const LStr &src) : LStr(strdup(src.c_str())), deletable_(true) {}
			CStrImpl(const CStrImpl &src) : LStr(strdup(src.p_)), deletable_(true) {}

			CStrImpl(const char *src, bool deletable) : LStr(src), deletable_(deletable) {}
			explicit CStrImpl(std::size_t size) : LStr((const char *)malloc(size)), deletable_(true) {}

			~CStrImpl() { clear(); }

			void clear() { if(p_ && deletable_) { free(const_cast<char *>(p_)); p_ = 0; } }
			bool deletable() { return deletable_; }

			void hold(const char *str) { clear(); deletable_ = true; p_ = str; }
			void justhold(const char *str) { clear(); deletable_ = false; p_ = str; }

			CStrImpl &operator=(const char *src) { clear(); p_ = strdup(src); return *this; }
			CStrImpl &operator=(const std::string &src) { clear(); p_ = strdup(src.c_str()); return *this; }
			CStrImpl &operator=(const LStr &src) { clear(); p_ = strdup(src.c_str()); return *this; }
			CStrImpl &operator=(const CStrImpl &src) { clear(); p_ = strdup(src.p_); return *this; }

		private:
			bool deletable_;
		};

	}

	/** ������萔�|�C���^�N���X
	 *
	 * ���L�\�Ȃ悤��gctp::Pointer�̔h���ł��邪�A�|�C���^�ł͂Ȃ����̂ł��邩�̂悤�ɐU����
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright &copy; 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/28 22:45:41
	 */
	class CStr : public Pointer<detail::CStrImpl> {
	public:
		/// �f�t�H���g�R���X�g���N�^
		CStr() {}
		/// wchar_t * ����ϊ�(MB->Unicode�ϊ�)
		CStr(const wchar_t *src) : Pointer<detail::CStrImpl>(src ? new detail::CStrImpl(src) : 0) {}
		/// char * ����ϊ�
		CStr(const char *src) : Pointer<detail::CStrImpl>(src ? new detail::CStrImpl(src) : 0) {}
		/// std::string ����ϊ�
		CStr(const std::string &src) : Pointer<detail::CStrImpl>(src.empty() ? 0 : new detail::CStrImpl(src)) {}
		/// LStr����ϊ�
		CStr(const LStr &src) : Pointer<detail::CStrImpl>(src.c_str() ? new detail::CStrImpl(src) : 0) {}
		/// �T�C�Y���w�肵�ăo�b�t�@�m��
		explicit CStr(std::size_t size) : Pointer<detail::CStrImpl>(size > 0 ? new detail::CStrImpl(size) : 0) {}

		CStr(Ptr const & rhs): Pointer<detail::CStrImpl>(rhs) {}

		CStr(Pointer<detail::CStrImpl> const & rhs): Pointer<detail::CStrImpl>(rhs) {}

		/// C�������Ԃ�
		const char *c_str() const { if(get()) return get()->c_str(); else return 0; }
		/// ������̒�����Ԃ�(�m�ۂ����o�b�t�@�̃T�C�Y�ł͂Ȃ��A���̏��strlen�Ōv��_�ɒ���)
		std::size_t length() const { if(get()) return get()->length(); else return 0; }
		/// ������̒�����Ԃ�
		std::size_t size() const { if(get()) return get()->size(); else return 0; }
		/// �o�b�t�@���Ȃ��A�������̓k���X�g�����O���H
		bool empty() const { if(get()) return get()->empty(); else return true; }

		/// �o�b�t�@�J��
		void clear() { release(); }

		/// ��r
		bool operator==(const CStr &rhs) const { return (**this == *rhs); }
		/// ��r
		bool operator==(const LStr &rhs) const { return (**this == rhs); }
		/// ��r
		bool operator==(const char *rhs) const { return (**this == rhs); }
		/// ��r
		bool operator!=(const CStr &rhs) const { return (**this != *rhs); }
		/// ��r
		bool operator!=(const LStr &rhs) const { return (**this != rhs); }
		/// ��r
		bool operator!=(const char *rhs) const { return (**this != rhs); }
		/// ��r
		bool operator<(const CStr &rhs) const { return (**this < *rhs); }
		/// ��r
		bool operator<(const LStr &rhs) const { return (**this < rhs); }
		/// ��r
		bool operator<(const char *rhs) const { return (**this < rhs); }

		/// ������̈�𕡐������A���̊J�����s��Ȃ�CStr��Ԃ�
		static CStr hold(const char *str) { return *new detail::CStrImpl(str, false); }
	};

	/** �e���|����������萔�|�C���^�N���X
	 *
	 * CStr���i�[�����R���e�i�ɁA��r�Ȃǂ̖ړI�ň�؎����ŗ̈�m�ۂ��Ȃ�CStr���~�����Ƃ��A������g���B
	 *
	 * ���̂��߁A�R���e�i�ɃR�s�[�����ƂԂ牺����Q�Ƃ���������\�������邽�߁A�����͐T�d��
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright &copy; 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/28 22:45:41
	 */
	class TempCStr : private boost::base_from_member<detail::CStrImpl>, public CStr {
		typedef boost::base_from_member<detail::CStrImpl> Dummy;
	public:
		TempCStr(const char *str) : Dummy(str, false), CStr(member)
		{
			member.setDeleter(NullDeleter::get());
		}
	};

	template<class T>
	std::basic_ostream<char, T> & operator<< (std::basic_ostream<char, T> & os, CStr const & s)
	{
		if(s.c_str()) return os << s.c_str();
		return os;
	}

	template<class T>
	std::basic_ostream<char, T> & operator>> (std::basic_ostream<char, T> & os, CStr const & s)
	{
		std::basic_string<char, T> w;
		os >> w; s = reinterpret_cast<const char*>(w.c_str());
		return os;
	}

} // namespace gctp

#endif //_GCTP_CSTR_HPP_