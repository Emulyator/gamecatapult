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
#include <gctp/cstr.hpp>
#include <map>
#include <iosfwd>

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
		typedef std::map<CStr, Hndl> Index;

		/// 追加
		bool insert(const char *key, const Hndl hndl);

		/// 置き換え
		void set(const char *key, Hndl);

		/// 削除
		void erase(const char *key);

		/// 検索
		Hndl find(const char *key) const;

		/// 検索
		Hndl operator[](const char *key) const
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

		Index::iterator findUpper(const char *key)
		{
			return index_.upper_bound(key);
		}
		Index::const_iterator findUpper(const char *key) const
		{
			return index_.upper_bound(key);
		}
		Index::iterator findLower(const char *key)
		{
			return index_.lower_bound(key);
		}
		Index::const_iterator findLower(const char *key) const
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

	/// グローバルデータベースインスタンス
	DB &db();

	/** データベースに登録しつつ製作
	 *
	 * すでにデータベースにあるならそれを返し、そうで無かったら新規製作しＤＢに登録する、という
	 * 操作のテンプレート
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 14:03:54
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	Pointer<_T> createOnDB(const char *name)
	{
		Handle<_T> h = db()[name];
		if(h) return h.lock();
		else {
			Pointer<_T> ret = new _T;
			if(ret) {
				if(ret->setUp(name)) {
					PRNN("リソース"<<name<<"を制作");
					h = ret;
					db().insert(name, h);
					return ret;
				}
			}
			PRNN("リソース"<<name<<"の制作に失敗");
		}
		return 0;
	}

#if !defined(_MSC_VER) || (_MSC_VER > 1300)
	/** データベースに登録しつつ製作
	 *
	 * すでにデータベースにあるならそれを返し、そうで無かったら新規製作しＤＢに登録する、という
	 * 操作のテンプレート
	 *
	 * setUpメソッドを指定できる
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 14:03:54
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, bool (_T::*SetUpMethod)(const char *name)>
	Pointer<_T> createOnDB(const char *name)
	{
		Handle<_T> h = db()[name];
		if(h) return h.lock();
		else {
			Pointer<_T> ret = new _T;
			if(ret) {
				if((ret.get()->*SetUpMethod)(name)) {
					PRNN("リソース"<<name<<"を制作");
					h = ret;
					db().insert(name, h);
					return ret;
				}
			}
			PRNN("リソース"<<name<<"の制作に失敗");
		}
		return 0;
	}
#endif

	/** データベースに登録しつつ製作
	 *
	 * すでにデータベースにあるならそれを返し、そうで無かったら新規製作しＤＢに登録する、という
	 * 操作のテンプレート
	 *
	 * DB登録名とsetUpに渡す文字列を別々に指定できる
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 14:03:54
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	Pointer<_T> createOnDB(const char *name, const char *setup)
	{
		Handle<_T> h = db()[name];
		if(h) return h.lock();
		else {
			Pointer<_T> ret = new _T;
			if(ret) {
				if(ret->setUp(setup)) {
					PRNN("リソース"<<name<<"を制作");
					h = ret;
					db().insert(name, h);
					return ret;
				}
			}
			PRNN("リソース"<<name<<"の制作に失敗");
		}
		return 0;
	}

#if !defined(_MSC_VER) || (_MSC_VER > 1300)
	/** データベースに登録しつつ製作
	 *
	 * すでにデータベースにあるならそれを返し、そうで無かったら新規製作しＤＢに登録する、という
	 * 操作のテンプレート
	 *
	 * DB登録名とsetUpに渡す文字列を別々に指定できる
	 * 更にsetUpメソッドを指定する
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 14:03:54
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, bool (_T::*SetUpMethod)(const char *name)>
	Pointer<_T> createOnDB(const char *name, const char *setup)
	{
		Handle<_T> h = db()[name];
		if(h) return h.lock();
		else {
			Pointer<_T> ret = new _T;
			if(ret) {
				if((ret->*SetUpMethod)(setup)) {
					PRNN("リソース"<<name<<"を制作");
					h = ret;
					db().insert(name, h);
					return ret;
				}
			}
			PRNN("リソース"<<name<<"の制作に失敗");
		}
		return 0;
	}
#endif
} // namespace gctp

#endif //_GCTP_DB_HPP_