/** @file
 * GameCatapult コンテキストクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/context.hpp>
#include <gctp/serializer.hpp>
#include <gctp/fileserver.hpp>
#include <gctp/turi.hpp>
#include <gctp/db.hpp>
#include <gctp/cstr.hpp>

using namespace std;

namespace gctp { namespace core {

	/** コンテキストの使用開始を宣言
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/19 2:07:06
	 */
	void Context::push()
	{
		prev_ = current__;
		current__ = this;
		is_pushed_ = true;
	}

	/** コンテキストの使用終了を宣言
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/19 2:07:06
	 */
	void Context::pop()
	{
		if(is_pushed_) {
			is_pushed_ = false;
			GCTP_ASSERT(current__ == this);
			if(prev_) current__ = prev_.get();
			else current__ = parent_.get();
		}
	}

	/** 要求されたリソースをロードし、保持する
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::load(const _TCHAR *name)
	{
		//GCTP_ASSERT(current_ == this); これ必要か？
		if(name) {
			Hndl h = find(name);
			if(h) return h;
			TURI uri(name);
//#ifdef _WIN32
//			uri.convertLower();
//#endif
			std::basic_string<_TCHAR> ext = uri.extension();
			RealizeMethod f = Extension::get(ext.c_str());
			if(f) {
				BufferPtr file = fileserver().getFile(uri.raw().c_str());
				if(file) {
					Context *backup = current__; // 一時的にカレントを差し替える
					current__ = this;
					loading_async_ = false;
					Ptr p = f(0, file);
					current__ = backup;
					if(p) {
						ptrs_.push_back(p);
						db_.insert(name, p);
						return p;
					}
				}
				else {
					dbgout << _T("要求されたりソース\"") << name << _T("\"は見つかりませんでした") << endl;
				}
			}
			else {
				dbgout << _T("Context::load : 拡張子'")<<uri.extension()<<_T("'のリアライザは登録されていない(")<<uri.raw()<<_T("の読み込み時)") << endl;
			}
		}
		return Hndl();
	}

	/** 要求されたリソースをロードし、保持する（リアライザ指定版）
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::load(const _TCHAR *name, RealizeMethod realizer)
	{
		//GCTP_ASSERT(current_ == this); これ必要か？
		if(name && realizer) {
			Hndl h = find(name);
			if(h) return h;
			TURI uri(name);
//#ifdef _WIN32
//			uri.convertLower();
//#endif
			BufferPtr file = fileserver().getFile(uri.raw().c_str());
			if(file) {
				Context *backup = current__; // 一時的にカレントを差し替える
				current__ = this;
				loading_async_ = false;
				Ptr p = realizer(0, file);
				current__ = backup;
				if(p) {
					ptrs_.push_back(p);
					db_.insert(name, p);
					return p;
				}
			}
			else {
				dbgout << _T("要求されたりソース\"") << name << _T("\"は見つかりませんでした") << endl;
			}
		}
		return Hndl();
	}

	/** 要求されたリソースをロードし、保持する(非同期版)
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::loadAsync(const _TCHAR *name)
	{
		//GCTP_ASSERT(current_ == this); これ必要か？
		if(name) {
			synchronize(true);
			Hndl h = find(name);
			if(h) return h;
			TURI uri(name);
//#ifdef _WIN32
//			uri.convertLower();
//#endif
			std::basic_string<_TCHAR> ext = uri.extension();
			RealizeMethod f = Extension::get(ext.c_str());
			if(f) {
				while(fileserver().busy()) {
					fileserver().service(0);
				}
				AsyncBufferPtr file = fileserver().getFileAsync(uri.raw().c_str());
				if(file) {
					Ptr p = f(0, 0);
					if(p) {
						file->setRealizer(f);
						file->connect(on_ready_slot);
						ptrs_.push_back(p);
						db_.insert(name, p);
						return p;
					}
				}
				else {
					dbgout << _T("要求されたりソース\"") << name << _T("\"は見つかりませんでした") << endl;
				}
			}
			else {
				dbgout << _T("Context::load : 拡張子'")<<uri.extension()<<_T("'のリアライザは登録されていない(")<<uri.raw()<<_T("の読み込み時)") << endl;
			}
		}
		return Hndl();
	}

	/** 要求されたリソースをロードし、保持する(非同期リアライザ指定版)
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::loadAsync(const _TCHAR *name, RealizeMethod realizer)
	{
		//GCTP_ASSERT(current_ == this); これ必要か？
		if(name && realizer) {
			synchronize(true);
			Hndl h = find(name);
			if(h) return h;
			TURI uri(name);
//#ifdef _WIN32
//			uri.convertLower();
//#endif
			while(fileserver().busy()) {
				fileserver().service(0);
			}
			AsyncBufferPtr file = fileserver().getFileAsync(uri.raw().c_str());
			if(file) {
				Ptr p = realizer(0, 0);
				if(p) {
					file->setRealizer(realizer);
					file->connect(on_ready_slot);
					ptrs_.push_back(p);
					db_.insert(name, p);
					return p;
				}
			}
			else {
				dbgout << _T("要求されたりソース\"") << name << _T("\"は見つかりませんでした") << endl;
			}
		}
		return Hndl();
	}

	/** 渡されたオブジェクトを保持する
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void Context::add(const Ptr ptr)
	{
		if(ptr) ptrs_.push_back(ptr);
	}

	/** 渡されたオブジェクトを保持する
	 * 
	 * これはその他のcreate系と異なり、既存のエントリを上書きする
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::insert(const Ptr ptr, const _TCHAR *name)
	{
		if(ptr) {
			ptrs_.push_back(ptr);
			if(name) db_.insert(name, ptr);
		}
		return ptr;
	}

	/** 要求されたオブジェクトを製作し、保持する
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::create(const GCTP_TYPEINFO &typeinfo)
	{
		Context *backup = current__; // 一時的にカレントを差し替える
		current__ = this;
		Ptr ret = Factory::create(typeinfo);
		current__ = backup;
		if(ret) ptrs_.push_back(ret);
		return ret;
	}

	/** 要求されたオブジェクトを製作し、保持する
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::create(const char *classname)
	{
		Context *backup = current__; // 一時的にカレントを差し替える
		current__ = this;
		Ptr ret = Factory::create(classname);
		current__ = backup;
		if(ret) ptrs_.push_back(ret);
		return ret;
	}

	Hndl Context::create(const GCTP_TYPEINFO &typeinfo, const _TCHAR *name)
	{
		Hndl ret;
		if(name) {
			ret = db_[name];
			if(!ret) {
				ret = create(typeinfo);
				if(ret && name) db_.insert(name, ret);
			}
		}
		else ret = create(typeinfo);
		return ret;
	}

	Hndl Context::create(const char *classname, const _TCHAR *name)
	{
		Hndl ret;
		if(name) {
			ret = db_[name];
			if(!ret) {
				ret = create(classname);
				if(ret && name) db_.insert(name, ret);
			}
		}
		else ret = create(classname);
		return ret;
	}

	Hndl Context::find(const _TCHAR *name)
	{
		if(name) {
			Hndl ret = db_[name];
			if(ret) return ret;
			else if(parent_) return parent_->find(name);
		}
		return Hndl();
	}

	bool Context::onReady(const _TCHAR *name, RealizeMethod realizer, BufferPtr buffer)
	{
		if(realizer) {
			Context *backup = current__; // 一時的にカレントを差し替える
			current__ = this;
			loading_async_ = true;
			Ptr rsrc = find(name).lock();
			if(rsrc) {
				Ptr p = realizer(rsrc, buffer);
				if(!p) {
					db_.erase(name);
					ptrs_.remove(rsrc);
				}
			}
			current__ = backup;
		}
		return false;
	}

	void Context::serialize(Serializer &serializer)
	{
		Context *backup = current__; // 一時的にカレントを差し替える
		current__ = this;
		if(serializer.isLoading()) {
			int size;
			serializer.istream() >> size;
			for(int i = 0; i < size; i++) {
				Ptr ptr;
				serializer << ptr;
				ptrs_.push_back(ptr);
			}
		}
		else if(serializer.isWriting()) {
			int size = (int)ptrs_.size();
			serializer.ostream() << size;
			for(PtrList::iterator i = ptrs_.begin(); i != ptrs_.end(); ++i) {
				serializer << *i;
			}
		}
		current__ = backup;
	}

	Context *Context::current__ = NULL;

	bool Context::setUp(luapp::Stack &L)
	{
		// gctp.core.Context.current().newChild()経由でなければ作れない
		return false;
	}

	int Context::newChild(luapp::Stack &L)
	{
		gctp::TukiRegister::newUserData(L, newChild());
		return 1;
	}

	int Context::load(luapp::Stack &L)
	{
		/* (const char *fname) */
		Hndl ret;
		if(L.top() >= 2) {
			RealizeMethod f = Extension::get(WCStr(L[2].toCStr()).c_str());
			if(f) {
#ifdef UNICODE
				ret = load(WCStr(L[1].toCStr()).c_str(), f);
#else
				ret = load(L[1].toCStr(), f);
#endif
			}
			else {
				dbgout << _T("Context::load : 拡張子'")<<L[2].toCStr()<<_T("'のリアライザは登録されていない(")<<L[1].toCStr()<<_T("の読み込み時)") << endl;
			}
		}
		else if(L.top() >= 1) {
#ifdef UNICODE
			ret = load(WCStr(L[1].toCStr()).c_str());
#else
			ret = load(L[1].toCStr());
#endif
		}
		if(ret) return gctp::TukiRegister::push(L, ret);
		return 0;
	}
	
	int Context::loadAsync(luapp::Stack &L)
	{
		/* (const char *fname) */
		Hndl ret;
		if(L.top() >= 2) {
			RealizeMethod f = Extension::get(WCStr(L[2].toCStr()).c_str());
			if(f) {
#ifdef UNICODE
				ret = loadAsync(WCStr(L[1].toCStr()).c_str(), f);
#else
				ret = loadAsync(L[1].toCStr(), f);
#endif
			}
			else {
				dbgout << _T("Context::load : 拡張子'")<<L[2].toCStr()<<_T("'のリアライザは登録されていない(")<<L[1].toCStr()<<_T("の読み込み時)") << endl;
			}
		}
		else if(L.top() >= 1) {
#ifdef UNICODE
			L << loadAsync(WCStr(L[1].toCStr()).c_str());
#else
			L << loadAsync(L[1].toCStr());
#endif
			return 1;
		}
		return 0;
	}
	
	int Context::create(luapp::Stack &L)
	{
		/* ( const char *classname, const char *name) */
		Hndl ret;
		if(L.top() >= 2) {
#ifdef UNICODE
			ret = create(L[1].toCStr(), WCStr(L[2].toCStr()).c_str());
#else
			ret = create(L[1].toCStr(), L[2].toCStr());
#endif
		}
		else if(L.top() >= 1) {
			ret = create(L[1].toCStr(), 0);
		}
		if(ret) return gctp::TukiRegister::push(L, ret);
		return 0;
	}

	int Context::insert(luapp::Stack &L)
	{
		/* ( const char *classname, const char *name) */
		Hndl ret;
		if(L.top() >= 2) {
			Ptr p = toObject(L[1]);
#ifdef UNICODE
			ret = insert(p, WCStr(L[2].toCStr()).c_str());
#else
			ret = insert(p, L[2].toCStr());
#endif
		}
		if(ret) return gctp::TukiRegister::push(L, ret);
		return 0;
	}

	int Context::find(luapp::Stack &L)
	{
		Hndl ret;
		if(L.top() >= 1) {
#ifdef UNICODE
			ret = find(WCStr(L[1].toCStr()).c_str());
#else
			ret = find(L[1].toCStr());
#endif
		}
		if(ret) return gctp::TukiRegister::push(L, ret);
		return 0;
	}

	int Context::size(luapp::Stack &L)
	{
		L << (int)size();
		return 1;
	}

	int Context::current(luapp::Stack &L)
	{
		return gctp::TukiRegister::push(L, Hndl(current__));
	}

	int Context::pairs(luapp::Stack &L)
	{
		LuaPair *ud = new(lua_newuserdata(L, sizeof(LuaPair))) LuaPair;
		ud->db = &db_; ud->i = db_.begin();
		lua_newtable(L);                // mt for method table
		lua_pushliteral(L, "__call");
		lua_pushcfunction(L, _pair_next);
		lua_settable(L, -3);
		lua_pushliteral(L, "__gc");
		lua_pushcfunction(L, _pair_gc);
		lua_settable(L, -3);
		lua_setmetatable(L, -2);
		return 1;
	}

	int Context::_pair_next(lua_State *l)
	{
		LuaPair *ud = static_cast<LuaPair *>(lua_touserdata(l, 1));
		if(ud && ud->db && ud->i != ud->db->end()) {
			luapp::Stack L(l);
#ifdef UNICODE
			L << CStr(ud->i->first.c_str()).c_str();
#else
			L << ud->i->first.c_str();
#endif
			int r = gctp::TukiRegister::push(L, ud->i->second);
			if(r == 0) {
				L << ud->i->second.get();
				r = 1;
			}
			++ud->i;
			return 1+r;
		}
		return 0;
	}

	int Context::_pair_gc(lua_State *l)
	{
		LuaPair *ud = static_cast<LuaPair *>(lua_touserdata(l, -1));
		if(ud) ud->~LuaPair();
		return 0;
	}

	int Context::ipairs(luapp::Stack &L)
	{
		LuaIPair *ud = new(lua_newuserdata(L, sizeof(LuaIPair))) LuaIPair;
		ud->n = 1; ud->list = &ptrs_; ud->i = ptrs_.begin();
		lua_newtable(L);
		lua_pushliteral(L, "__call");
		lua_pushcfunction(L, _ipair_next);
		lua_settable(L, -3);
		lua_pushliteral(L, "__gc");
		lua_pushcfunction(L, _ipair_gc);
		lua_settable(L, -3);
		lua_setmetatable(L, -2);
		return 1;
	}

	int Context::_ipair_next(lua_State *l)
	{
		LuaIPair *ud = static_cast<LuaIPair *>(lua_touserdata(l, 1));
		if(ud && ud->list && ud->i != ud->list->end()) {
			luapp::Stack L(l);
			L << ud->n;
			int r = gctp::TukiRegister::push(L, *ud->i);
			if(r == 0) {
				L << ud->i->get();
				r = 1;
			}
			++ud->i;
			ud->n++;
			return 1+r;
		}
		return 0;
	}

	int Context::_ipair_gc(lua_State *l)
	{
		LuaIPair *ud = static_cast<LuaIPair *>(lua_touserdata(l, -1));
		if(ud) ud->~LuaIPair();
		return 0;
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, core, Context, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, core, Context)
		TUKI_METHOD(Context, newChild)
		TUKI_METHOD(Context, load)
		TUKI_METHOD(Context, loadAsync)
		TUKI_METHOD(Context, create)
		TUKI_METHOD(Context, insert)
		TUKI_METHOD(Context, find)
		TUKI_METHOD(Context, size)
		TUKI_METHOD(Context, pairs)
		TUKI_METHOD(Context, ipairs)
		TUKI_METHOD(Context, current)
	TUKI_IMPLEMENT_END(Context)

}} // namespace gctp::core
