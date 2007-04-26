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
#include <gctp/turi.hpp>
#include <gctp/db.hpp>

using namespace std;

namespace gctp {

	GCTP_IMPLEMENT_CLASS_NS(gctp, Context, Object);

	Context::Context(bool do_open) : is_open_(false)
	{
		if(do_open) open();
	}

	Context::~Context()
	{
		close();
	}

	/** コンテキストの使用開始を宣言
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/19 2:07:06
	 */
	void Context::open()
	{
		prev_ = current_;
		current_ = this;
		is_open_ = true;
	}

	/** コンテキストの使用終了を宣言
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/19 2:07:06
	 */
	void Context::close()
	{
		if(is_open_) {
			is_open_ = false;
			GCTP_ASSERT(current_ == this);
			current_ = prev_;
		}
	}

	/** 要求されたリソースをロードし、保持する
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool Context::load(const _TCHAR *name)
	{
		GCTP_ASSERT(current_ == this);
		TURI uri(name);
//#ifdef _WIN32
//		uri.convertLower();
//#endif
		std::basic_string<_TCHAR> ext = uri.extension();
		RealizeMethod f = Extension::get(ext.c_str());
		if(f) {
			Ptr p = f(uri.raw().c_str());
			if(p) {
				ptrs_.push_back(p);
				return true;
			}
		}
		else {
			PRNN(_T("Context::load : 拡張子'")<<uri.extension()<<_T("'のリアライザは登録されていない(")<<uri.raw()<<_T("の読み込み時)"));
		}
		return false;
	}

	/** 渡されたオブジェクトを保持する
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::add(const Ptr ptr, const _TCHAR *name)
	{
		GCTP_ASSERT(current_ == this);
		if(ptr) {
			ptrs_.push_back(ptr);
			if(name) db().insert(name, ptr);
		}
		return ptr;
	}

	/** 要求されたオブジェクトを製作し、保持する
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::create(const GCTP_TYPEINFO &typeinfo, const _TCHAR *name)
	{
		GCTP_ASSERT(current_ == this);
		Ptr ret = Factory::create(typeinfo);
		if(ret) {
			ptrs_.push_back(ret);
			if(name) db().insert(name, ret);
		}
		return ret;
	}

	/** 要求されたオブジェクトを製作し、保持する
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::create(const char *classname, const _TCHAR *name)
	{
		GCTP_ASSERT(current_ == this);
		Ptr ret = Factory::create(classname);
		if(ret) {
			ptrs_.push_back(ret);
			if(name) db().insert(name, ret);
		}
		return ret;
	}

	void Context::serialize(Serializer &serializer)
	{
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
	}

	Context *Context::current_ = NULL;

	bool Context::setUp(luapp::Stack &L)
	{
		return true;
	}

	int Context::luaLoad(luapp::Stack &L)
	{
		/* (const char *fname) */
#ifdef UNICODE
		WCStr str = L[1].toCStr();
		L << load(str.c_str());
#else
		L << load(L[1].toCStr());
#endif
		return 1;
	}
	
	int Context::luaCreate(luapp::Stack &L)
	{
		/* ( const char *classname, const char *name) */
#ifdef UNICODE
		WCStr str = L[2].toCStr();
		L << (create(L[1].toCStr(), str.c_str()) ? true : false);
#else
		L << (create(L[1].toCStr(), L[2].toCStr()) ? true : false);
#endif
		return 1;
	}

	TUKI_IMPLEMENT_BEGIN_NS(gctp, Context)
		TUKI_METHOD_EX(Context, "load", luaLoad)
		TUKI_METHOD_EX(Context, "create", luaCreate)
	TUKI_IMPLEMENT_END(Context)

} // namespace gctp
