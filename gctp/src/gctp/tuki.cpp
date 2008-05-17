/** @file
 * LuaのC++ラッパー
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/tuki.hpp>
#include <gctp/stringmap.hpp>
#include <gctp/typemap.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/serializer.hpp>
#include <gctp/bfstream.hpp>
#include <boost/tokenizer.hpp>

using namespace std;

namespace gctp {

	namespace {

		const char *UNKNOWNNAME = "gctp.Object";
		typedef void (*RegisterFunc)(lua_State *);
		typedef StaticStringMap<RegisterFunc> TukiMap;
		typedef TypeMap<const char *> TukiTypeMap;
		
		static TukiMap &registry()
		{
			static TukiMap registry;
			return registry;
		}
		static TukiTypeMap &typeRegistry()
		{
			static TukiTypeMap registry;
			return registry;
		}

	}

	/** luapp::Table用に特殊化したシリアライズ演算子
	 */
	template<>
	Serializer &operator<<(Serializer &lhs, luapp::Table &rhs)
	{
		if(lhs.isLoading()) {
			for(;;) {
				// キー
				luapp::Key key;
				uchar type;
				lhs.istream() >> type;
				if(type == luapp::NIL) break;
				std::string str;
				key.type = static_cast<luapp::Type>(type);
				switch(key.type) {
				case luapp::BOOLEAN: lhs.istream() >> key.boolean; break;
				case luapp::NUMBER: lhs.istream() >> key.number; break;
				case luapp::STRING:{
					lhs.istream() >> str;
					key.string = str.c_str();
								   }
					break;
				//case luapp::FUNCTION: os << "FUNCTION:" << (void *)v.toCFunction(); break;
				//case luapp::THREAD: os << "THREAD:" << (void *)v.toThread(); break;
				//case luapp::USERDATA:
				//case luapp::LIGHTUSERDATA:
				//	os << "USERDATA:" << v.toUserData();
				default: continue;
				}

				// 値
				lhs.istream() >> type;
				switch(type) {
				case luapp::TABLE:{
					// テーブルを挿入
					luapp::Table t = rhs.newTable(key);
					lhs << t;
								  }
					break;
				case luapp::BOOLEAN:{
					bool b = false;
					lhs.istream() >> b;
					// テーブルに挿入
					rhs.declare(key, b);
									}
					break;
				case luapp::NUMBER:{
					lua_Number n;
					lhs.istream() >> n;
					// テーブルに挿入
					rhs.declare(key, n);
								   }
					break;
				case luapp::STRING:{
					std::string str;
					lhs.istream() >> str;
					// テーブルに挿入
					rhs.declare(key, str.c_str());
								   }
					break;
				//case luapp::FUNCTION: os << "FUNCTION:" << (void *)v.toCFunction(); break;
				//case luapp::THREAD: os << "THREAD:" << (void *)v.toThread(); break;
				//case luapp::USERDATA:
				//case luapp::LIGHTUSERDATA:
				}
			}
		}
		else if(lhs.isWriting()) {
			luapp::Table::AltIterator i(rhs);
			while(i.next()) {
				// キー
				uchar type = static_cast<uchar>(i.key().type());
				lhs.ostream() << type;
				switch(i.key().type()) {
				case luapp::BOOLEAN: lhs.ostream() << i.key().toBoolean(); break;
				case luapp::NUMBER: lhs.ostream() << i.key().toNumber(); break;
				case luapp::STRING: lhs.ostream() << i.key().toCStr(); break;
				//case luapp::FUNCTION: os << "FUNCTION:" << (void *)v.toCFunction(); break;
				//case luapp::THREAD: os << "THREAD:" << (void *)v.toThread(); break;
				//case luapp::USERDATA:
				//case luapp::LIGHTUSERDATA:
				//	os << "USERDATA:" << v.toUserData();
				default: continue;
				}

				// 値
				type = static_cast<uchar>(i.value().type());
				switch(type) {
				//case luapp::NIL: os << "NIL"; break;
				case luapp::TABLE:
					lhs.ostream() << type;
					{
						luapp::Table t = i.value().toTable();
						lhs << t;
					}
					break;
				case luapp::BOOLEAN: lhs.ostream() << type << i.value().toBoolean(); break;
				case luapp::NUMBER: lhs.ostream() << type << i.value().toNumber(); break;
				case luapp::STRING: lhs.ostream() << type << i.value().toCStr(); break;
				//case luapp::FUNCTION: os << "FUNCTION:" << (void *)v.toCFunction(); break;
				//case luapp::THREAD: os << "THREAD:" << (void *)v.toThread(); break;
				//case luapp::USERDATA:
				//case luapp::LIGHTUSERDATA:
				//	os << "USERDATA:" << v.toUserData();
				}
			}
			lhs.ostream() << static_cast<uchar>(luapp::NIL);
		}
		return lhs;
	}

	TukiRegister::TukiRegister(const char *luaname, const GCTP_TYPEINFO &type, void (*register_func)(lua_State *L))
	{
		registry().put(luaname, register_func);
		typeRegistry().put(type, luaname);
	}

	TukiRegister::TukiRegister(const char *luaname, void (*register_func)(lua_State *L))
	{
		registry().put(luaname, register_func);
	}

	void TukiRegister::useTuki()
	{
	}

	int TukiRegister::luaRegisterPackage(lua_State *l)
	{
		std::string str = luapp::State(l)[1].toString();
		str += ". ";
		str.at(str.length()-1) = static_cast<char>(1);
		TukiMap::iterator i = registry().lower_bound(str.c_str());
		str.at(str.length()-1) = static_cast<char>(-1);
		TukiMap::iterator end = registry().upper_bound(str.c_str());
		for(; i != end; ++i) {
			RegisterFunc f = (RegisterFunc)(i->second);
			if(f) f(l);
		}
		return 0;
	}

	void TukiRegister::registerIt(lua_State *l, const char *name)
	{
		RegisterFunc f = registry().get(name);
		if(f) f(l);
		
		std::string str(name);
		str += ". ";
		str.at(str.length()-1) = static_cast<char>(1);
		TukiMap::iterator i = registry().lower_bound(str.c_str());
		str.at(str.length()-1) = static_cast<char>(-1);
		TukiMap::iterator end = registry().upper_bound(str.c_str());
		for(; i != end; ++i) {
			f = (RegisterFunc)(i->second);
			f(l);
		}
	}

	void TukiRegister::registerIt(lua_State *l, const GCTP_TYPEINFO &type)
	{
		const char *luaname = typeRegistry().get(type);
		if(luaname) {
			RegisterFunc f = registry().get(luaname);
			if(f) f(l);
		}
	}

	/// 型不明の状態から、Luaにさらす
	int TukiRegister::push(lua_State *l, Hndl hobj)
	{
		if(hobj) {
			const char *luaname = typeRegistry().get(GCTP_TYPEID(*hobj));
			if(!luaname) luaname = UNKNOWNNAME;
			UserData *ud = new(lua_newuserdata(l, sizeof(UserData))) UserData;
			if(ud) {
				ud->h = hobj;  // store pointer to object in userdata
				luaL_getmetatable(l, luaname);  // lookup metatable in Lua registry
				lua_setmetatable(l, -2);
				return 1;  // userdata containing pointer to T object
			}
		}
		return 0;
	}

	int TukiRegister::luaRegister(lua_State *l)
	{
		luapp::Stack L(l);
		const char *name = L[1].toCStr();
		registerIt(l, name);
		return 0;
	}

	int TukiRegister::luaTableDump(lua_State *l)
	{
		luapp::Stack L(l);
		if(L[1].isString() && L[2].isTable()) {
			obfstream f(L[1].toCStr());
			Serializer serializer(f);
			serializer.ostream() << 'DAUL';
			serializer << L[2].toTable();
		}
		return 0;
	}

	int TukiRegister::luaTableLoad(lua_State *l)
	{
		luapp::Stack L(l);
		if(L[1].isString() && L[2].isTable()) {
			ibfstream f(L[1].toCStr());
			Serializer serializer(f);
			if(serializer.isOpen()) {
				int32_t magic;
				serializer.istream() >> magic;
				if(magic == 'DAUL') {
					serializer << L[2].toTable();
				}
			}
		}
		return 0;
	}

	namespace {
		void setBatchLoaderToPreload(luapp::Stack &L, luapp::Table &preload, std::string &name)
		{
			std::string::size_type n = name.rfind(".");
			if(n != std::string::npos) {
				name.erase(n);
				if(preload[name.c_str()].isNil()) preload.declare(name.c_str(), TukiRegister::luaRegisterPackage);
				setBatchLoaderToPreload(L, preload, name);
			}
		}
	}

	void TukiRegister::registerMe(lua_State *l)
	{
		luapp::Stack L(l);
		luapp::Table preload = L.global()["package"]["preload"];
		for(TukiMap::const_iterator i = registry().begin(); i != registry().end(); ++i)
		{
			preload.declare(i->first.c_str(), i->second);
			std::string name = i->first.c_str();
			setBatchLoaderToPreload(L, preload, name);
		}
		// 汎用メタテーブル定義
		TukiRegister::registerObject(l, UNKNOWNNAME, 0, toString, 0);
		//lua_register(L, "require", luaRegister); // 標準のrequireを置き換えてしまう...ってのもありかなぁ（特にコンシューマ）
		lua_register(L, "tuki_dump", luaTableDump);
		lua_register(L, "tuki_load", luaTableLoad);
	}

	int TukiRegister::registerTable(lua_State *L, const char *name)
	{
		int ret = LUA_GLOBALSINDEX;
		std::string str(name);
		boost::char_separator<char> sep("/.");
		boost::tokenizer< boost::char_separator<char> > tok(str, sep);
		lua_checkstack(L, 3);
		for(boost::tokenizer< boost::char_separator<char> >::iterator i = tok.begin(); i != tok.end(); ++i) {
			// グローバルへ登録
			lua_pushstring(L, i->c_str());
			lua_gettable(L, ret);  /* check whether lib already exists */
			if(lua_isnil(L, -1)) {  /* no? */
				lua_pop(L, 1);
				lua_newtable(L);  /* create it */
				lua_pushstring(L, i->c_str());
				lua_pushvalue(L, -2);
				lua_settable(L, ret);  /* register it with given name */
			}
			if(ret > 0) {
				lua_insert(L, -2);
				lua_pop(L, 1);
			}
			ret = lua_gettop(L);
		}
		return ret;
	}

	void TukiRegister::registerFuncs(lua_State *L, const char *name, const Func *funcs)
	{
		if(funcs && funcs->name) {
			int methods = registerTable(L, name);
			lua_checkstack(L, 2);
			while(funcs->name) {
				lua_pushstring(L, funcs->name);
				lua_pushcfunction(L, (lua_CFunction)funcs->func);
				lua_settable(L, methods);
				funcs++;
			}
		}
	}
	
	void TukiRegister::registerObject(lua_State *L
			, const char *name
			, const lua_CFunction f_new
			, const lua_CFunction f_tostring
			, const MemberRegisterer reg_mem)
	{
		int methods = registerTable(L, name);

		lua_checkstack(L, 6);
		luaL_newmetatable(L, name);
		int metatable = lua_gettop(L);

		lua_pushliteral(L, "__metatable");
		lua_pushvalue(L, methods);
		lua_settable(L, metatable);  // hide metatable from Lua getmetatable()

		lua_pushliteral(L, "__index");
		lua_pushvalue(L, methods);
		lua_settable(L, metatable);

		lua_pushliteral(L, "__tostring");
		lua_pushcfunction(L, f_tostring);
		lua_settable(L, metatable);

		lua_pushliteral(L, "__gc");
		lua_pushcfunction(L, &TukiRegister::deleteThis);
		lua_settable(L, metatable);
		// add same method to method table as "delete"
		lua_pushliteral(L, "delete");
		lua_pushcfunction(L, &TukiRegister::deleteThis);
		lua_settable(L, methods);
		
		lua_newtable(L);                // mt for method table
		if(f_new) {
			int mt = lua_gettop(L);
			lua_pushliteral(L, "__call");
			lua_pushcfunction(L, f_new);
			lua_pushliteral(L, "new");
			lua_pushvalue(L, -2);		// dup new_T function
			lua_settable(L, methods);	// add new_T to method table
			lua_settable(L, mt);		// mt.__call = new_T
		}
		lua_setmetatable(L, methods);

		// fill method table with methods from class T
		if(reg_mem) reg_mem(L, methods);

		lua_pop(L, 2);  // drop metatable and method table
	}

	// garbage collection metamethod
	int TukiRegister::deleteThis(lua_State *L)
	{
		UserData *ud = static_cast<UserData *>(lua_touserdata(L, 1));
		if(ud) {
			// これをキーしたレジストリエントリがあったら削除
			void *p = ud->p.get();
			if(!p) p = ud->h.get();
			lua_checkstack(L, 2);
			lua_pushlightuserdata(L, p);
			lua_gettable(L, LUA_REGISTRYINDEX);
			if(!lua_isnil(L, -1)) {
				lua_pop(L, 1);
				lua_pushlightuserdata(L, p);
				lua_pushnil(L);
				lua_settable(L, LUA_REGISTRYINDEX);
			}
			else lua_pop(L, 1);
			ud->h = 0;
			ud->p = 0;  // call destructor for T objects
		}
		else {
			luaL_typerror(L, 1, "TukiObject");
		}
		return 0;
	}

	Object *TukiRegister::check(lua_State *L, int iud)
	{
		lua_checkstack(L, 2);
		if(!lua_getmetatable(L, iud)) return 0;  /* no metatable? */
		lua_pushliteral(L, "__gc");
		lua_gettable(L, -2);
		if(lua_iscfunction(L, -1) && lua_tocfunction(L, -1) == &TukiRegister::deleteThis) {
			lua_pop(L, 2);
			// tukiオブジェクトの証
			UserData *ud = static_cast<UserData *>(lua_touserdata(L, iud));
			if(ud) {
				if(ud->p) return ud->p.get();  // pointer to T object
				if(ud->h) return ud->h.get();  // pointer to T object
			}
		}
		else lua_pop(L, 2);
		return 0;
	}

	int TukiRegister::toString(lua_State *L)
	{
		Object *p = check(L, 1);
		if(p) {
			char buff[256];
			sprintf(buff, "%p", p);
			lua_checkstack(L, 1);
			lua_pushfstring(L, "%s (%s)", UNKNOWNNAME, buff);
			return 1;
		}
		return 0;
	}

} // namespace gctp
