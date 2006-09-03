#ifndef _GCTP_FACTORY_HPP_
#define _GCTP_FACTORY_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ファクトリーメソッドデータベースクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 23:00:42
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/typeinfo.hpp>

namespace gctp {

	class Object;
	class Ptr;

	/// ファクトリーメソッドの型
	typedef Object *(*FactoryMethod)();

	/** GameCatapultファクトリー登録クラス
	 *
	 * このクラスのインスタンスは、静的オブジェクトとして、ファクトリーメソッドの自動登録をするためのもの。
	 *
	 * gctp::Factoryの機構を利用するには、Object派生であること、さらにデフォルトコンストラクタ
	 * を持つこと、が条件。\n
	 * GCTP_IMPLEMENT_FACTORYの実装を参考にFactoryMethodを自前で作れば、必ずしもデフォルトコンストラクタ
	 * を持つ必要は無いけれども。
	 * @see GCTP_DECLARE_FACTORY
	 * @see GCTP_IMPLEMENT_FACTORY
	 * @see GCTP_USE_FACTORY
	 * @see Class
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/09 23:00:53
	 *
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Factory {
	public:
		Factory(const GCTP_TYPEINFO &typeinfo, const FactoryMethod f);
		static Ptr create(const GCTP_TYPEINFO &typeinfo);
		static Ptr create(const char *classname);
	};

} // namespace gctp

/** ファクトリーメソッドを追加するためのマクロ
 *
 * @code
 *  class Foo : public Object {
 *     Foo();
 *     GCTP_DECLARE_FACTORY; //<-こいつをクラス定義内に書く
 *  };
 *
 * in impleremt file.
 *
 * GCTP_IMPLEMENT_FACTORY(Foo, Object); //<-こいつをcppファイル内に書く
 * @endcode
 *
 * アクセス指定子が含まれていることに注意。このマクロの直後はprivateになる。
 */
#define GCTP_DECLARE_FACTORY										\
	public:															\
		static gctp::Factory &useFactory() { return factory__; }	\
		static gctp::Object *create();								\
	private:														\
		static gctp::Factory factory__;

/** GCTP_DECLARE_FACTORYマクロに対応する実装マクロ
 *
 * C++上のクラス名とは別のクラス名をつけたい場合はこちら使用する
 *
 * @code
 *  class Foo : public Object {
 *     Foo();
 *     GCTP_DECLARE_FACTORY; //<-こいつをクラス定義内に書く
 *  };
 *
 * in impleremt file.
 *
 * GCTP_IMPLEMENT_FACTORY(Foo); //<-こいつをcppファイル内に書く
 * @endcode
 */
#define GCTP_IMPLEMENT_FACTORY(_classname)												\
	gctp::Object *_classname::create() {												\
		gctp::Object *ret = new(_classname::allocate(sizeof(_classname))) _classname();	\
		if(ret) ret->setDeleter(_classname::deleter());									\
		return ret;																		\
	}																					\
	gctp::Factory _classname::factory__(GCTP_TYPEID(_classname), _classname::create);

/** GameCatapultのオブジェクトを使用可能にする
 *
 * Factoryが宣言されていても、そのクラスが直接使用されていない（Pointerのテンプレート引数でのみ
 * 使っている場合など）とリンカによってFactoryのオブジェクトが削除される。
 * それを防ぐためにこれをアプリケーション本体のどこかに書いておく。
 */
#define GCTP_USE_FACTORY(_classname)	{ _classname::useFactory(); }

#endif //_GCTP_FACTORY_HPP_