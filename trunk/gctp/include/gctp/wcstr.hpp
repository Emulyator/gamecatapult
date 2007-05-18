#ifndef _GCTP_WCSTR_HPP_
#define _GCTP_WCSTR_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 固定文字列クラスヘッダファイル
 *
 * バッファを自分で保持しない、あるいはバッファの伸張を行わない文字列クラス。ワイド文字版
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
	class WLStr {
	public:
		/// デフォルトコンストラクタ
		WLStr() : p_(0) {}
		/// char *から変換
		WLStr(const wchar_t *src) : p_(src) {}
		/// std::stringから変換
		WLStr(const std::wstring &src) : p_(src.c_str()) {}

		/// C文字列を返す
		const wchar_t *c_str() const { return p_; }
		/// C文字列を返す
		const wchar_t *data() const { return p_; }
		/// 文字列の長さを返す
		std::size_t length() const { if(p_) return wcslen(p_); else return 0; }
		/// バッファのサイズを返す
		std::size_t size() const { if(p_) return wcslen(p_)*sizeof(wchar_t); else return 0; }
		/// ヌルポインタまたはヌルストリングか？
		bool empty() const { if(p_) return *p_ == L'\0'; else return true; }
		
		/// 比較
		bool operator==(const WLStr &rhs) const { return wcscmp(p_, rhs.p_) == 0; }
		/// 比較
		bool operator==(const wchar_t *rhs) const { return wcscmp(p_, rhs) == 0; }
		/// 比較
		bool operator!=(const WLStr &rhs) const { return wcscmp(p_, rhs.p_) != 0; }
		/// 比較
		bool operator!=(const wchar_t *rhs) const { return wcscmp(p_, rhs) != 0; }
		/// 比較
		bool operator<(const WLStr &rhs) const { return wcscmp(p_, rhs.p_) < 0; }
		/// 比較
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

		/** バッファの伸張を行わない文字列クラス
		 *
		 * strdup/mallocで領域確保するのは問題かも
		 *
		 * Pointerで指せるようにObject派生にchar *を保持させる
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

	/** 文字列定数ポインタクラス
	 *
	 * 共有可能なようにgctp::Pointerの派生であるが、ポインタではなく実体であるかのように振舞う
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright &copy; 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/28 22:45:41
	 */
	class WCStr : public Pointer<detail::WCStrImpl> {
	public:
		/// デフォルトコンストラクタ
		WCStr() {}
		/// char * から変換(MB->Unicode変換)
		WCStr(const char *src) : Pointer<detail::WCStrImpl>(src ? new detail::WCStrImpl(src) : 0) {}
		/// wchar_t * から変換
		WCStr(const wchar_t *src) : Pointer<detail::WCStrImpl>(src ? new detail::WCStrImpl(src) : 0) {}
		/// std::wstring から変換
		WCStr(const std::wstring &src) : Pointer<detail::WCStrImpl>(src.empty() ? 0 : new detail::WCStrImpl(src)) {}
		/// LStrから変換
		WCStr(const WLStr &src) : Pointer<detail::WCStrImpl>(src.c_str() ? new detail::WCStrImpl(src) : 0) {}
		/// サイズを指定してバッファ確保
		explicit WCStr(std::size_t size) : Pointer<detail::WCStrImpl>(size > 0 ? new detail::WCStrImpl(size) : 0) {}

		WCStr(detail::WCStrImpl * p, bool delete_guard = false) : Pointer<detail::WCStrImpl>(p)
		{
			if(delete_guard) addRef();
		}

		WCStr(Ptr const & rhs): Pointer<detail::WCStrImpl>(rhs) {}

		WCStr(Pointer<detail::WCStrImpl> const & rhs): Pointer<detail::WCStrImpl>(rhs) {}

		/// C文字列を返す
		const wchar_t *c_str() const { if(get()) return get()->c_str(); else return 0; }
		/// 文字列の長さを返す(確保したバッファのサイズではなく、その場でstrlenで計る点に注意)
		std::size_t length() const { if(get()) return get()->length(); else return 0; }
		/// 文字列の長さを返す
		std::size_t size() const { if(get()) return get()->size(); else return 0; }
		/// バッファがない、もしくはヌルストリングか？
		bool empty() const { if(get()) return get()->empty(); else return true; }

		/// バッファ開放
		void clear() { release(); }

		/// 比較
		bool operator==(const WCStr &rhs) const { return (**this == *rhs); }
		/// 比較
		bool operator==(const WLStr &rhs) const { return (**this == rhs); }
		/// 比較
		bool operator==(const wchar_t *rhs) const { return (**this == rhs); }
		/// 比較
		bool operator!=(const WCStr &rhs) const { return (**this != *rhs); }
		/// 比較
		bool operator!=(const WLStr &rhs) const { return (**this != rhs); }
		/// 比較
		bool operator!=(const wchar_t *rhs) const { return (**this != rhs); }
		/// 比較
		bool operator<(const WCStr &rhs) const { return (**this < *rhs); }
		/// 比較
		bool operator<(const WLStr &rhs) const { return (**this < rhs); }
		/// 比較
		bool operator<(const wchar_t *rhs) const { return (**this < rhs); }

		/// 文字列領域を複製せず、その開放も行わないCStrを返す
		static WCStr hold(const wchar_t *str) { return new detail::WCStrImpl(str, true); }
	};

	/** テンポラリ文字列定数ポインタクラス
	 *
	 * WCStrを格納したコンテナに、比較などの目的で一切自分で領域確保しないWCStrが欲しいとき、これを使う。
	 *
	 * そのため、コンテナにコピーされるとぶら下がり参照が発生する可能性があるため、扱いは慎重に
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
