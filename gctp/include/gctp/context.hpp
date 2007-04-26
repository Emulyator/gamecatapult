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
#include <gctp/tuki.hpp>
#include <tchar.h> // VC限定だな…

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

		/** ロード要求
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		bool load(const _TCHAR *name);

		/** オブジェクト登録
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/08 11:18:22
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Hndl add(const Ptr ptr, const _TCHAR *name = 0);

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

	GCTP_DECLARE_CLASS
	public:
		virtual void serialize(Serializer &);

	public:
		// luapp
		bool setUp(luapp::Stack &L);
		int luaLoad(luapp::Stack &L);
		int luaCreate(luapp::Stack &L);
	TUKI_DECLARE(Context)

		friend Context &context();
	private:
		bool is_open_;
		PtrList ptrs_;
		Context *prev_;

		static Context *current_;
	};

	/// カレントコンテキスト
	inline Context &context() { return *Context::current_; }

} // namespace gctp

#endif //_GCTP_CONTEXT_HPP_