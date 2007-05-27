#ifndef _GCTP_CSTR_HPP_
#define _GCTP_CSTR_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 固定文字列クラスヘッダファイル
 *
 * バッファを自分で保持しない、あるいはバッファの伸張を行わない文字列クラス。
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 13:29:41
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <string>
#include <iosfwd>
#include <gctp/pointer.hpp>
#include <boost/utility.hpp>

// とりあえず。。。
#pragma warning(disable : 4996)

namespace gctp {

	/** リテラル文字列クラス
	 *
	 * バッファを自分で保持しない文字列クラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 13:35:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class LStr {
	public:
		/// デフォルトコンストラクタ
		LStr() : p_(0) {}
		/// char *から変換
		LStr(const char *src) : p_(src) {}
		/// std::stringから変換
		LStr(const std::string &src) : p_(src.c_str()) {}

		/// C文字列を返す
		const char *c_str() const { return p_; }
		/// C文字列を返す
		const char *data() const { return p_; }
		/// 文字列の長さを返す
		std::size_t length() const { if(p_) return strlen(p_); else return 0; }
		/// 文字列の長さを返す
		std::size_t size() const { if(p_) return strlen(p_); else return 0; }
		/// ヌルポインタまたはヌルストリングか？
		bool empty() const { if(p_) return *p_ == '\0'; else return true; }
		
		/// 比較
		bool operator==(const LStr &rhs) const { return strcmp(p_, rhs.p_) == 0; }
		/// 比較
		bool operator==(const char *rhs) const { return strcmp(p_, rhs) == 0; }
		/// 比較
		bool operator!=(const LStr &rhs) const { return strcmp(p_, rhs.p_) != 0; }
		/// 比較
		bool operator!=(const char *rhs) const { return strcmp(p_, rhs) != 0; }
		/// 比較
		bool operator<(const LStr &rhs) const { return strcmp(p_, rhs.p_) < 0; }
		/// 比較
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

		/** バッファの伸張を行わない文字列クラス
		 *
		 * strdup/mallocで領域確保するのは問題かも
		 *
		 * Pointerで指せるようにObject派生にchar *を保持させる
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

	/** 文字列定数ポインタクラス
	 *
	 * 共有可能なようにgctp::Pointerの派生であるが、ポインタではなく実体であるかのように振舞う
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright &copy; 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/28 22:45:41
	 */
	class CStr : public Pointer<detail::CStrImpl> {
	public:
		/// デフォルトコンストラクタ
		CStr() {}
		/// wchar_t * から変換(MB->Unicode変換)
		CStr(const wchar_t *src) : Pointer<detail::CStrImpl>(src ? new detail::CStrImpl(src) : 0) {}
		/// char * から変換
		CStr(const char *src) : Pointer<detail::CStrImpl>(src ? new detail::CStrImpl(src) : 0) {}
		/// std::string から変換
		CStr(const std::string &src) : Pointer<detail::CStrImpl>(src.empty() ? 0 : new detail::CStrImpl(src)) {}
		/// LStrから変換
		CStr(const LStr &src) : Pointer<detail::CStrImpl>(src.c_str() ? new detail::CStrImpl(src) : 0) {}
		/// サイズを指定してバッファ確保
		explicit CStr(std::size_t size) : Pointer<detail::CStrImpl>(size > 0 ? new detail::CStrImpl(size) : 0) {}

		CStr(Ptr const & rhs): Pointer<detail::CStrImpl>(rhs) {}

		CStr(Pointer<detail::CStrImpl> const & rhs): Pointer<detail::CStrImpl>(rhs) {}

		/// C文字列を返す
		const char *c_str() const { if(get()) return get()->c_str(); else return 0; }
		/// 文字列の長さを返す(確保したバッファのサイズではなく、その場でstrlenで計る点に注意)
		std::size_t length() const { if(get()) return get()->length(); else return 0; }
		/// 文字列の長さを返す
		std::size_t size() const { if(get()) return get()->size(); else return 0; }
		/// バッファがない、もしくはヌルストリングか？
		bool empty() const { if(get()) return get()->empty(); else return true; }

		/// バッファ開放
		void clear() { release(); }

		/// 比較
		bool operator==(const CStr &rhs) const { return (**this == *rhs); }
		/// 比較
		bool operator==(const LStr &rhs) const { return (**this == rhs); }
		/// 比較
		bool operator==(const char *rhs) const { return (**this == rhs); }
		/// 比較
		bool operator!=(const CStr &rhs) const { return (**this != *rhs); }
		/// 比較
		bool operator!=(const LStr &rhs) const { return (**this != rhs); }
		/// 比較
		bool operator!=(const char *rhs) const { return (**this != rhs); }
		/// 比較
		bool operator<(const CStr &rhs) const { return (**this < *rhs); }
		/// 比較
		bool operator<(const LStr &rhs) const { return (**this < rhs); }
		/// 比較
		bool operator<(const char *rhs) const { return (**this < rhs); }

		/// 文字列領域を複製せず、その開放も行わないCStrを返す
		static CStr hold(const char *str) { return *new detail::CStrImpl(str, false); }
	};

	/** テンポラリ文字列定数ポインタクラス
	 *
	 * CStrを格納したコンテナに、比較などの目的で一切自分で領域確保しないCStrが欲しいとき、これを使う。
	 *
	 * そのため、コンテナにコピーされるとぶら下がり参照が発生する可能性があるため、扱いは慎重に
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