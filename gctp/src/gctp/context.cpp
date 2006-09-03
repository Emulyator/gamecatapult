/** @file
 * GameCatapult �R���e�L�X�g�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/context.hpp>
#include <gctp/serializer.hpp>
#include <gctp/uri.hpp>
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

	/** �R���e�L�X�g�̎g�p�J�n��錾
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

	/** �R���e�L�X�g�̎g�p�I����錾
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

	/** �v�����ꂽ���\�[�X�����[�h���A�ێ�����
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool Context::load(const char *name)
	{
		GCTP_ASSERT(current_ == this);
		URI uri(name);
//#ifdef _WIN32
//		uri.convertLower();
//#endif
		std::string ext = uri.extension();
		RealizeMethod f = Extension::get(ext.c_str());
		if(f) {
			Ptr p = f(uri.raw().c_str());
			if(p) {
				ptrs_.push_back(p);
				return true;
			}
		}
		else {
#ifdef UNICODE
			wchar_t _ext[256];
			if(0 == MultiByteToWideChar(932, 0, ext.c_str(), -1, _ext, 256)) MultiByteToWideChar(CP_ACP, 0, ext.c_str(), -1, _ext, 256);
			PRNN(_T("Context::load : �g���q'")<<_ext<<_T("'�̃��A���C�U�͓o�^����Ă��Ȃ�"));
#else
			PRNN(_T("Context::load : �g���q'")<<uri.extension()<<_T("'�̃��A���C�U�͓o�^����Ă��Ȃ�"));
#endif
		}
		return false;
	}

	/** �n���ꂽ�I�u�W�F�N�g��ێ�����
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::add(const Ptr ptr, const char *name)
	{
		GCTP_ASSERT(current_ == this);
		if(ptr) {
			ptrs_.push_back(ptr);
			if(name) db().insert(name, ptr);
		}
		return ptr;
	}

	/** �v�����ꂽ�I�u�W�F�N�g�𐻍삵�A�ێ�����
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::create(const GCTP_TYPEINFO &typeinfo, const char *name)
	{
		GCTP_ASSERT(current_ == this);
		Ptr ret = Factory::create(typeinfo);
		if(ret) {
			ptrs_.push_back(ret);
			if(name) db().insert(name, ret);
		}
		return ret;
	}

	/** �v�����ꂽ�I�u�W�F�N�g�𐻍삵�A�ێ�����
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::create(const char *classname, const char *name)
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
			int size = ptrs_.size();
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
		load(L[1].toCStr());
		return 1;
	}
	
	int Context::luaCreate(luapp::Stack &L)
	{
		/* ( const char *classname, const char *name) */
		create(L[1].toCStr(), L[2].toCStr());
		return 0;
	}

	TUKI_IMPLEMENT_BEGIN_NS(gctp, Context)
		TUKI_METHOD_EX(Context, "load", luaLoad)
		TUKI_METHOD_EX(Context, "create", luaCreate)
	TUKI_IMPLEMENT_END(Context)

} // namespace gctp
