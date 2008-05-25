#ifndef _GCTP_EXTENSION_HPP_
#define _GCTP_EXTENSION_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult リソースリアライザ登録クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/buffwd.hpp> // for BufferPtr
#include <tchar.h>

namespace gctp {

	/** リソース名からリソースを構築する関数ポインタ
	 *
	 * 非同期用に、bufferがNULLだったら
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:17:49
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	typedef Ptr (*RealizeMethod)(BufferPtr buffer);

	/** リソース名からリソースを構築する関数ポインタ
	 *
	 * 非同期用に、bufferがNULLだったら空のオブジェクト（デフォルトコンストラクタが呼ばれただけ）を、
	 * selfにNULL以外が渡されたらそれに対してSetUpを呼び出すようになっていなければいけない。
	 * 同期読みの場合はself==NULL,buffer!=NULLで呼び出される。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:17:49
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	//typedef Ptr (*RealizeMethod)(Ptr self, BufferPtr buffer);

	/** リソースリアライザ登録クラス
	 *
	 * 拡張子→リアライザのＤＢを構成する。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:17:44
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Extension {
	public:
		Extension(const _TCHAR *extension, const RealizeMethod f);
		~Extension();
		bool guard_for_unlink() { return true; }
		static RealizeMethod get(const _TCHAR *extension);
	private:
		const _TCHAR *ext_;
	};

	template<class T, bool (T::*SetUpMethod)(BufferPtr)>
	Ptr realize(BufferPtr buffer)
	{
		Pointer<T> ret = new T;
		if(ret) {
			if((ret.get()->*SetUpMethod)(buffer)) {
				return ret;
			}
		}
		return Ptr();
	}

/*	template<class T, bool (T::*SetUpMethod)(BufferPtr)>
	Ptr realize(Ptr self, BufferPtr buffer)
	{
		if(!self) self = new T;
		if(self && buffer) {
			if((self.get()->*SetUpMethod)(buffer)) {
				return self;
			}
			return Ptr();
		}
		return self;
	}*/

} // namespace gctp

/** 汎用リアライザ定義マクロ
 *
 * グローバルＤＢに登録するのであれば、const char *nameを受け取るsetUpメンバ関数を
 * 用意して、GCTP_IMPLEMENT_REALIZER(txt, Text); などとしてやればよい
 *
 * リアライザは同名のリソースが存在した場合、新規に制作せず既存のオブジェクトを返す。
 *
 * ユーザーがカスタムしたものを定義するにしても、この規約を守ってもらう必要がある。
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#define GCTP_REGISTER_REALIZER(_E, _C)							\
	static Ptr _C##_E##_realize(BufferPtr buffer) {				\
		return gctp::realize<_C, &_C::setUp>(buffer);			\
	}															\
	static Extension _C##_E##_realizer(_T(#_E), _C##_E##_realize)

/** 汎用リアライザ定義マクロ
 *
 * セットアップメソッドを指定できる
 *
 * グローバルＤＢに登録するのであれば、const char *nameを受け取るsetUpメンバ関数を
 * 用意して、GCTP_IMPLEMENT_REALIZER(txt, Text); などとしてやればよい
 *
 * リアライザは同名のリソースが存在した場合、新規に制作せず既存のオブジェクトを返す。
 *
 * ユーザーがカスタムしたものを定義するにしても、この規約を守ってもらう必要がある。
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#define GCTP_REGISTER_REALIZER2(_E, _C, _M)						\
	static Ptr _C##_E##_realize(BufferPtr buffer) {				\
		return gctp::realize<_C, _M>(buffer);					\
	}															\
	static Extension _C##_E##_realizer(_T(#_E), _C##_E##_realize)

/** 汎用リアライザ定義マクロ
 *
 * 拡張子文字列と、realizeメソッドの名前を別にする（共通の実装する場合）
 *
 * グローバルＤＢに登録するのであれば、const char *nameを受け取るsetUpメンバ関数を
 * 用意して、GCTP_IMPLEMENT_REALIZER("setup string", txt, Text); などとしてやればよい
 *
 * リアライザは同名のリソースが存在した場合、新規に制作せず既存のオブジェクトを返す。
 *
 * ユーザーがカスタムしたものを定義するにしても、この規約を守ってもらう必要がある。
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#define GCTP_REGISTER_REALIZER_EX(_S, _E, _C)				\
	static Ptr _C##_E##_realize(BufferPtr buffer) {			\
		return gctp::realize<_C, &_C::setUp>(buffer);		\
	}														\
	static Extension _C##_E##_realizer(_S, _C##_E##_realize)

/** 汎用リアライザ定義マクロ
 *
 * セットアップメソッドを指定できる
 *
 * 拡張子文字列と、realizeメソッドの名前を別にする（共通の実装をする場合）
 *
 * コンテキストＤＢに登録するのであれば、const char *nameを受け取るsetUpメンバ関数を
 * 用意して、GCTP_IMPLEMENT_REALIZER("setup string", txt, Text); などとしてやればよい
 *
 * リアライザは同名のリソースが存在した場合、新規に制作せず既存のオブジェクトを返す。
 *
 * ユーザーがカスタムしたものを定義するにしても、この規約を守ってもらう必要がある。
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#define GCTP_REGISTER_REALIZER2_EX(_S, _E, _C, _M)			\
	static Ptr _C##_E##_realize(BufferPtr buffer) {			\
		return gctp::realize<_C, _M>(buffer);				\
	}														\
	static Extension _C##_E##_realizer(_S, _C##_E##_realize)

#endif //_GCTP_EXTENSION_HPP_
