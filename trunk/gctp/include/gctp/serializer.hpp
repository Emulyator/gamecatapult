#ifndef _GCTP_SERIALIZER_HPP_
#define _GCTP_SERIALIZER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult シリアライザクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 23:00:42
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointervector.hpp>
#include <gctp/iobstream.hpp>

namespace gctp {

	/** GameCatapultシリアライザクラス
	 *
	 * シリアライズに関わる問題は、Ptrの扱いを残すのみ。（一応解決。だが、まだ問題あり）
	 *
	 * シリアライズの過程で生成したオブジェクトをこのSerializerが一端保持してしまう。\n
	 * なもんでロードが終わったらSerializerが解体されるようにコーディングすべし。
	 *
	 * @code
	 * // ファイル"serialize"にシリアライズされる
	 * obfstream stream("serialize");
	 * Serializer serializer(stream);
	 * serializer << foo << bar;
	 * @endcode
	 *
	 * Pointerをシリアライズするには、それで指されるオブジェクトが、クラス登録、ファクトリー登録
	 * されている必要がある。\n
	 * 実質シリアライズをするオブジェクトは、GCTP_DECLARE_CLASS GCTP_IMPLEMENT_CLASS を書く必要がある。
	 * @see Class
	 * @see Factory
	 * @see GCTP_DECLARE_SERIALIZE
	 * @see GCTP_IMPREMENT_SERIALIZE_BEGIN
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/09 23:01:14
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Serializer {
	public:
		/// 読み込み用に初期化
		Serializer(ibstream &stream);
		/// 書き込み用に初期化
		Serializer(obstream &stream);
		/// オープンされているか？
		bool isOpen() { return istream_ || ostream_; }
		/// 読み込みモードか？
		bool isLoading() { return istream_ ? (!istream_->eof()) : false; }
		/// 書き込みモードか？
		bool isWriting() { return ostream_ ? true : false; }
		/// 読み込みストリームを返す
		ibstream &istream() { return *istream_; }
		/// 書き込みストリームを返す
		obstream &ostream() { return *ostream_; }
		/// 読み込み時に製作されたオブジェクトを保持するベクタ
		PtrVector ptrs;
		/// 指定ポインターと同じ値を持つptrs内のインデックス+1の値を返す
		std::size_t findPtr(Ptr ptr);

		class LoadFailExeption {};
		void checkVersion(int ver);
	private:
		ibstream *istream_;
		obstream *ostream_;
		enum{ HEAD = 'PTCG', MAGIC = 'IRES', _FORCE_DWORD_ = 0x7FFFFFFF };
	};

	/** シリアライズ演算子テンプレート
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/09 23:02:24
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<typename _T>
	Serializer &operator<<(Serializer& lhs, _T &rhs)
	{
		if(lhs.isLoading()) lhs.istream() >> rhs;
		else if(lhs.isWriting()) lhs.ostream() << rhs;
		return lhs;
	}

	/** Object用に特殊化したシリアライズ演算子
	 *
	 * Object派生は、Objectにキャストすること。
	 */
	template<>
	Serializer &operator<<(Serializer &lhs, Object &rhs);

	/** Ptr用に特殊化したシリアライズ演算子
	 *
	 * 共有オブジェクトを正しくシリアライズするため。
	 *
	 * Pointerの各特殊化バージョンは、Ptrにキャストすること。
	 */
	template<>
	Serializer &operator<<(Serializer &lhs, Ptr &rhs);

	/** Hndl用に特殊化したシリアライズ演算子
	 *
	 * 共有オブジェクトを正しくシリアライズするため。
	 *
	 * Handleの各特殊化バージョンは、Hndlにキャストすること。
	 */
	template<>
	Serializer &operator<<(Serializer &lhs, Hndl &rhs);
	
	/** Pointer用シリアライズ演算子
	 *
	 * ちょっと苦肉の策
	 */
	template<typename PointeeType> Serializer &operator>>(Serializer &lhs, Pointer<PointeeType> &rhs) {
		return lhs << (*reinterpret_cast<Ptr *>(&rhs));
	}

	/** Handle用シリアライズ演算子
	 *
	 * ちょっと苦肉の策
	 */
	template<typename HandledType> Serializer &operator>>(Serializer &lhs, Handle<HandledType> &rhs) {
		return lhs << (*reinterpret_cast<Hndl *>(&rhs));
	}

} // namespace gctp

/** シリアライズ機能を追加するためのマクロ
 *
 * @code
 *  class Foo : Object {
 *     Foo();
 *     int menber1;
 *     int menber2;
 *     Pointer<Bar> ptr_member;
 *  
 *  GCTP_DECLARE_SERIALIZE	//<-こいつをクラス定義内に書く
 *  };
 *
 * in impleremt file.
 *
 * //これらをcppファイル内に書く
 * GCTP_IMPLEMENT_SERIALIZE(Foo, Object)
 *    GCTP_SERIALIZE(<< menber2 << menber2 >> ptr_menber)
 * GCTP_IMPLEMENT_SERIALIZE_END();
 * @endcode
 *
 * アクセス指定子が含まれていることに注意。このマクロの直後はprotectedになる。
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/01/05 13:30:37
 * $Id: store.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_DECLARE_SERIALIZE						\
	protected:										\
		virtual void serialize(gctp::Serializer &);

/** 上記宣言マクロに対応する実装マクロ
 *
 * @code
 *  class Foo : public Object {
 *     Foo();
 *  GCTP_DECLARE_CLASS
 *  GCTP_DECLARE_SERIALIZE	//<-こいつをクラス定義内に書く
 *     int menber1;
 *     int menber2;
 *     Pointer<Bar> ptr_member;
 *  };
 *
 * in impleremt file.
 *
 * //これらをcppファイル内に書く
 * GCTP_IMPLEMENT_CLASS(Foo, Object)
 * GCTP_IMPLEMENT_SERIALIZE_BEGIN(0, Foo, Object)
 *    GCTP_SERIALIZE(<< menber2 << menber2 >> ptr_menber)	// シリアライズするメンバを<<で繋げて書く
 * GCTP_IMPLEMENT_SERIALIZE_END();
 * @endcode
 *
 * 必ずスーパークラスを指定する
 *
 * Pointer<>をシリアライズしたい場合は、GCTP_SERIALIZEPTRマクロか>>演算子を使う。
 * ただし、これはHndl.lockで確保したPtrに対して行わないこと。Hndlとして登録されているオブジェクトを
 * シリアライズすると、ロード時にHndl登録されたオブジェクトとは別にインスタンス化されてしまう。
 * Hndl.lockはごく短い時間、処理のアトミック性を維持するために用意されたもので、これで確保したオブジェクトを
 * シリアライズしようという欲求は、間違ったものである。
 *
 * @param _ver バージョン番号 メンバ構成が変わったら、それ以前のシリアライズデータが無効になるように、別の値を指定する
 * @param _classname クラス名
 * @param _superclassname 継承したクラス名
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/01/05 13:30:37
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_IMPREMENT_SERIALIZE_BEGIN(_ver, _classname, _superclassname)	\
	void _classname::serialize(gctp::Serializer &_serializer_) {			\
		_superclassname::serialize(_serializer_);							\
		_serializer_.checkVersion(_ver);
#define GCTP_SERIALIZE(_members)	_serializer_ _members;
#define GCTP_SERIALIZEPTR(_member)	(*reinterpret_cast<Ptr *>(&_member))
#define GCTP_SERIALIZEHNDL(_member)	(*reinterpret_cast<Hndl *>(&_member))

#define GCTP_IMPREMENT_SERIALIZE_END	\
	}

#endif //_GCTP_SERIALIZER_HPP_