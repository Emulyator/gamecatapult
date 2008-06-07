#ifndef _GCTP_CORE_CONTEXT_HPP_
#define _GCTP_CORE_CONTEXT_HPP_
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
#include <gctp/signal.hpp>
#include <tchar.h>

namespace gctp {
	class Serializer;
}

namespace gctp { namespace core {

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
		 */
		Context(bool do_push = true) : is_pushed_(false), loading_async_(false)
		{
			on_ready_slot.bind(this);
			if(do_push) push();
		}

		/** コンストラクタ
		 *
		 * 親を指定する
		 *
		 * 同時にカレントに設定。（コンテキストスタックに自分をプッシュ）
		 */
		Context(Handle<Context> parent, bool do_push = true) : is_pushed_(false), loading_async_(false), parent_(parent)
		{
			on_ready_slot.bind(this);
			if(do_push) push();
		}

		/** デストラクタ
		 *
		 * 同時にコンテキストスタックをポップ
		 *
		 * コンテキストは生成された順に開放されなければならない。（そうしないと
		 * フラグメントを起こすから）カレントが自分で無かった場合、エラーメッセージを送出する
		 *
		 * デストラクタ内なので、例外は投げない
		 */
		~Context()
		{
			pop();
		}

		/// コンテキストをカレントに
		void push();
		/// 直前のコンテキストをカレントに
		void pop();

		/// 子コンテキストを作る
		Pointer<Context> newChild(bool do_push = true)
		{
			return new Context(this, do_push);
		}

		Handle<Context> parent() { return parent_; }

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
		 * @return 未構築のオブジェクト。リクエストに失敗した場合はヌルハンドルが返る。
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl loadAsync(const _TCHAR *name);

		/** (作成済みの)オブジェクト登録
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		void add(const Ptr ptr);

		/** (作成済みの)オブジェクト登録
		 *
		 * ほかのクリエイト系と違い、これは既存エントリを上書きする
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl insert(const Ptr ptr, const _TCHAR *name);

		/// オブジェクト登録
		template<class T>
		Handle<T> create(const _TCHAR *name)
		{
			if(name) {
				Hndl h = find(name);
				if(h) return h;
			}
			Pointer<T> p = new T;
			if(p) return insert(p, name);
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
				if(p->setUp()) return insert(p, name);
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
				if(p->setUp(name)) return insert(p, name);
			}
			return 0;
		}

		/** オブジェクト製作
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl create(const GCTP_TYPEINFO &typeinfo);
		
		/** オブジェクト製作
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl create(const char *classname);

		/** 名前付きオブジェクト製作
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl create(const GCTP_TYPEINFO &typeinfo, const _TCHAR *name);
		
		/** 名前付きオブジェクト製作
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl create(const char *classname, const _TCHAR *name);

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

		/// カレントコンテキストを返す
		inline static Context &current()
		{
			GCTP_ASSERT(current_);
			return *current_;
		}

		/// 非同期読み込みによるロード処理か？（リアライズ関数の中などで使用）
		bool loadingAsync()
		{
			return loading_async_;
		}

	protected:
		// luapp
		bool setUp(luapp::Stack &L);
		int newChild(luapp::Stack &L);
		int load(luapp::Stack &L);
		int loadAsync(luapp::Stack &L);
		int isReady(luapp::Stack &L);
		int create(luapp::Stack &L);
		int find(luapp::Stack &L);
		int pairs(luapp::Stack &L);
		int ipairs(luapp::Stack &L);
		static int current(luapp::Stack &L);

		bool loadAsync(const _TCHAR *name, const Slot2<const _TCHAR *, BufferPtr> *callback);
		bool onReady(const _TCHAR *name, BufferPtr buffer);
		MemberSlot2<Context, const _TCHAR *, BufferPtr, &Context::onReady> on_ready_slot;

	private:
		bool is_pushed_;
		bool loading_async_;
		PtrList ptrs_;
		DB db_;
		Handle<Context> parent_;
		Handle<Context> prev_;

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

}} // namespace gctp.core

namespace gctp {
	/// カレントコンテキスト
	inline core::Context &context()
	{
		return core::Context::current();
	}
}

#endif //_GCTP_CORE_CONTEXT_HPP_