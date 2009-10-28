#ifndef _GCTP_POINTER_HPP_
#define _GCTP_POINTER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 参照カウンタ型スマートポインタ基底クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 22:37:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <functional>           // for std::less
#include <algorithm>            // for std::swap
#include <iosfwd>               // for std::basic_ostream
#include <gctp/object.hpp>
#include <boost/static_assert.hpp>
#include <boost/detail/workaround.hpp>

namespace gctp {

	namespace detail {
		/** 弱参照のためのスタブ
		 */
		class Stub {
			friend class Object;
			friend class Hndl;

			Stub(Object *p) : refcount_(1), p_(p), mutex_(0)
			{
				p_->stub_ = this;
				synchronize(p_->mutex_ ? true : false);
			}
			Stub(const Stub &src); // not implement
			~Stub()
			{
				if(p_) p_->stub_ = 0;
				if(mutex_) delete mutex_;
			}

			bool addRef() const
			{
				Object::AutoLock al(mutex_);
				if(refcount_ > 0) {
					refcount_++;
					return true;
				}
				else return false;
			}

			bool decRef() const
			{
				Object::AutoLock al(mutex_);
				if(refcount_ > 0 && --refcount_==0) {
					refcount_ = -1;
					return true;
				}
				else return false;
			}

			void synchronize(bool yes)
			{
				if(yes && !mutex_) {
					mutex_ = new Mutex;
				}
				if(!yes && mutex_) {
					delete mutex_; mutex_ = 0;
				}
			}

			mutable int refcount_;
			Object *p_;
			Mutex *mutex_;
		};
	}

	/** 参照カウンタポインター基底クラス
	 *
	 * 型抹消の手段としてvoidポインター的に使う。
	 *
	 * @note MT-UNSAFE(gctp::Objectに対してsychronizeを呼び出せばOK)
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/26 22:37:31
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Ptr {
	public:
		Ptr() : p_(0) {}

		/// 参照先の型情報を返す
		const GCTP_TYPEINFO &typeinfo() const { return GCTP_THISTYPEID(*p_); }

		Ptr(const Object *p) : p_(0)
		{
			if(p && p->addRef()) p_ = const_cast<Object *>(p);
		}

		Ptr(Ptr const & rhs) : p_(0)
		{
			if(rhs.p_ && rhs.p_->addRef()) p_ = rhs.p_;
		}

		~Ptr() { release(); }

		Ptr & operator=(Ptr const & rhs)
		{
			Ptr(rhs).swap(*this);
			return *this;
		}
		
		Ptr & operator=(Object * rhs)
		{
			Ptr(rhs).swap(*this);
			return *this;
		}

		/// 二つのポインタを入れ替え
		void swap(Ptr & rhs)
		{
			std::swap(p_, rhs.p_);
		}

		/// 解放
		void release()
		{
			if(p_ && p_->decRef()) Object::destroy(p_);
			p_ = 0;
		}

		/// 生ポインタを取得
		Object *get()
		{
			return p_;
		}

		/// 生ポインタを取得
		const Object *get() const
		{
			return p_;
		}

		Object &operator*()
		{
			return *p_;
		}

		const Object &operator*() const
		{
			return *p_;
		}

		Object *operator->()
		{
			return p_;
		}

		const Object *operator->() const
		{
			return p_;
		}

#if defined(__SUNPRO_CC) && BOOST_WORKAROUND(__SUNPRO_CC, <= 0x530)

		operator bool () const
		{
			return p_ != 0;
		}

#elif \
    ( defined(__MWERKS__) && BOOST_WORKAROUND(__MWERKS__, < 0x3200) ) || \
    ( defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ < 304) )

		typedef const GCTP_TYPEINFO &(Ptr::*SafeBoolType) () const;

		operator SafeBoolType() const
		{
			return p_ == 0 ? 0 : &Ptr::typeinfo;
		}

#else 

		/*typedef Object * Ptr::*SafeBoolType;

		operator SafeBoolType() const // never throws
		{
			return p_ == 0 ? 0 : &Ptr::p_;
		}*/
	private:
		static void safebooltrue( Ptr *** ) {}
	    typedef void (*SafeBoolType)( Ptr *** );
	public:
		operator SafeBoolType() const
		{
			return p_ == 0 ? 0 
				: safebooltrue;
		}

#endif

		// operator! is redundant, but some compilers need it
		bool operator! () const
		{
			return p_ == 0;
		}

		bool operator==(Ptr const & rhs) const
		{
			return p_ == rhs.p_;
		}

		bool operator!=(Ptr const & rhs) const
		{
			return p_ != rhs.p_;
		}

	protected:
		Object *p_;
	};

	inline bool operator==(Object * a, Ptr const & b)
	{
		return a == b.get();
	}

	inline bool operator!=(Object * a, Ptr const & b)
	{
		return a != b.get();
	}

	inline bool operator<(Ptr const & a, Ptr const & b)
	{
		return std::less<const Object *>()(a.get(), b.get());
	}

	/// 二つのポインタを入れ替え
	inline void swap(Ptr & lhs, Ptr & rhs)
	{
		lhs.swap(rhs);
	}

	// mem_fn support

	inline const Object * get_pointer(Ptr const & p)
	{
		return p.get();
	}

	// operator<<

	template<class E, class T> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Ptr const & p)
	{
		os << p.get();
		return os;
	}

	/** 参照カウンタポインタークラステンプレート
	 *
	 * パフォーマンス上の都合から、排他制御はもっと高次の部分で行ってもらう設計を要求する。
	 *
	 * 具体的には、必要なときにgctp::Objectに対してsychronize(true)を呼び出す。\n
	 * （デフォルトでは排他制御をしない）
	 *
	 * @code
	 * class Foo : public Object {...};
	 * Pointer<Foo> p = new Foo;
	 * p->member();
	 * p = 0;  // これで解放
	 * @endcode
	 *
	 * Copyright (c) 2001, 2002 Peter Dimov
	 * @note MT-UNSAFE(gctp::Objectに対してsychronizeを呼び出せばOK)
	 */
	template<class T>
	class Pointer : public Ptr {
	private:

		typedef Pointer this_type;

	public:

		typedef T element_type;

		Pointer() {}

		Pointer(T * p): Ptr(p) {}

		Pointer(Ptr const & rhs): Ptr(GCTP_DYNCAST<const T *>(rhs.get())) {}

		Pointer(Pointer const & rhs): Ptr(rhs.p_) {}

		Pointer & operator=(Pointer const & rhs)
		{
			this_type(rhs).swap(*this);
			return *this;
		}

		Pointer & operator=(T * rhs)
		{
			this_type(rhs).swap(*this);
			return *this;
		}
		
#ifdef __MWERKS__
		template<class _T1> operator Pointer<_T1>() { return Pointer<_T1>(GCTP_DYNCAST<_T1 *>(get())); }
#else
		template<class _T1> Pointer(Pointer<_T1>& rhs) : Ptr(GCTP_DYNCAST<T *>(rhs.get())) {}
		
		template<class _T1> Pointer<T>& operator=(Pointer<_T1>& rhs)
		{
			release();
			if(Object *tmp = GCTP_DYNCAST<T *>(rhs.get())) {
				this_type(rhs).swap(*this);
			}
			return *this;
		}
#endif

		/// 生ポインタを取得
		T *get()
		{
			return reinterpret_cast<T *>(p_);
		}

		/// 生ポインタを取得
		const T *get() const
		{
			return reinterpret_cast<const T *>(p_);
		}

		T &operator*()
		{
			return *get();
		}

		const T &operator*() const
		{
			return *get();
		}

		T *operator->()
		{
			return get();
		}

		const T *operator->() const
		{
			return get();
		}
	};

	template<class T> inline bool operator==(Ptr const & a, Pointer<T> const & b)
	{
		return a.get() == b.get();
	}

	template<class T> inline bool operator==(Pointer<T> const & a, Ptr const & b)
	{
		return a.get() == b.get();
	}

	template<class T> inline bool operator!=(Ptr const & a, Pointer<T> const & b)
	{
		return a.get() != b.get();
	}

	template<class T> inline bool operator!=(Pointer<T> const & a, Ptr const & b)
	{
		return a.get() != b.get();
	}

	template<class T, class U> inline bool operator==(Pointer<T> const & a, Pointer<U> const & b)
	{
		return a.get() == b.get();
	}

	template<class T, class U> inline bool operator!=(Pointer<T> const & a, Pointer<U> const & b)
	{
		return a.get() != b.get();
	}

	template<class T> inline bool operator==(Pointer<T> const & a, T * b)
	{
		return a.get() == b;
	}

	template<class T> inline bool operator!=(Pointer<T> const & a, T * b)
	{
		return a.get() != b;
	}

	template<class T> inline bool operator==(T * a, Pointer<T> const & b)
	{
		return a == b.get();
	}

	template<class T> inline bool operator!=(T * a, Pointer<T> const & b)
	{
		return a != b.get();
	}

	template<class T> inline bool operator<(Pointer<T> const & a, Pointer<T> const & b)
	{
		return std::less<const T *>()(a.get(), b.get());
	}

	template<class T> void swap(Pointer<T> & lhs, Pointer<T> & rhs)
	{
		lhs.swap(rhs);
	}

	// mem_fn support

	template<class T> Pointer<T> static_pointer_cast(Ptr const & p)
	{
		return static_cast<T *>(p.get());
	}

	template<class T> Pointer<T> dynamic_pointer_cast(Ptr const & p)
	{
		return GCTP_DYNCAST<T *>(p.get());
	}

	template<class T> T * get_pointer(Pointer<T> const & p)
	{
		return p.get();
	}

	template<class T, class U> Pointer<T> static_pointer_cast(Pointer<U> const & p)
	{
		return static_cast<T *>(p.get());
	}

	template<class T, class U> Pointer<T> dynamic_pointer_cast(Pointer<U> const & p)
	{
		return GCTP_DYNCAST<T *>(p.get());
	}

	// operator<<

	template<class E, class T, class Y> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Pointer<Y> const & p)
	{
		os << p.get();
		return os;
	}

	/** GameCatapultハンドル基底クラス
	 *
	 * ようするにweak_ptr。\n
	 * 参照数を上げずにオブジェクトを補足し、指しているオブジェクトが解体されていたら、NULLを返す。
	 *
	 * 型抹消の手段としてvoidポインター的に使う。
	 *
	 * @note MT-UNSAFE(gctp::Objectに対してsychronizeを呼び出せばOK)
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/28 19:20:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Hndl {
	public:
		friend class Object;
		Hndl() : stub_(0) {}

		Hndl(const Hndl &src) : stub_(src.stub_)
		{
			if(stub_) stub_->addRef();
		}

		Hndl(const Object *p) : stub_(0)
		{
			enter(const_cast<Object *>(p));
		}

		Hndl(Ptr const & p) : stub_(0)
		{
			enter(const_cast<Object *>(p.get()));
		}

		~Hndl()
		{
			expire();
		}

		/// ハンドルのスタブを取得
		const detail::Stub *stub() const { return stub_; }

		/// 参照先の型情報を返す（NULLポインタの場合typeid(Object)か、gctp::TypeInfo(Object)が返ってくる）
		const GCTP_TYPEINFO &typeinfo() const
		{
			if(stub_ && stub_->p_) return GCTP_THISTYPEID(*stub_->p_);
			return GCTP_TYPEID(Object);
		}

		Hndl & operator=(Ptr const & rhs)
		{
			Hndl(rhs).swap(*this);
			return *this;
		}

		Hndl & operator=(Hndl const & rhs)
		{
			Hndl(rhs).swap(*this);
			return *this;
		}
		
		Hndl & operator=(Object * rhs)
		{
			Hndl(rhs).swap(*this);
			return *this;
		}

		/// 二つのハンドルを入れ替え
		void swap(Hndl & rhs)
		{
			std::swap(stub_, rhs.stub_);
		}

		// implicit conversion to "bool"

#if defined(__SUNPRO_CC) && BOOST_WORKAROUND(__SUNPRO_CC, <= 0x530)

		operator bool () const
		{
			return get() != 0;
		}

#elif \
    ( defined(__MWERKS__) && BOOST_WORKAROUND(__MWERKS__, < 0x3200) ) || \
    ( defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ < 304) )

		typedef const GCTP_TYPEINFO &(Hndl::*unspecified_bool_type) () const;

		operator unspecified_bool_type () const
		{
			return get() == 0? 0: &Hndl::typeinfo;
		}

#else 

		/*typedef detail::Stub * Hndl::*SafeBoolType;

		operator SafeBoolType() const // never throws
		{
			return get() == 0? 0: &Hndl::stub_;
		}*/

	private:
		static void safebooltrue( Hndl *** ) {}
	    typedef void (*SafeBoolType)( Hndl *** );
	public:
		operator SafeBoolType() const
		{
			return get() == 0 ? 0 
				: safebooltrue;
		}

#endif
		// operator! is redundant, but some compilers need it

		bool operator! () const // never throws
		{
			return get() == 0;
		}

		bool operator==(Hndl const & rhs) const
		{
			return get() == rhs.get();
		}

		bool operator!=(Hndl const & rhs) const
		{
			return get() != rhs.get();
		}

		bool operator==(Object const * rhs) const
		{
			return get() == rhs;
		}

		bool operator!=(Object const * rhs) const
		{
			return get() != rhs;
		}

		/// 生ポインタを取得
		Object *get()
		{
			if(stub_) return stub_->p_;
			return 0;
		}

		/// 生ポインタを取得
		const Object *get() const
		{
			if(stub_) return stub_->p_;
			return 0;
		}

		Object &operator*()
		{
			return *get();
		}

		const Object &operator*() const
		{
			return *get();
		}

		Object *operator->()
		{
			return get();
		}

		const Object *operator->() const
		{
			return get();
		}

		/// gctp::Ptrに変換
		Ptr lock()
		{
			Object::AutoLock al(stub_ ? stub_->mutex_ : 0);
			return Ptr(get());
		}

		/// const gctp::Ptrに変換
		const Ptr lock() const
		{
			Object::AutoLock al(stub_ ? stub_->mutex_ : 0);
			return Ptr(get());
		}

	protected:
		void expire() const
		{
			if(stub_ && stub_->decRef()) delete stub_;
			stub_ = 0;
		}

		mutable detail::Stub *stub_;

	private:
		void enter(Object *p)
		{
			if(p) {
				Object::AutoLock al(p->mutex_);
				stub_ = p->stub_;
				if(!stub_) stub_ = new detail::Stub(p);
				if(!stub_->addRef()) stub_ = 0;
			}
		}
	};

	inline bool operator==(Object * a, Hndl const & b)
	{
		return a == b.get();
	}

	inline bool operator!=(Object * a, Hndl const & b)
	{
		return a != b.get();
	}

	inline bool operator<(Hndl const & a, Hndl const & b)
	{
		return std::less<const Object *>()(a.get(), b.get());
	}

	/// 二つのポインタを入れ替え
	inline void swap(Hndl & lhs, Hndl & rhs)
	{
		lhs.swap(rhs);
	}

	// mem_fn support

	inline const Object * get_pointer(Hndl const & p)
	{
		return p.get();
	}

	// operator<<

	template<class E, class T> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Hndl const & p)
	{
		os << p.get();
		return os;
	}

	/** ハンドルクラステンプレート
	 *
	 * ようするにweak_ptr。\n
	 * 参照数を上げずにオブジェクトを補足し、指しているオブジェクトが解体されていたら、NULLを返す。
	 *
	 * パフォーマンス上の都合から、排他制御はもっと高次の部分で行ってもらう設計を要求する。\n
	 * synchronizeをしても、lockするようにしないとダメなんだが…\n
	 * やはりlockしないとポインターを取り出せないようにすべきか
	 *
	 * @code
	 * class Foo : public Object {...};
	 * Pointer<Foo> p = new Foo;
	 * Handle<Foo> h = p;
	 * h->member();
	 * Pointer<Foo> p1 = h.lock(); // gctp::Pointerで取り出す
	 * p = 0;  // これで解放
	 * if(h) printf("hはまだ生きてる");
	 * p1 = 0;  // これで解放
	 * if(!h) printf("hは既に死んでいる");
	 * @endcode
	 *
	 * Copyright (c) 2001, 2002 Peter Dimov
	 *
	 * @note MT-UNSAFE(gctp::Objectに対してsychronizeを呼び出せばOK)
	 */
	template<class T>
	class Handle : public Hndl {
	public:

		typedef T element_type;

		Handle() {}

		Handle(T * p): Hndl(p) {}

		Handle(Ptr const & p): Hndl(p)
		{
			if(GCTP_DYNCAST<const T *>(p.get())==0) expire();
		}

		Handle(Pointer<T> const &p): Hndl(p) {}

		Handle(Hndl const & rhs): Hndl(rhs)
		{
			if(GCTP_DYNCAST<const T *>(rhs.get())==0) expire();
		}

		Handle(Handle const & rhs): Hndl(rhs) {}
		
		Handle & operator=(T *rhs)
		{
			Hndl(rhs).swap(*this);
			return *this;
		}

#if defined(__MWERKS__) || (defined(_MSC_VER) && (_MSC_VER < 1300))
		template<typename T> operator Handle<T>() { return Handle<T>(GCTP_DYNCAST<T *>(get())); }
#else
		template<class _T1> Handle(const Handle<_T1>& rhs): Hndl(rhs)
		{
			if(GCTP_DYNCAST<const T *>(rhs.get())==0) expire();
		}

		template<class _T1> Handle<T>& operator=(const Handle<_T1>& rhs)
		{
			if(GCTP_DYNCAST<const T *>(rhs.get())==0) expire();
			else {
				Hndl(rhs).swap(*this);
			}
			return *this;
		}
#endif

		/// 生ポインタを取得
		T *get()
		{
			return reinterpret_cast<T *>(Hndl::get());
		}

		/// 生ポインタを取得
		const T *get() const
		{
			return reinterpret_cast<const T *>(Hndl::get());
		}

		T &operator*()
		{
			return *get();
		}

		const T &operator*() const
		{
			return *get();
		}

		T *operator->()
		{
			return get();
		}

		const T *operator->() const
		{
			return get();
		}

		/// gctp::Pointerに変換
		Pointer<T> lock()
		{
			return Pointer<T>(get());
		}

		/// const gctp::Pointerに変換
		const Pointer<T> lock() const
		{
			return Pointer<T>(get());
		}
	};

	template<class T> inline bool operator==(Hndl const & a, Handle<T> const & b)
	{
		return a.get() == b.get();
	}

	template<class T> inline bool operator==(Handle<T> const & a, Hndl const & b)
	{
		return a.get() == b.get();
	}

	template<class T> inline bool operator!=(Hndl const & a, Handle<T> const & b)
	{
		return a.get() != b.get();
	}

	template<class T> inline bool operator!=(Handle<T> const & a, Hndl const & b)
	{
		return a.get() != b.get();
	}

	template<class T, class U> inline bool operator==(Handle<T> const & a, Handle<U> const & b)
	{
		return a.get() == b.get();
	}

	template<class T, class U> inline bool operator!=(Handle<T> const & a, Handle<U> const & b)
	{
		return a.get() != b.get();
	}

	template<class T> inline bool operator==(Handle<T> const & a, T * b)
	{
		return a.get() == b;
	}

	template<class T> inline bool operator!=(Handle<T> const & a, T * b)
	{
		return a.get() != b;
	}

	template<class T> inline bool operator==(T * a, Handle<T> const & b)
	{
		return a == b.get();
	}

	template<class T> inline bool operator!=(T * a, Handle<T> const & b)
	{
		return a != b.get();
	}

	template<class T> inline bool operator<(Handle<T> const & a, Handle<T> const & b)
	{
		return std::less<const T *>()(a.get(), b.get());
	}

	// mem_fn support

	template<class T> Handle<T> static_pointer_cast(Hndl const & p)
	{
		return static_cast<T *>(p.get());
	}

	template<class T> Handle<T> dynamic_pointer_cast(Hndl const & p)
	{
		return GCTP_DYNCAST<T *>(p.get());
	}

	template<class T> T * get_pointer(Handle<T> const & p)
	{
		return p.get();
	}

	template<class T, class U> Handle<T> static_pointer_cast(Handle<U> const & p)
	{
		return static_cast<T *>(p.get());
	}

	template<class T, class U> Handle<T> dynamic_pointer_cast(Handle<U> const & p)
	{
		return GCTP_DYNCAST<T *>(p.get());
	}

	// operator<<

	template<class E, class T, class Y> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Handle<Y> const & p)
	{
		os << p.get();
		return os;
	}

} // namespace gctp

#endif //_GCTP_POINTER_HPP_
