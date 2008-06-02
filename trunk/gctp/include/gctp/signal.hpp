#ifndef _GCTP_SIGNAL_HPP_
#define _GCTP_SIGNAL_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult Signal/Slotクラスヘッダファイル
 *
 * gctp::Signal / gctp::Slotとは、簡易版のSignal/Slot機構である。
 * Boost.Signalsのほうがより柔軟で便利だが、boost::shared_ptrとboost::anyに依存しており、これをgctp::Pointer / gctp::Handleに
 * よる実装に置き換えたかった。Slotをgctp::Handleで補足することにすれば、scoped_connectionなどを用いなくても、
 * disconnectの操作がより簡便になる。
 * boost::anyもObject派生に限定すれば、gctp::Handleで代用可能である。
 * また、Slot関数自身に、その呼び出しコンテキストでの脱退の操作を許したかった。
 * (これはBoost.Signalsには標準で用意されていない)
 * 用途を入力イベントへの応答やゲームオブジェクトのMixinに限定し、その範囲では便利に使えるようにしてある。
 * そのため種類はすべて呼び出す、しかない。
 *
 * void (int i, int j)のようなpreferred 形式を採用しなかったのは、関数形式を bool (...)に固定し、返り値を次回も実行するか、
 * つまりfalseならdisconnectする、というフラグに使うため。おかげで引数の数に制限ができてしまったが。
 * またboost::functionでSlotを受けるのではなく、Object派生の関数オブジェクトを常に要求する、つまり普通の関数を受け付けない、
 * のは、boost::any&boost:shared_ptrの排除、Handleによるconnect/disconnect判定の実現、のため。普通の関数を受け付けるように
 * しようとすると、結局はBoost.Signals同様にboost::any&boost:shared_ptrを使った実装にならざるを得ない。
 *
 * ツリー構造の形成は、gctp::Sceneのようにgctp::Signalを持つオブジェクトにそのgctp::Signalを発火するメンバ関数を定義し、
 * そのgctp::Slotを提供することで実現する｡gctp::Signalは、スロットのデータベース機能を提供しない
 *
 * (マウスなどの入力メッセージで）処理をのっとりたい場合、つまり、より上位のスロットが実行された時点で下位のスロットを実行しない、
 * という処理を実現するには、Slotのオーバーライドで実現する｡
 *
 * やっぱこれいろいろへんだなぁ。変えたいんだけど、いまさら。。。プライオリティーが変えやすいというメリットはあるけど
 *
 * VC6では\n
 * class Foo {\n
 *     bool bar();\n
 *     MemberSlot0<Foo, bar> slot;\n
 *     Foo() : slot(this) {}\n
 * };\n
 * という表現が使えない。（クラス定義の最中にメンバ関数barをシンボルとして認識できない）\n
 * 替わりに\n
 * class Foo {\n
 *     bool bar();\n
 *     Pointer<Slot> slot;\n
 *     Foo();\n
 * };\n
 * inline Foo::Foo() : slot(new MemberSlot0<Foo, &Foo::bar>(this)) {}\n
 * などする。\n
 * この表現をしてもsignal << slot;という表現が通用するように、Signalは
 * connect(Pointer<Slot> slot)を持つ。が、このために引数リストが違うスロットで
 * あってもエラーにならず接続できてしまう。引数リストが合わないスロットは、呼び出し時に
 * チェックされ安全に削除されるが、何のエラーも返さない。
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/handlelist.hpp>

namespace gctp {

	/** スロット基底クラス
	 *
	 * スロット自身がプライオリティー値を持つため、複数のシグナルに接続したとき、それぞれに別個の優先度を
	 * 与えられないことに注意。マスク値も同様。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/12/06 0:09:31
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Slot : public Object
	{
	public:
		enum {
// うーん。。。
#ifdef MAX_PRIORITY
#define BACKUP_MAX_PRIORITY MAX_PRIORITY
#undef MAX_PRIORITY
#endif
#ifdef MIN_PRIORITY
#define BACKUP_MIN_PRIORITY MIN_PRIORITY
#undef MIN_PRIORITY
#endif
			MAX_PRIORITY = 0x7FFF,
			MIN_PRIORITY = 0x8000
#ifdef BACKUP_MAX_PRIORITY
#define MAX_PRIORITY BACKUP_MAX_PRIORITY
#undef BACKUP_MAX_PRIORITY
#endif
#ifdef BACKUP_MIN_PRIORITY
#define MIN_PRIORITY BACKUP_MIN_PRIORITY
#undef BACKUP_MIN_PRIORITY
#endif
		};
		/// プライオリティーを指定して初期化
		Slot(int16_t pri) : pri_(pri), mask_(0xFFFF) {}
		/// プライオリティーを返す
		int16_t priority() const { return pri_; }
		/// プライオリティーを設定
		int16_t setPriority(int16_t pri) { std::swap(pri_, pri); return pri; }
		/// マスク値を返す
		uint16_t mask() const { return mask_; }
		/// マスク値を設定
		uint16_t setMask(uint16_t mask) { std::swap(mask_, mask); return mask; }
	protected:
		int16_t pri_;
		uint16_t mask_;
	};

	/** シグナルクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Signal : public Object
	{
	public:
		/// 動作変更フラグ
		enum Flag {
			SORT = 1,			///< プライオリティーによるソートを行う
			MASK = 1<<1,		///< マスキングを行う
			_FORCE_WORD_ = 0x7FFF
		};

		/// コンストラクタ
		Signal(uint16_t flags = SORT|MASK) : mask_(0xFFFF), flags_(flags) {}

		/// すでに登録されていないか？
		bool isUnique(const Slot &slot);
		/// スロット接続
		void connect(const Slot &slot);
		/// スロット接続(重複をチェック)
		void connectOnce(const Slot &slot)
		{
			if(isUnique(slot)) connect(slot);
		}
		/// スロット切断
		void disconnect(const Slot &slot);
		/// 接続されているスロットの数を返す
		std::size_t num() const;
		/// マスク値を返す
		uint16_t mask() const { return mask_; }
		/// マスク値を設定
		uint16_t setMask(uint16_t mask) { std::swap(mask_, mask); return mask; }
		/// フラグを返す
		uint16_t flags() const { return flags_; }
		/// フラグを設定
		uint16_t setFlags(uint16_t flags) { std::swap(flags_, flags); return flags; }
	protected:
		/// プライオリティ順にスロットをソート
		void sort();
		/// 実行するかどうかテスト
		bool canInvoke(const Slot &slot);

		typedef HandleList<Slot> SlotList;
		SlotList slots_;
		uint16_t mask_;
		uint16_t flags_;
	};

	/** 単シグナルクラス
	 *
	 * ひとつのスロットしか取れない
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class MonoSignal : public Object
	{
	public:
		/// スロット接続
		void connect(const Slot &slot)
		{
			slot_ = Handle<Slot>(const_cast<Slot *>(&slot));
		}
		/// スロット切断
		void disconnect(const Slot &slot)
		{
			if(slot_ == Handle<Slot>(const_cast<Slot *>(&slot))) slot_ = 0;
		}
		/// スロットが接続されているか？
		bool empty() const { return !slot_; }
	protected:
		Handle<Slot> slot_;
	};

	/** 引数なしのスロットクラス
	 *
	 * virtual bool operator()(int &pri)をオーバーライドする。
	 *
	 * 指定したメソッドでfalseを返すと、実行後そのSignalからはずされる。
	 *
	 * ドキュメントには記載されないが、Slot1～Slot10が定義されていて、最大１０個の引数を持ったSlotを定義できる。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:52:09
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Slot0 : public Slot
	{
	public:
		/// プライオリティーを指定して初期化
		Slot0(int16_t pri) : Slot(pri) {}
		/** 呼び出しの実体
		 * @return 継続するか？(falseを返すとdisconnectされる)または、呼び出しを中断する（接続先のシグナル_Thunkがtrueのとき）
		 */
		virtual bool operator()() = 0;
	};

	/** 引数なしのシグナルクラス
	 *
	 * プライオリティー順に全スロットを呼び出す。
	 *
	 * ドキュメントには記載されないが、Signal1～Signal10が定義されていて、最大１０個の引数を持ったSignalを定義できる。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<bool _Thunk>
	class Signal0 : public Signal
	{
	public:
		typedef Slot0 SlotType;
		/// 関係付けられている関数の呼び出し
		void operator()()
		{
			sort();
			for(SlotList::iterator it = slots_.begin(); it != slots_.end();) {
				Handle<SlotType> slot = (*it);
				if(slot) {
					if(canInvoke(*slot)) {
						if(_Thunk) {
							if((*slot)()) break;
							else ++it;
						}
						else {
							if((*slot)()) ++it;
							else it = slots_.erase(it);
						}
					}
					else ++it;
				}
				else it = slots_.erase(it);
			}
		}

		/// スロット接続
		Signal0 &operator<<(const SlotType &slot) { connect(slot); return *this; }
		/// スロット切断
		Signal0 &operator>>(const SlotType &slot) { disconnect(slot); return *this; }
	};

	/** 引数なしの単シグナルクラス
	 *
	 * ドキュメントには記載されないが、Signal1～Signal10が定義されていて、最大１０個の引数を持ったSignalを定義できる。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class MonoSignal0 : public MonoSignal
	{
	public:
		typedef Slot0 SlotType;
		/// 関係付けられている関数の呼び出し
		bool operator()()
		{
			Handle<SlotType> slot = slot_;
			if(slot) return (*slot)();
			return false;
		}

		/// スロット接続
		MonoSignal0 &operator<<(const SlotType &slot) { connect(slot); return *this; }
		/// スロット切断
		MonoSignal0 &operator>>(const SlotType &slot) { disconnect(slot); return *this; }
	};

	/** シグナルを発行するスロットクラス
	 *
	 * プライオリティー順に全スロットを呼び出す。
	 *
	 * ツリー構造を形成するために使われる。固定の引数として、handledが渡される｡このhandled初期値はtrueで、
	 * trueが設定されたまま返るとこのオブジェクトが保持するSignalを発火しない。下位のスロットにシグナルを渡す場合は
	 * falseを設定する。
	 *
	 * ドキュメントには記載されないが、SignalSlot1～SignalSlot10が定義されていて、(bool handled 以外に）
	 * 最大１０個の引数を持ったSignalSlotを定義できる。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<bool _Thunk>
	class SignalSlot0 : public Slot0
	{
	public:
		typedef Slot0 SlotType;
		/// 関係付けられている関数の呼び出し
		virtual bool operator()()
		{
			bool handled = true, ret;
			ret = (*this)(handled);
			if(!handled) signal_();
			return ret;
		}
		/// スロット接続
		SignalSlot0 &operator<<(const SlotType &slot) { signal_.connect(slot); return *this; }
		/// スロット切断
		SignalSlot0 &operator>>(const SlotType &slot) { signal_.disconnect(slot); return *this; }
		
		/// シグナルオブジェクトを返す
		Signal0<_Thunk> &signal() { return signal_; }
		/// シグナルオブジェクトを返す
		const Signal0<_Thunk> &signal() const { return signal_; }

		/** 呼び出しの実体
		 * @param pri プライオリティ 呼び出し先でこのスロットのプライオリティーを変更可能にするため
		 * @param handled デフォルトでtrue falseを入れると子シグナルを発火しない
		 * @return 継続するか？(falseを返すとdisconnectされる)または、呼び出しを中断する（接続先のシグナル_Thunkがtrueのとき）
		 */
		virtual bool operator()(bool &handled) = 0;

	private:
		Signal0<_Thunk> signal_;
	};

	/** 引数なしのスロットクラス
	 *
	 * fn_に代入。
	 *
	 * 指定したメソッドでfalseを返すと、実行後そのSignalからはずされる。
	 *
	 * ドキュメントには記載されないが、FnSlot1～FnSlot10が定義されていて、最大１０個の引数を持ったFnSlotを定義できる。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:52:09
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<bool (*Method)()>
	class FnSlot0 : public Slot0
	{
	public:
		/// プライオリティーを指定して初期化
		FnSlot0(int16_t pri = 0) : Slot0(pri) {}
		/** 呼び出しの実体
		 * @return 継続するか？(falseを返すとdisconnectされる)
		 */
		virtual bool operator()() { return (*Method)(); }
	};

	/** メンバ関数を指定するSlotクラステンプレート
	 *
	 * 指定したメソッドでfalseを返すと、実行後そのSignalからはずれる。
	 *
	 * 実行されつづける限り、trueを返すこと。
	 *
	 * @param _T オブジェクトの型
	 * @param Method 実行されるメンバ関数の指定
	 *
	 * @code
	 *  class Foo : public Object {
	 *  public:
	 *      bool update(int &pri);
	 *      ConstMemberSlot0<Foo, Foo::update> slot;
	 *  };
	 * 
	 *  Pointer<Foo> foo;
	 *  update_signal.connect(foo->slot);
	 * @endcode
	 * 
	 * メンバ関数ポインターで持つのとどっちがいいのかなぁ…
	 *
	 * ドキュメントには記載されないが、MemberSlot1～MemberSlot10が定義されていて、最大１０個の引数を持った
	 * MemberSlotを定義できる。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:52
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, bool (_T::*Method)()>
	class MemberSlot0 : public Slot0
	{
	public:
		/// コンストラクタ
		MemberSlot0(int16_t pri = 0) : Slot0(pri) {}
		/// コンストラクタ プライオリティーと関連付けるインスタンスを指定する
		MemberSlot0(_T *_this, int16_t pri = 0) : Slot0(pri), this_(_this) {}
		/// インスタンスを指定する
		void bind(_T *_this) { this_ = _this; }
		virtual bool operator()()
		{
			if(this_) return (this_.get()->*Method)();
			return false;
		}
	private:
		Handle<_T> this_;
	};

	/** メンバ関数を指定するSlotクラステンプレート
	 *
	 * MemberSlotのconstつきメンバ関数用。
	 *
	 * 一部コンパイラで必要。コンパイル可能ならMemberSlot<const T, ...>でもよい。
	 *
	 * 指定したメソッドでfalseを返すと、実行後そのSignalからはずれる。\n
	 * 実行されつづける限り、trueを返すこと。
	 *
	 * @param _T オブジェクトの型
	 * @param Method 実行されるメンバ関数の指定
	 *
	 * @code
	 *  class Foo : public Object {
	 *  public:
	 *      bool draw(int &pri) const;
	 *      ConstMemberSlot0<Foo, Foo::draw> slot;
	 *  };
	 * 
	 *  Pointer<Foo> foo;
	 *  draw_invoker.connect(foo->slot);
	 * @endcode
	 *
	 * ドキュメントには記載されないが、ConstMemberSlot1～ConstMemberSlot10が定義されていて、最大１０個の引数を持った
	 * ConstMemberSlotを定義できる。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:52
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, bool (_T::*Method)() const>
	class ConstMemberSlot0 : public Slot0
	{
	public:
		/// コンストラクタ
		ConstMemberSlot0(int16_t pri = 0) : Slot0(pri), this_(_this) {}
		/// コンストラクタ プライオリティーと関連付けるインスタンスを指定する
		ConstMemberSlot0(const _T *_this, int16_t pri = 0) : Slot0(pri), this_(_this) {}
		/// インスタンスを指定する
		void bind(const _T *_this) { this_ = _this; }
		virtual bool operator()()
		{
			if(this_) return (this_.get()->*Method)();
			return false;
		}
	private:
		Handle<const _T> this_;
	};

	/** メンバ関数を指定するSignalSlotクラステンプレート
	 *
	 * 指定したメソッドでfalseを返すと、実行後そのSignalからはずれる。
	 *
	 * 実行されつづける限り、trueを返すこと。
	 *
	 * @param _T オブジェクトの型
	 * @param Method 実行されるメンバ関数の指定
	 *
	 * @code
	 *  class Foo : public Object {
	 *  public:
	 *      bool update(int &pri);
	 *      ConstMemberSignalSlot0<Foo, Foo::update> slot;
	 *  };
	 * 
	 *  Pointer<Foo> foo;
	 *  update_signal.connect(foo->slot);
	 * @endcode
	 * 
	 * ドキュメントには記載されないが、MemberSlot1～MemberSlot10が定義されていて、(int &pri以外に）最大１０個の引数を持った
	 * MemberSlotを定義できる。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:52
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<bool _Thunk, class _T, bool (_T::*Method)(bool &)>
	class MemberSignalSlot0 : public SignalSlot0<_Thunk>
	{
	public:
		/// コンストラクタ
		MemberSignalSlot0(int16_t pri = 0) : SignalSlot0<_Thunk>(pri) {}
		/// コンストラクタ プライオリティーと関連付けるインスタンスを指定する
		MemberSignalSlot0(_T *_this, int16_t pri = 0) : SignalSlot0<_Thunk>(pri), this_(_this) {}
		/// インスタンスを指定する
		void bind(_T *_this) { this_ = _this; }
		virtual bool operator()(bool &handled)
		{
			if(this_) return (this_.get()->*Method)(handled);
			return false;
		}
	private:
		Handle<_T> this_;
	};

	/** メンバ関数を指定するSignalSlotクラステンプレート
	 *
	 * MemberSignalSlotのconstつきメンバ関数用。
	 *
	 * 一部コンパイラで必要。コンパイル可能ならMemberSignalSlot<const T, ...>でもよい。
	 *
	 * 指定したメソッドでfalseを返すと、実行後そのSignalからはずれる。\n
	 * 実行されつづける限り、trueを返すこと。
	 *
	 * @param _T オブジェクトの型
	 * @param Method 実行されるメンバ関数の指定
	 *
	 * @code
	 *  class Foo : public Object {
	 *  public:
	 *      bool draw(int &pri) const;
	 *      ConstMemberSignalSlot0<Foo, Foo::draw> slot;
	 *  };
	 * 
	 *  Pointer<Foo> foo;
	 *  draw_invoker.connect(foo->slot);
	 * @endcode
	 *
	 * ドキュメントには記載されないが、ConstMemberSignalSlot1～ConstMemberSignalSlot10が定義されていて、
	 * (bool handled 以外に）最大１０個の引数を持ったConstMemberSignalSlotを定義できる。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:52
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<bool _Thunk, class _T, bool (_T::*Method)(bool &) const>
	class ConstMemberSignalSlot0 : public SignalSlot0<_Thunk>
	{
	public:
		/// コンストラクタ
		ConstMemberSignalSlot0(int16_t pri = 0) : SignalSlot0<_Thunk>(pri) {}
		/// コンストラクタ プライオリティーと関連付けるインスタンスを指定する
		ConstMemberSignalSlot0(const _T *_this, int16_t pri = 0) : SignalSlot0<_Thunk>(pri), this_(_this) {}
		/// インスタンスを指定する
		void bind(const _T *_this) { this_ = _this; }
		virtual bool operator()(bool &handled)
		{
			if(this_) return (this_.get()->*Method)(handled);
			return false;
		}
	private:
		Handle<const _T> this_;
	};

} // namespace gctp

#include <boost/preprocessor/cat.hpp> 
#include <gctp/signal.inl>

#endif //_GCTP_SIGNAL_HPP_