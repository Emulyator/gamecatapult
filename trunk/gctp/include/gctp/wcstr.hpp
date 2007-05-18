#ifndef _GCTP_WCSTR_HPP_
#define _GCTP_WCSTR_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �Œ蕶����N���X�w�b�_�t�@�C��
 *
 * �o�b�t�@�������ŕێ����Ȃ��A���邢�̓o�b�t�@�̐L�����s��Ȃ�������N���X�B���C�h������
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
	class WLStr {
	public:
		/// �f�t�H���g�R���X�g���N�^
		WLStr() : p_(0) {}
		/// char *����ϊ�
		WLStr(const wchar_t *src) : p_(src) {}
		/// std::string����ϊ�
		WLStr(const std::wstring &src) : p_(src.c_str()) {}

		/// C�������Ԃ�
		const wchar_t *c_str() const { return p_; }
		/// C�������Ԃ�
		const wchar_t *data() const { return p_; }
		/// ������̒�����Ԃ�
		std::size_t length() const { if(p_) return wcslen(p_); else return 0; }
		/// �o�b�t�@�̃T�C�Y��Ԃ�
		std::size_t size() const { if(p_) return wcslen(p_)*sizeof(wchar_t); else return 0; }
		/// �k���|�C���^�܂��̓k���X�g�����O���H
		bool empty() const { if(p_) return *p_ == L'\0'; else return true; }
		
		/// ��r
		bool operator==(const WLStr &rhs) const { return wcscmp(p_, rhs.p_) == 0; }
		/// ��r
		bool operator==(const wchar_t *rhs) const { return wcscmp(p_, rhs) == 0; }
		/// ��r
		bool operator!=(const WLStr &rhs) const { return wcscmp(p_, rhs.p_) != 0; }
		/// ��r
		bool operator!=(const wchar_t *rhs) const { return wcscmp(p_, rhs) != 0; }
		/// ��r
		bool operator<(const WLStr &rhs) const { return wcscmp(p_, rhs.p_) < 0; }
		/// ��r
		bool operator<(const wchar_t *rhs) const { return wcscmp(p_, rhs) < 0; }
	protected:
		const wchar_t *p_;
	};

	template<class T>
	std::basic_ostream<wchar_t, T> & operator<< (std::basic_ostream<wchar_t, T> & os, WLStr const & s)
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
		class WCStrImpl : public Object, public WLStr {
		public:
			WCStrImpl() : WLStr(0), deletable_(false) {}
			WCStrImpl(const char *src) : WLStr(0), deletable_(false)
			{
				size_t s = strlen(src);
				size_t ws = mbstowcs(0, src, s);
				if(ws >= 0 && ws != (size_t)-1) {
					p_ = (const wchar_t *)malloc(sizeof(wchar_t)*(ws+1));
					if(p_) {
						size_t ts = mbstowcs((wchar_t *)p_, src, s);
						if(ts >= 0 && ts != (size_t)-1) {
							((wchar_t *)p_)[ws] = L'\0';
							deletable_ = true;
						}
						else {
							free((void *)p_);
							p_ = 0;
						}
					}
				}
			}
			WCStrImpl(const wchar_t *src) : WLStr(wcsdup(src)), deletable_(true) {}
			WCStrImpl(const std::wstring &src) : WLStr(wcsdup(src.c_str())), deletable_(true) {}
			WCStrImpl(const WLStr &src) : WLStr(wcsdup(src.c_str())), deletable_(true) {}
			WCStrImpl(const WCStrImpl &src) : WLStr(wcsdup(src.p_)), deletable_(true) {}

			WCStrImpl(const wchar_t *src, bool deletable) : WLStr(src), deletable_(deletable) {}
			explicit WCStrImpl(std::size_t size) : WLStr((const wchar_t *)malloc(size)), deletable_(true) {}

			~WCStrImpl() { clear(); }

			void clear() { if(p_ && deletable_) { free(const_cast<wchar_t *>(p_)); p_ = 0; } }
			bool deletable() { return deletable_; }

			void hold(const wchar_t *str) { clear(); deletable_ = true; p_ = str; }
			void justhold(const wchar_t *str) { clear(); deletable_ = false; p_ = str; }

			WCStrImpl &operator=(const wchar_t *src) { clear(); p_ = wcsdup(src); return *this; }
			WCStrImpl &operator=(const std::wstring &src) { clear(); p_ = wcsdup(src.c_str()); return *this; }
			WCStrImpl &operator=(const WLStr &src) { clear(); p_ = wcsdup(src.c_str()); return *this; }
			WCStrImpl &operator=(const WCStrImpl &src) { clear(); p_ = wcsdup(src.p_); return *this; }
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
	class WCStr : public Pointer<detail::WCStrImpl> {
	public:
		/// �f�t�H���g�R���X�g���N�^
		WCStr() {}
		/// char * ����ϊ�(MB->Unicode�ϊ�)
		WCStr(const char *src) : Pointer<detail::WCStrImpl>(src ? new detail::WCStrImpl(src) : 0) {}
		/// wchar_t * ����ϊ�
		WCStr(const wchar_t *src) : Pointer<detail::WCStrImpl>(src ? new detail::WCStrImpl(src) : 0) {}
		/// std::wstring ����ϊ�
		WCStr(const std::wstring &src) : Pointer<detail::WCStrImpl>(src.empty() ? 0 : new detail::WCStrImpl(src)) {}
		/// LStr����ϊ�
		WCStr(const WLStr &src) : Pointer<detail::WCStrImpl>(src.c_str() ? new detail::WCStrImpl(src) : 0) {}
		/// �T�C�Y���w�肵�ăo�b�t�@�m��
		explicit WCStr(std::size_t size) : Pointer<detail::WCStrImpl>(size > 0 ? new detail::WCStrImpl(size) : 0) {}

		WCStr(detail::WCStrImpl * p, bool delete_guard = false) : Pointer<detail::WCStrImpl>(p)
		{
			if(delete_guard) addRef();
		}

		WCStr(Ptr const & rhs): Pointer<detail::WCStrImpl>(rhs) {}

		WCStr(Pointer<detail::WCStrImpl> const & rhs): Pointer<detail::WCStrImpl>(rhs) {}

		/// C�������Ԃ�
		const wchar_t *c_str() const { if(get()) return get()->c_str(); else return 0; }
		/// ������̒�����Ԃ�(�m�ۂ����o�b�t�@�̃T�C�Y�ł͂Ȃ��A���̏��strlen�Ōv��_�ɒ���)
		std::size_t length() const { if(get()) return get()->length(); else return 0; }
		/// ������̒�����Ԃ�
		std::size_t size() const { if(get()) return get()->size(); else return 0; }
		/// �o�b�t�@���Ȃ��A�������̓k���X�g�����O���H
		bool empty() const { if(get()) return get()->empty(); else return true; }

		/// �o�b�t�@�J��
		void clear() { release(); }

		/// ��r
		bool operator==(const WCStr &rhs) const { return (**this == *rhs); }
		/// ��r
		bool operator==(const WLStr &rhs) const { return (**this == rhs); }
		/// ��r
		bool operator==(const wchar_t *rhs) const { return (**this == rhs); }
		/// ��r
		bool operator!=(const WCStr &rhs) const { return (**this != *rhs); }
		/// ��r
		bool operator!=(const WLStr &rhs) const { return (**this != rhs); }
		/// ��r
		bool operator!=(const wchar_t *rhs) const { return (**this != rhs); }
		/// ��r
		bool operator<(const WCStr &rhs) const { return (**this < *rhs); }
		/// ��r
		bool operator<(const WLStr &rhs) const { return (**this < rhs); }
		/// ��r
		bool operator<(const wchar_t *rhs) const { return (**this < rhs); }

		/// ������̈�𕡐������A���̊J�����s��Ȃ�CStr��Ԃ�
		static WCStr hold(const wchar_t *str) { return new detail::WCStrImpl(str, true); }
	};

	/** �e���|����������萔�|�C���^�N���X
	 *
	 * WCStr���i�[�����R���e�i�ɁA��r�Ȃǂ̖ړI�ň�؎����ŗ̈�m�ۂ��Ȃ�WCStr���~�����Ƃ��A������g���B
	 *
	 * ���̂��߁A�R���e�i�ɃR�s�[�����ƂԂ牺����Q�Ƃ���������\�������邽�߁A�����͐T�d��
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright &copy; 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/28 22:45:41
	 */
	class TempWCStr : private boost::base_from_member<detail::WCStrImpl>, public WCStr {
		typedef boost::base_from_member<detail::WCStrImpl> Dummy;
	public:
		TempWCStr(const wchar_t *str) : Dummy(str, false), WCStr(&member, true)
		{
		}
	};

	template<class T>
	std::basic_ostream<wchar_t, T> & operator<< (std::basic_ostream<wchar_t, T> & os, WCStr const & s)
	{
		if(s.c_str()) return os << s.c_str();
		return os;
	}

	template<class T>
	std::basic_ostream<wchar_t, T> & operator>> (std::basic_ostream<wchar_t, T> & os, WCStr const & s)
	{
		std::basic_string<wchar_t, T> w;
		os >> w; s = reinterpret_cast<const char*>(w.c_str());
		return os;
	}

} // namespace gctp

#endif //_GCTP_WCSTR_HPP_
