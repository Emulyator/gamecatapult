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
#include <gctp/pointer.hpp>
//#ifndef GCTP_NOTDEF_BOOST_FUNC
//#include <boost/function.hpp>
//#endif
#include <tchar.h> // VC限定だな…

namespace gctp {

// どういうわけか特定のところで、boost::function1ありだとVC6ではコンパイルできないので…
//#ifndef GCTP_NOTDEF_BOOST_FUNC
	/** リソース名からリソースを構築する関数ポインタ
	 *
	 * （ちょっと思うところがあって関数オブジェクトでなくポインタに）
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:17:49
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	typedef Ptr (*RealizeMethod)(const _TCHAR *);
	//typedef boost::function1<PointerBase, const _TCHAR *> RealizeMethod;

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
		//Extension(const _TCHAR *extension, const RealizeMethod &f);
		~Extension();
		bool guard_for_unlink() { return true; }
		static RealizeMethod get(const _TCHAR *extension);
	private:
		const _TCHAR *ext_;
	};
//#endif

#if 0
	/** 汎用リアライザテンプレート
	 *
	 * グローバルＤＢに登録するのであれば、const char *nameを受け取るsetUpメンバ関数を
	 * 用意して、Extension txt_realizer("txt", Realizer<Text>()); としてやればよい
	 *
	 * リアライザは同名のリソースが存在した場合、新規に制作せず既存のオブジェクトを返す。
	 *
	 * ユーザーがカスタムしたものを定義するにしても、この規約を守ってもらう必要がある。
	 * 
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:18:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	class Realizer {
	public:
		PointerBase operator()(const char *name) {
			Handle<_T> h = db()[name];
			if(h) return h.lock();
			Pointer<_T> ret = Object::create(GCTP_TYPEID(_T));
			if(ret) {
				ret->setUp(name);
				db().insert(Hndl(ret.get()), name);
			}
			return ret;
		}
	};
#endif

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
#define GCTP_REGISTER_REALIZER(_E, _C)						\
	static Ptr _C##_E##_realize(const _TCHAR *name) {		\
		return createOnDB<_C>(name);						\
	}														\
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
#define GCTP_REGISTER_REALIZER2(_E, _C, _M)					\
	static Ptr _C##_E##_realize(const _TCHAR *name) {		\
		return createOnDB<_C, _M>(name);					\
	}														\
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
#define GCTP_REGISTER_REALIZER_EX(_S, _E, _T)				\
	static Ptr _T##_E##_realize(const _TCHAR *name) {		\
		return createOnDB<_T>(name);						\
	}														\
	static Extension _T##_E##_realizer(_S, _T##_E##_realize)

/** 汎用リアライザ定義マクロ
 *
 * セットアップメソッドを指定できる
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
#define GCTP_REGISTER_REALIZER2_EX(_S, _E, _T, _M)			\
	static Ptr _T##_E##_realize(const _TCHAR *name) {		\
		return createOnDB<_T, _M>(name);					\
	}														\
	static Extension _T##_E##_realizer(_S, _T##_E##_realize)

#endif //_GCTP_EXTENSION_HPP_
