#ifndef _GCTP_DB_HPP_
#define _GCTP_DB_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult リソースデータベースクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:26:10
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointer.hpp>
#include <gctp/tcstr.hpp>
#include <map>
#include <iosfwd>
#include <tchar.h> // VC限定だな…

namespace gctp {

	/** データベース
	 *
	 * 登録されたリソースの索引。
	 *
	 * キー値は任意の文字列が使えるが、すべて大文字のものは、システムに予約されている。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 17:47:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class DB {
	public:
		typedef std::map<TCStr, Hndl> Index;

		/// 追加
		bool insert(const _TCHAR *key, const Hndl hndl);

		/// 置き換え
		void set(const _TCHAR *key, Hndl);

		/// 削除
		void erase(const _TCHAR *key);

		/// 検索
		Hndl find(const _TCHAR *key) const;

		/// 検索
		Hndl operator[](const _TCHAR *key) const
		{
			return find(key);
		}
		
		Index::iterator begin()
		{
			return index_.begin();
		}
		Index::const_iterator begin() const
		{
			return index_.begin();
		}
		Index::iterator end()
		{
			return index_.end();
		}
		Index::const_iterator end() const
		{
			return index_.end();
		}

		Index::iterator findUpper(const _TCHAR *key)
		{
			return index_.upper_bound(key);
		}
		Index::const_iterator findUpper(const _TCHAR *key) const
		{
			return index_.upper_bound(key);
		}
		Index::iterator findLower(const _TCHAR *key)
		{
			return index_.lower_bound(key);
		}
		Index::const_iterator findLower(const _TCHAR *key) const
		{
			return index_.lower_bound(key);
		}

	private:
		Index index_;
	};

	// operator<<
	template<class E, class T> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, DB const & d)
	{
		for(DB::Index::const_iterator i = d.begin(); i != d.end(); ++i) {
			os << i->first << std::endl;
		}
		return os;
	}

} // namespace gctp

#endif //_GCTP_DB_HPP_