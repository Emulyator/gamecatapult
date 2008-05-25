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
#include <gctp/fileserver.hpp>
#include <gctp/turi.hpp>
#include <gctp/db.hpp>
#include <gctp/cstr.hpp>

using namespace std;

namespace gctp { namespace core {

	/** �R���e�L�X�g�̎g�p�J�n��錾
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/19 2:07:06
	 */
	void Context::push()
	{
		prev_ = current_;
		current_ = this;
		is_pushed_ = true;
	}

	/** �R���e�L�X�g�̎g�p�I����錾
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/19 2:07:06
	 */
	void Context::pop()
	{
		if(is_pushed_) {
			is_pushed_ = false;
			GCTP_ASSERT(current_ == this);
			if(prev_) current_ = prev_.get();
			else current_ = parent_.get();
		}
	}

	/** �v�����ꂽ���\�[�X�����[�h���A�ێ�����
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::load(const _TCHAR *name)
	{
		//GCTP_ASSERT(current_ == this); ����K�v���H
		if(name) {
			Hndl h = find(name);
			if(h) {
				//ptrs_.push_back(h.lock());
				// �ǂ����ׂ����c
				// ���d�Ŋm�ۂ���\�������邩��A�ǉ����Ȃ��A�̂ق��ł�����
				return h;
			}
			TURI uri(name);
//#ifdef _WIN32
//			uri.convertLower();
//#endif
			std::basic_string<_TCHAR> ext = uri.extension();
			RealizeMethod f = Extension::get(ext.c_str());
			if(f) {
				BufferPtr file = fileserver().getFile(uri.raw().c_str());
				if(file) {
					Context *backup = current_; // �ꎞ�I�ɃJ�����g�������ւ���
					current_ = this;
					loading_async_ = false;
					Ptr p = f(file);
					current_ = backup;
					if(p) {
						ptrs_.push_back(p);
						db_.insert(name, p);
						return p;
					}
				}
			}
			else {
				PRNN(_T("Context::load : �g���q'")<<uri.extension()<<_T("'�̃��A���C�U�͓o�^����Ă��Ȃ�(")<<uri.raw()<<_T("�̓ǂݍ��ݎ�)"));
			}
		}
		return Hndl();
	}

	/** �v�����ꂽ���\�[�X�̃��[�h�����N�G�X�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool Context::loadAsync(const _TCHAR *name, const Slot2<const _TCHAR *, BufferPtr> *callback)
	{
		//GCTP_ASSERT(current_ == this); ����K�v���H
		if(name) {
			Hndl h = find(name);
			if(h) {
				//ptrs_.push_back(h.lock());
				// �ǂ����ׂ����c
				// ���d�Ŋm�ۂ���\�������邩��A�ǉ����Ȃ��A�̂ق��ł�����
				return true;
			}
			TURI uri(name);
//#ifdef _WIN32
//			uri.convertLower();
//#endif
			std::basic_string<_TCHAR> ext = uri.extension();
			RealizeMethod f = Extension::get(ext.c_str());
			if(f) {
				if(!fileserver().busy()) {
					AsyncBufferPtr file = fileserver().getFileAsync(uri.raw().c_str());
					if(file) {
						// find/isReady�̌Ăяo���̒i�K�Ń��A���C�Y
						ptrs_.push_back(file);
						db_.insert(name, file);
						file->connect(on_ready_slot);
						if(callback) file->connect(*callback);
					}
				}
			}
			else {
				PRNN(_T("Context::load : �g���q'")<<uri.extension()<<_T("'�̃��A���C�U�͓o�^����Ă��Ȃ�(")<<uri.raw()<<_T("�̓ǂݍ��ݎ�)"));
			}
		}
		return false;
	}

	bool Context::loadAsync(const _TCHAR *name, const Slot2<const _TCHAR *, BufferPtr> &callback)
	{
		return loadAsync(name, &callback);
	}

	bool Context::loadAsync(const _TCHAR *name)
	{
		return loadAsync(name, 0);
	}

	/** �n���ꂽ�I�u�W�F�N�g��ێ�����
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
			if(name) db_.insert(name, ptr);
		}
		return ptr;
	}

	/** �v�����ꂽ�I�u�W�F�N�g�𐻍삵�A�ێ�����
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::create(const GCTP_TYPEINFO &typeinfo, const _TCHAR *name)
	{
		Context *backup = current_; // �ꎞ�I�ɃJ�����g�������ւ���
		current_ = this;
		Ptr ret = Factory::create(typeinfo);
		current_ = backup;
		if(ret) {
			ptrs_.push_back(ret);
			if(name) db_.insert(name, ret);
		}
		return ret;
	}

	/** �v�����ꂽ�I�u�W�F�N�g�𐻍삵�A�ێ�����
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:36:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl Context::create(const char *classname, const _TCHAR *name)
	{
		Context *backup = current_; // �ꎞ�I�ɃJ�����g�������ւ���
		current_ = this;
		Ptr ret = Factory::create(classname);
		current_ = backup;
		if(ret) {
			ptrs_.push_back(ret);
			if(name) db_.insert(name, ret);
		}
		return ret;
	}

	Hndl Context::find(const _TCHAR *name)
	{
		if(name) {
			Hndl ret = db_[name];
			if(ret) {
				AsyncBufferPtr async = ret.get();
				// ���ꂾ�Ɩ��B
				// ����t���[���œ������\�[�X��v�������ꍇ�A���L����Ȃ��B
				// loadAsync���_�ŋ�̎��̂����K�v����
				// load�Ώۂ̃��\�[�X�́A�S���u��̏�ԁv���`�ł���K�v����i����͂��łɂn�j�BRealizer�͂��������I�u�W�F�N�g�����j
				// AsyncBuffer���X�g��ʌɎ��K�v����H
				// ����Ƃ�load�\���\�[�X�͂��ׂ�AsyncBufferPtr�������o�Ɏ��H
				// ����͖������ۂ��B�i���[�h�\��Object�ŁA���ʂȔh���N���X�ł͂Ȃ��j
				// ���Ƃ�����Context���ێ���������K�v����
				// ����GraphFile�������Ă���asyncsolvers��Conext�����ׂ�
				if(async) {
					if(async->isReady()) {
						onReady(name, async);
						return db_[name];
					}
				}
				else return ret;
			}
			else if(parent_) return parent_->find(name);
		}
		return Hndl();
	}

	bool Context::onReady(const _TCHAR *name, BufferPtr buffer)
	{
		TURI uri(name);
		std::basic_string<_TCHAR> ext = uri.extension();
		RealizeMethod f = Extension::get(ext.c_str());
		if(f) {
			Context *backup = current_; // �ꎞ�I�ɃJ�����g�������ւ���
			current_ = this;
			loading_async_ = true;
			Ptr p = f(buffer);
			current_ = backup;
			ptrs_.remove(buffer);
			if(p) {
				ptrs_.push_back(p);
				db_.set(name, p);
			}
			else db_.erase(name);
		}
		else {
			PRNN(_T("Context::load : �g���q'")<<uri.extension()<<_T("'�̃��A���C�U�͓o�^����Ă��Ȃ�(")<<uri.raw()<<_T("�̓ǂݍ��ݎ�)"));
		}
		return false;
	}

	void Context::serialize(Serializer &serializer)
	{
		Context *backup = current_; // �ꎞ�I�ɃJ�����g�������ւ���
		current_ = this;
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
		current_ = backup;
	}

	Context *Context::current_ = NULL;

	bool Context::setUp(luapp::Stack &L)
	{
		// gctp.core.Context.current().child()�o�R�łȂ���΍��Ȃ�
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
		if(L.top() >= 1) {
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
		if(L.top() >= 1) {
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

	int Context::current(luapp::Stack &L)
	{
		return gctp::TukiRegister::push(L, Hndl(current_));
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
		TUKI_METHOD(Context, find)
		TUKI_METHOD(Context, pairs)
		TUKI_METHOD(Context, ipairs)
		TUKI_METHOD(Context, current)
	TUKI_IMPLEMENT_END(Context)

	AsyncSolver::AsyncSolver(Context &owner, Pointer<AsyncBuffer> async_buffer) : owner(owner), async_buffer(async_buffer) {}

}} // namespace gctp::core
