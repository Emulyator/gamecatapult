#ifndef _GCTP_TUKI_HPP_
#define _GCTP_TUKI_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file
 * LuaのC++ラッパー
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/06/26 2:43:16
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <string>
#include <vector>
#include <utility>

#include <lua.hpp>

#include <gctp/pointer.hpp>
#include <gctp/typeinfo.hpp>
#include <gctp/serializer.hpp>
#include <boost/tokenizer.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

namespace gctp {

	/** Tuki情報をデータベース登録するオブジェクト
	 *
	 * register機構を提供する。
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/06/26 3:39:00
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class TukiRegister {
	public:
		typedef void (*MemberRegisterer)(lua_State *L, int methods);
		struct Func {
			const char *name;
			lua_CFunction func;
		};
		// Userdataとして制作される型
		struct UserData {
			Ptr p;
			Hndl h;
		};
		TukiRegister(const char *luaname, const GCTP_TYPEINFO &type, void (*register_func)(lua_State *L));
		TukiRegister(const char *luaname, void (*register_func)(lua_State *L));
		void useTuki();
		static void registerIt(lua_State *l, const char *name);
		static void registerIt(lua_State *l, const GCTP_TYPEINFO &type);
		static int push(lua_State *l, Hndl hobj);
		static int newUserData(lua_State *l, Ptr pobj);
		static int luaRegisterPackage(lua_State *L);
		static int luaRegister(lua_State *L);
		static int luaTableDump(lua_State *L);
		static int luaTableLoad(lua_State *L);
		static void registerMe(lua_State *L);
		static int registerTable(lua_State *L, const char *name);
		static void registerFuncs(lua_State *L, const char *name, const Func *funcs);
		static void registerObject(lua_State *L, const char *name, const lua_CFunction f_new, const lua_CFunction f_tostring, const MemberRegisterer reg_mem);
		// garbage collection metamethod
		static int dispose(lua_State *L);
		// Tukiオブジェクトかどうか検査
		static Object *check(lua_State *L, int narg);
	private:
		// 汎用メタテーブル構築用
		static int toString(lua_State *L);
	};

	/** Tukiのregister機能とtuki_dump/tuki_loadを登録するInitializer
	 *
	 * 典型的には、
	 * @code
 luapp::State L(TukiInitializer());
 	 * @endcode
 	 * とする。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2003/09/21 3:02:48
	 */
	class TukiInitializer {
	public:
		void operator()(lua_State *lua) const
		{
			luaL_openlibs(lua);
			TukiRegister::registerMe(lua);
		}
	};

	/** A template class for binding C++ to Lua
	 *
	 * LunaをGameCatapult用にカスタマイズ
	 * @author Lenny Palozzi
	 * @author SAM
	 * @date 2003/07/02 4:36:08
	 *
	 * Copyright (C) 2003 Lenny Palozzi. All rights reserved.
	 * Copyright (C) 2005 SAM. All rights reserved.
	 */
	template <typename T>
	class Tuki {
	public:
		typedef int (T::*MemberFunction)(luapp::Stack &);
		typedef void (T::*MemberFunctionSelfReturn)(luapp::Stack &);
		struct Method {
			enum Type {
				NONE,
				MEMBER,
				SELF,
				STATIC,
			};
			Type type_;
			const char *name_;
			union {
				MemberFunction				mfunc_;
				MemberFunctionSelfReturn	selffunc_;
				luapp::Function				sfunc_;
			};
			Method(const char *name, MemberFunction method) : type_(MEMBER), name_(name), mfunc_(method) {}
			Method(const char *name, MemberFunctionSelfReturn method) : type_(SELF), name_(name), selffunc_(method) {}
			Method(const char *name, luapp::Function method) : type_(STATIC), name_(name), sfunc_(method) {}
			Method() : type_(NONE), name_(0), sfunc_(0) {}
		};
		// Userdataとして制作される型
		struct UserData : TukiRegister::UserData {
			Pointer<T> &pointer() { return reinterpret_cast<Pointer<T> &>(p); }
			Handle<T> &handle() { return reinterpret_cast<Handle<T> &>(h); }
		};

		static void registerMe(lua_State *L)
		{
			TukiRegister::registerObject(L, T::lua_name__, newThis, toString, registerMember);
		}

		// get userdata from Lua stack and return pointer to T object
		static T *check(lua_State *L, int narg) {
			UserData *ud = static_cast<UserData *>(luaL_checkudata(L, narg, T::lua_name__));
			if(ud) {
				if(ud->p) return ud->pointer().get();  // pointer to T object
				if(ud->h) return ud->handle().get();  // pointer to T object
			}
			return 0;
		}

	private:
		Tuki();  // hide default constructor

		// member function dispatcher
		static int thunk(lua_State *L)
		{
			// stack has userdata, followed by method args
			T *obj = check(L, 1);  // get 'self', or if you prefer, 'this'
			if(obj) {
				lua_remove(L, 1);  // remove self so member function args start at index 1
				// get member function from upvalue
				Method *l = static_cast<Method *>(lua_touserdata(L, lua_upvalueindex(1)));
				luapp::Stack ls(L);
				return (obj->*(l->mfunc_))(ls);  // call member function
			}
			else luaL_typerror(L, 1, T::lua_name__);
			return 0;
		}

		// member function dispatcher without removing self from stack
		static int altThunk(lua_State *L)
		{
			// stack has userdata, followed by method args
			T *obj = check(L, 1);  // get 'self', or if you prefer, 'this'
			if(obj) {
				// move self to registry
				lua_pushlightuserdata(L, (void *)altThunk);
				lua_pushvalue(L, 1);
				lua_settable(L, LUA_REGISTRYINDEX);
				lua_remove(L, 1);
				// get member function from upvalue
				Method *l = static_cast<Method *>(lua_touserdata(L, lua_upvalueindex(1)));
				luapp::Stack ls(L);
				(obj->*(l->selffunc_))(ls);  // call member function
				// push self to stack top
				lua_pushlightuserdata(L, (void *)altThunk);
				lua_gettable(L, LUA_REGISTRYINDEX);
				lua_insert(L, 1);
				// remove self from registry
				lua_pushlightuserdata(L, (void *)altThunk);
				lua_pushnil(L);
				lua_settable(L, LUA_REGISTRYINDEX);
				lua_settop(L, 1);
				return 1;
			}
			else luaL_typerror(L, 1, T::lua_name__);
			return 0;
		}

		// create a new T object and
		// push onto the Lua stack a userdata containing a pointer to T object
		static int newThis(lua_State *L)
		{
			lua_remove(L, 1);   // use classname:new(), instead of classname.new()
			Pointer<T> p = Factory::create(GCTP_TYPEID(T));  // call constructor for T objects
			luapp::Stack lua(L);
			if(p && p->LuaCtor(lua)) {
				UserData *ud = new (lua_newuserdata(L, sizeof(UserData))) UserData;
				if(ud) {
					ud->pointer() = p;  // store pointer to object in userdata
					luaL_getmetatable(L, T::lua_name__);  // lookup metatable in Lua registry
					lua_setmetatable(L, -2);
					return 1;  // userdata containing pointer to T object
				}
			}
			return 0;
		}

		static int toString(lua_State *L)
		{
			T *p = check(L, 1);
			if(p) {
				char buff[256];
				sprintf(buff, "%p", p);
				lua_checkstack(L, 1);
				lua_pushfstring(L, "%s (%s)", T::lua_name__, buff);
				return 1;
			}
			return 0;
		}
		
		static void registerMember(lua_State *L, int methods)
		{
			lua_checkstack(L, 2);
			// fill method table with methods from class T
			for(Method *l = T::lua_methods__; l->name_; l++) {
				lua_pushstring(L, l->name_);
				if(l->type_ == Method::MEMBER) {
					lua_pushlightuserdata(L, (void*)l);
					lua_pushcclosure(L, thunk, 1);
				}
				else if(l->type_ == Method::SELF) {
					lua_pushlightuserdata(L, (void*)l);
					lua_pushcclosure(L, altThunk, 1);
				}
				else {
					lua_pushlightuserdata(L, (void*)l->sfunc_);
					lua_pushcclosure(L, luapp::Stack::thunk, 1);
				}
				lua_settable(L, methods);
			}
		}
	};

	/// UserDataをgctp::Object *に変換
	inline Object *toObject(const luapp::Value &val) { return val.check(gctp::TukiRegister::check); }
	
	/** UserDataのキャスト演算子
	 *
	 * 指定した型のポインター型で返る点に注意。
	 * @code
	   Pointer<Foo> p = tuki_cast<Foo>(l[1]);
	 * @endcode
	 */
	template<typename T> Pointer<T> tuki_cast(const luapp::Value &val) {
		return Pointer<T>(toObject(val));
	}

	/** luapp::Table用に特殊化したシリアライズ演算子
	 */
	template<>
	Serializer &operator<<(Serializer &lhs, luapp::Table &rhs);

} // namespace gctp

/** GameCatapultオブジェクトをluaオブジェクトへバインドするマクロ
 *
 * アクセス指定子が含まれていることに注意。このマクロの直後はprivateになる。
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/07/06 5:05:09
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define TUKI_DECLARE(T)											\
	public:														\
		static void useTuki() { return lua_reg__.useTuki(); }	\
	private:													\
		friend class gctp::Tuki<T>;								\
		static const char *lua_name__;							\
		static gctp::TukiRegister lua_reg__;					\
		static gctp::Tuki<T>::Method lua_methods__[];

#define TUKI_IMPLEMENT_BEGIN_EX(name, T)										\
	const char *T::lua_name__ = name;											\
	gctp::TukiRegister T::lua_reg__(T::lua_name__, GCTP_TYPEID(T), gctp::Tuki<T>::registerMe);	\
	gctp::Tuki<T>::Method T::lua_methods__[] = {

#define TUKI_IMPLEMENT_BEGIN(T)	TUKI_IMPLEMENT_BEGIN_EX(#T, T)

#define TUKI_IMPLEMENT_BEGIN_NS(ns, T)	TUKI_IMPLEMENT_BEGIN_EX(#ns"."#T, T)

#define TUKI_IMPLEMENT_BEGIN_NS2(ns1, ns2, T)	TUKI_IMPLEMENT_BEGIN_EX(#ns1"."#ns2"."#T, T)

#define TUKI_IMPLEMENT_BEGIN_NS3(ns1, ns2, ns3, T)	TUKI_IMPLEMENT_BEGIN_EX(#ns1"."#ns2"."#ns3"."#T, T)

#define TUKI_METHOD_EX2(T, name, B, method)	\
	gctp::Tuki<T>::Method(name, &B::method),

#define TUKI_METHOD_EX(T, name, method)	TUKI_METHOD_EX2(T, name, T, method)

#define TUKI_METHOD2(T, B, name)		TUKI_METHOD_EX2(T, #name, B, name)

#define TUKI_METHOD(T, name)			TUKI_METHOD_EX(T, #name, name)

#define TUKI_IMPLEMENT_END(T)	\
	gctp::Tuki<T>::Method() };

#define GCTP_USE_TUKI(_classname)	{ _classname::useTuki(); GCTP_USE_CLASS(_classname); }

#define TUKI_DECLARE_FUNCS(ns)	namespace ns { void useTuki(); }

#define TUKI_DECLARE_FUNCS2_(ns1, ns2, s)	TUKI_DECLARE_FUNCS(ns1##s##ns2)
#define TUKI_DECLARE_FUNCS2(ns1, ns2)	TUKI_DECLARE_FUNCS2_(ns1, ns2, _)

#define TUKI_DECLARE_FUNCS3_(ns1, ns2, ns3, s)	TUKI_DECLARE_FUNCS(ns1##s##ns2##s##ns3)
#define TUKI_DECLARE_FUNCS3(ns1, ns2, ns3)	TUKI_DECLARE_FUNCS(ns1, ns2, ns3, _)

#define TUKI_FUNCS_BEGIN_EX(ns, name)									\
	namespace ns {														\
		extern gctp::TukiRegister::Func tuki_funcs__[];					\
		static void registerMe(lua_State *L) { 							\
			gctp::TukiRegister::registerFuncs(L, name, tuki_funcs__);	\
		}																\
		static gctp::TukiRegister lua_reg__(name, registerMe);			\
		void useTuki() { lua_reg__.useTuki(); }							\
		static gctp::TukiRegister::Func tuki_funcs__[] = {

#define TUKI_FUNCS_BEGIN(ns)	TUKI_FUNCS_BEGIN_EX(ns, #ns)

#define TUKI_FUNCS_BEGIN2(ns1, ns2)	TUKI_FUNCS_BEGIN_EX(ns1##_##ns2, #ns1"."#ns2)

#define TUKI_FUNCS_BEGIN3(ns1, ns2, ns3)	TUKI_FUNCS_BEGIN_EX(ns1##_##ns2##_##ns3, #ns1"."#ns2"."#ns3)

#define TUKI_FUNC_EX(name, func)	{name, func},

#define TUKI_FUNC(func)	TUKI_FUNC_EX(#func, func)

#define TUKI_FUNCS_END()	\
		{0, 0} };			\
	}

#define GCTP_USE_TUKI_FUNCS(ns)	{ ns::useTuki(); }

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //_GCTP_TUKI_HPP_