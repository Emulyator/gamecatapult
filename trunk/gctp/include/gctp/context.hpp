#ifndef _GCTP_CONTEXT_HPP_
#define _GCTP_CONTEXT_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult リソースコンテキストクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointerlist.hpp>
#include <gctp/extension.hpp>
#include <gctp/db.hpp>
#include <gctp/class.hpp>
#include <gctp/tuki.hpp>
#include <tchar.h>

namespace gctp {

	class Serializer;

	/** リソースコンテキストクラス
	 *
	 * そのコンテキストでロードしたリソースを保持し、解体時に保持していたリソースもいっしょに
	 * 解放する。
	 *
	 * カレントコンテキストを、スタック上に保持する機構ももつ
	 *
	 * データベースへの登録も管理し、要求リソースソース名からのコンストラクタ解決、
	 * リソースソース名からリソース名への変換、ＤＢのガベージ、を担当する。
	 * @see Extention
	 * 
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:18:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Context : public Object {
	public:
		/** コンストラクタ
		 *
		 * 同時にカレントに設定。（コンテキストスタックに自分をプッシュ）
		 *
		 * 最も最近に生成もしくは操作されたコンテキストがカレントになる
		 */
		Context(bool open = true);

		/** デストラクタ
		 *
		 * 同時にコンテキストスタックをポップ
		 *
		 * コンテキストは生成された順に開放されなければならない。（そうしないと
		 * フラグメントを起こすから）カレントが自分で無かった場合、エラーメッセージを送出する
		 *
		 * デストラクタ内なので、例外は投げない
		 */
		~Context();

		/// コンテキストを開く
		void open();
		/// コンテキストを閉じる
		void close();

		/** コンテンツロード
		 *
		 * @return 読み込んで構築済みのオブジェクト。失敗した場合はヌルハンドルが返る。
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl load(const _TCHAR *name);

		/** ロード要求
		 *
		 * @return リクエストが成功したか？
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		bool loadAsync(const _TCHAR *name);

		/** オブジェクト登録
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl add(const Ptr ptr, const _TCHAR *name = 0);

		/// オブジェクト登録
		template<class T>
		Handle<T> create(const _TCHAR *name = 0)
		{
			if(name) {
				Hndl h = find(name);
				if(h) return h;
			}
			Pointer<T> p = new T;
			if(p) return add(p, name);
			return 0;
		}
		/// オブジェクト登録
		template<class T>
		Handle<T> createAndSetUp(const _TCHAR *name = 0)
		{
			if(name) {
				Hndl h = find(name);
				if(h) return h;
			}
			Pointer<T> p = new T;
			if(p) {
				if(p->setUp()) return add(p, name);
			}
			return 0;
		}
		/// オブジェクト登録
		template<class T>
		Handle<T> createAndSetUpEx(const _TCHAR *name = 0)
		{
			if(name) {
				Hndl h = find(name);
				if(h) return h;
			}
			Pointer<T> p = new T;
			if(p) {
				if(p->setUp(name)) return add(p, name);
			}
			return 0;
		}

		/** オブジェクト製作
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl create(const GCTP_TYPEINFO &typeinfo, const _TCHAR *name = 0);
		
		/** オブジェクト製作
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl create(const char *classname, const _TCHAR *name = 0);

		/** オブジェクト検索
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl find(const _TCHAR *name);

		/// オブジェクト検索
		Hndl operator[](const _TCHAR *name)
		{
			return find(name);
		}

		/// シーケンスの先頭を返す
		PtrList::iterator begin()
		{
			return ptrs_.begin();
		}
		/// シーケンスの先頭を返す
		PtrList::const_iterator begin() const
		{
			return ptrs_.begin();
		}
		/// シーケンスの最後を返す
		PtrList::iterator end()
		{
			return ptrs_.end();
		}
		/// シーケンスの最後を返す
		PtrList::const_iterator end() const
		{
			return ptrs_.end();
		}

		virtual void serialize(Serializer &);

	protected:
		// luapp
		bool setUp(luapp::Stack &L);
		int load(luapp::Stack &L);
		int create(luapp::Stack &L);
		int find(luapp::Stack &L);
		int pairs(luapp::Stack &L);
		int ipairs(luapp::Stack &L);
		static int current(luapp::Stack &L);

		friend Context &context();

	private:
		bool is_open_;
		PtrList ptrs_;
		DB db_;
		Context *prev_;

		static Context *current_;

		struct LuaPair {
			DB *db;
			DB::Index::iterator i;
		};
		struct LuaIPair {
			int n;
			PtrList *list;
			PtrList::iterator i;
		};
		static int _pair_next(lua_State *l);
		static int _pair_gc(lua_State *l);
		static int _ipair_next(lua_State *l);
		static int _ipair_gc(lua_State *l);
	GCTP_DECLARE_CLASS
	TUKI_DECLARE(Context)
	};

	/// カレントコンテキスト
	inline Context &context()
	{
		GCTP_ASSERT(Context::current_);
		return *Context::current_;
	}

} // namespace gctp

#endif //_GCTP_CONTEXT_HPP_