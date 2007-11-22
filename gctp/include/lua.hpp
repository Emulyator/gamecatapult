#ifndef _LUA_HPP_
#define _LUA_HPP_
/** @file
 * Lua��C++���b�p�[�B���Â���luapp�B
 * @author SAM (T&GG, Org.) 
 * @date 2003/06/26 2:43:16
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) All rights reserved.
 *
 * ���̃\�[�X�R�[�h�͖����Ŏ��R�Ɏg�p�E�ύX�\�ł��B
 *
 * ���̏����𖞂�������ɂ����āA�i�|�ĕ����܂ށj���n����O���M�������܂��B\n
 * 1) �I���W�i���̒��쌠�\�L��ύX���Ȃ����ƁB\n
 * 2) ���L�Ɛӎ�����ύX�����܂߂邱�ƁB\n
 * �Ɛӎ���\n
 * ���̃\�t�g�E�F�A�͖��ۏ؂ł��B�x��Ȃ��쓮���邱�ƁA����y�ѓ��e�ɂ������r��
 * ���݂��Ȃ����ƁA���r���C������邱�ƁA��O�҂̌�����N�Q���Ă��Ȃ����Ƃ��̑�
 * �����َ����킸�A����̕ۏ؂����܂���B\n
 * ���҂���т��̊�^�ҁA�{�\�t�g�E�F�A�̔z�z�҂́A���̃\�t�g�E�F�A���g�p��������
 * �ɂ�邢���Ȃ鑹�Q�ɂ������̐ӔC�𕉂��܂���B�g�p�҂͎���̐ӔC�ƕ��S�ɂ�����
 * �g�p������̂Ƃ��܂��B
 */
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
//#include <iosfwd>
#include <iostream> // for std::cerr
#include <string>
#include <vector>
#include <utility>
#include <boost/function.hpp>
#include <gctp/dbgout.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4355)
#endif

namespace luapp {

	/** Lua�^�C�v���
	 *
	 * @author SAM (T&GG, Org.) <sowwaNO-SPAM-THANKS@water.sannet.ne.jp>
	 * @date 2003/06/26 3:31:33
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) All rights reserved.
	 */
	enum Type {
		NONE = LUA_TNONE,
		NIL = LUA_TNIL,
		BOOLEAN = LUA_TBOOLEAN,
		LIGHTUSERDATA = LUA_TLIGHTUSERDATA,
		NUMBER = LUA_TNUMBER,
		STRING = LUA_TSTRING,
		TABLE = LUA_TTABLE,
		FUNCTION = LUA_TFUNCTION,
		USERDATA = LUA_TUSERDATA,
		THREAD = LUA_TTHREAD,

		STACKVALUE = 100, // luapp::Key�ɂ����āA�X�^�b�N��̒l���w���A�̈Ӗ�
	};

	class Table;
	/** lua�X�^�b�N��̒l��\���N���X
	 *
	 * @author SAM (T&GG, Org.) <sowwaNO-SPAM-THANKS@water.sannet.ne.jp>
	 * @date 2003/06/26 3:40:39
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) All rights reserved.
	 */
	class Value {
	protected:
		lua_State *lua_;
		int index_;
		virtual int eval() const { return index_; }
		virtual void pop() const {}
		class EvalAndPop {
			const Value &value_;
			int index_;
		public:
			EvalAndPop(const Value &value) : value_(value), index_(value.eval()) {}
			~EvalAndPop() { value_.pop(); }
			int index() { return index_; }
		};
		friend class EvalAndPop;

	public:
		/** �R���X�g���N�^
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 * @date 2005/05/22 8:22:29
		 */
		Value(lua_State *lua, int index) : lua_(lua), index_(index) {}

		/// index��Ԃ�
		int index() const { return index_; }

		/// void*�ɕϊ�
		const void *toPointer() const
		{
			EvalAndPop _eval_(*this);
			return lua_topointer(lua_, _eval_.index());
		}

		/// bool�ɕϊ�
		bool toBoolean() const
		{
			EvalAndPop _eval_(*this);
			return (lua_toboolean(lua_, _eval_.index()))?true:false;
		}

		/// int�ɕϊ�
		int toInteger() const
		{
			EvalAndPop _eval_(*this);
			return static_cast<int>(lua_tonumber(lua_, _eval_.index()));
		}
		
		/// lua_Number�ɕϊ�
		lua_Number toNumber() const
		{
			EvalAndPop _eval_(*this);
			return lua_tonumber(lua_, _eval_.index());
		}
		
		/// Table�ɕϊ�
		Table toTable() const;
		
		/// Table�ɔh�����Ă���C���X�^���X�Ȃ�A�|�C���^��Ԃ�
		virtual Table *dynamicCast() { return 0; }

		/// Table�ɔh�����Ă���C���X�^���X�Ȃ�A�|�C���^��Ԃ�
		virtual const Table *dynamicCast() const { return 0; }

		/// const char *�ɕϊ�
		const char *toCStr() const
		{
			EvalAndPop _eval_(*this);
			return lua_tostring(lua_, _eval_.index());
		}

		/// std::string�ɕϊ�
		const std::string toString() const
		{
			EvalAndPop _eval_(*this);
			return std::string(lua_tostring(lua_, _eval_.index()), lua_strlen(lua_, _eval_.index()));
		}

		/// lua_CFunction(int (*)(lua_State *))�ɕϊ�
		lua_CFunction toCFunction() const
		{
			EvalAndPop _eval_(*this);
			return lua_tocfunction(lua_, _eval_.index());
		}

		/// �R���[�`��(lua_State *)�ɕϊ�
		lua_State *toCoroutine() const
		{
			EvalAndPop _eval_(*this);
			return lua_tothread(lua_, _eval_.index());
		}

		/// ���[�U�[�f�[�^(void *)�ɕϊ�
		void *toUserData() const
		{
			EvalAndPop _eval_(*this);
			return lua_touserdata(lua_, _eval_.index());
		}

		/// �^�ϊ�
		operator bool()
		{
			EvalAndPop _eval_(*this);
			return (lua_toboolean(lua_, _eval_.index()))?true:false;
		}

		/// �^�ϊ�
		operator int()
		{
			EvalAndPop _eval_(*this);
			return static_cast<int>(lua_tonumber(lua_, _eval_.index()));
		}

		/// �^�ϊ�
		operator lua_Number()
		{
			EvalAndPop _eval_(*this);
			return lua_tonumber(lua_, _eval_.index());
		}
		
		/// �^�ϊ�
		operator const char *()
		{
			EvalAndPop _eval_(*this);
			return lua_tostring(lua_, _eval_.index());
		}

		/// �^�ϊ�
		operator lua_CFunction()
		{
			EvalAndPop _eval_(*this);
			return lua_tocfunction(lua_, _eval_.index());
		}

		/// �^�ϊ�
		operator lua_State *()
		{
			EvalAndPop _eval_(*this);
			return lua_tothread(lua_, _eval_.index());
		}

		/// �^�ϊ�
		operator void *()
		{
			EvalAndPop _eval_(*this);
			return lua_touserdata(lua_, _eval_.index());
		}		
		
		/*@{*/
		/** �^�₢���킹 */
		Type type() const
		{
			EvalAndPop _eval_(*this);
			return (Type)lua_type(lua_, _eval_.index());
		}
		
		inline bool isNil() const
		{
			EvalAndPop _eval_(*this);
			return (lua_isnil(lua_, _eval_.index()))?true:false;
		}

		inline bool isBoolean() const
		{
			EvalAndPop _eval_(*this);
			return (lua_isboolean(lua_, _eval_.index()))?true:false;
		}

		inline bool isNumber() const
		{
			EvalAndPop _eval_(*this);
			return (lua_isnumber(lua_, _eval_.index()))?true:false;
		}
		
		inline bool isString() const
		{
			EvalAndPop _eval_(*this);
			return (lua_isstring(lua_, _eval_.index()))?true:false;
		}
		
		inline bool isTable() const
		{
			EvalAndPop _eval_(*this);
			return (lua_istable(lua_, _eval_.index()))?true:false;
		}

		inline bool isFunction() const
		{
			EvalAndPop _eval_(*this);
			return (lua_isfunction(lua_, _eval_.index()))?true:false;
		}
		
		inline bool isCFunction() const
		{
			EvalAndPop _eval_(*this);
			return (lua_iscfunction(lua_, _eval_.index()))?true:false;
		}
		
		inline bool isUserData() const
		{
			EvalAndPop _eval_(*this);
			return (lua_isuserdata(lua_, _eval_.index()))?true:false;
		}
		
		inline bool isLigntUserData() const
		{
			EvalAndPop _eval_(*this);
			return (lua_islightuserdata(lua_, _eval_.index()))?true:false;
		}

		inline bool isCoroutine() const
		{
			EvalAndPop _eval_(*this);
			return (lua_isthread(lua_, _eval_.index()))?true:false;
		}
		/*@}*/

		/// ������̒������擾
		size_t strlen() const
		{
			EvalAndPop _eval_(*this);
			return lua_strlen(lua_, _eval_.index());
		}
		
		/// �L���ȃC���X�^���X���H
		bool isOk() const {
			return lua_ ? true : false;
		}
		
		/// �L���ȃt�����[�U�[�f�[�^�ւ̃o�C���h��������������e���v���[�g
		template<typename T> T *check(T *(check)(lua_State *L, int argn)) const
		{
			EvalAndPop _eval_(*this);
			return check(lua_, _eval_.index());
		}

		/// �֐��Ăяo��
		bool operator()(class Callback &args, class Callback &rets);

		/// �X�^�b�N�g�b�v�ɐς�
		virtual void push() const
		{
			lua_checkstack(lua_, 1);
			lua_pushvalue(lua_, index_);
		}

		/** ����̒l�������H
		 *
		 * �^�ϊ����Z�q�������ς���`���������ŁA��r���Z�q���`�ł��Ȃ��Ȃ��Ă��܂����c
		 */
		bool isEqual(const Value &rhs) const
		{
			if(lua_ != rhs.lua_) return false;
			EvalAndPop _eval_lhs_(*this);
			EvalAndPop _eval_rhs_(rhs);
			return lua_equal(lua_, _eval_lhs_.index(), _eval_rhs_.index()) ? true : false;
		}
	};

	template<class E, class T>
	std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Value const & v)
	{
		switch(v.type()) {
		case NIL: os << "NIL"; break;
		case TABLE: os << "TABLE"; break;
		case NUMBER: os << v.toNumber(); break;
		case BOOLEAN: os << (v.toBoolean() ? "true" : "false"); break;
		case STRING: os << v.toCStr(); break;
		case FUNCTION: os << "FUNCTION:" << (void *)v.toCFunction(); break;
		case THREAD: os << "THREAD:" << (void *)v.toCoroutine(); break;
		case USERDATA: case LIGHTUSERDATA: os << "USERDATA:" << v.toUserData(); break;
		}
		return os;
	}

	/** �e�[�u���A�N�Z�X�̃L�[�l
	 *
	 * Table�A�N�Z�X�ɂ����āAL.global()["a"][1][L.global()["b"]["key"]][L[1]] �Ƃ�����
	 * ���������\�ɂ��邽�߂�Proxy�N���X�B���ڈӎ�����K�v�͖����Ǝv���B
	 * @author SAM (T&GG, Org.) <sowwaNO-SPAM-THANKS@water.sannet.ne.jp>
	 * @date 2003/07/04 3:11:53
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) All rights reserved.
	 */
	struct Key {
		Key() : type(NIL), i(0) {}
		Key(const int i, int) : type(NONE), i(i) {}
		Key(const bool boolean) : type(BOOLEAN), boolean(boolean) {}
		Key(const int number) : type(NUMBER), number(static_cast<lua_Number>(number)) {}
		Key(const lua_Number number) : type(NUMBER), number(number) {}
		Key(const char *string) : type(STRING), string(string) {}
		Key(const lua_CFunction cfunction) : type(FUNCTION), cfunction(cfunction) {}
		Key(const lua_State *thread) : type(THREAD), thread(thread) {}
		Key(const void *userdata) : type(LIGHTUSERDATA), userdata(userdata) {}
		Key(const Key &src);
		Key(const Value &value);
		Key(const Table &table);
		~Key();

		Type type;
		union {
			int i;
			bool boolean;
			lua_Number number;
			const char *string;
			lua_CFunction cfunction;
			const lua_State *thread;
			const void *userdata;
			const Table *table;
		};

		void push(lua_State *lua) const;

		void set(lua_State *lua, int index) const
		{
			if(type==NONE) lua_rawseti(lua, index, i);
			else lua_settable(lua, index);
		}
	};

	inline bool operator==(const Key &lhs, const Key &rhs)
	{
		return (lhs.type == rhs.type) && (lhs.userdata == rhs.userdata);
	}

	template<class E, class T>
	std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Key const & k)
	{
		switch(k.type) {
		case NIL: os << "NIL"; break;
		case NONE: os << "NONE:" << k.i; break;
		case TABLE: os << "TABLE"; break;
		case NUMBER: os << k.number; break;
		case BOOLEAN: os << (k.boolean ? "true" : "false"); break;
		case STRING: os << k.string; break;
		case FUNCTION: os << "FUNCTION:" << (void*)k.cfunction; break;
		case THREAD: os << "THREAD:" << (void*)k.thread; break;
		case LIGHTUSERDATA: os << "USERDATA:" << k.userdata; break;
		case STACKVALUE: os << "STACKVALUE:" << k.i; break;
		}
		return os;
	}

	class Stack;
	/** Lua�Ƀo�C���h����C�֐��̌^�iluapp::Stack�ň������󂯂���j
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/05/17 2:41:42
	 */
	typedef int (*Function)(Stack &);

	class TableItr;
	/** lua�e�[�u���N���X
	 *
	 * @author SAM (T&GG, Org.) <sowwaNO-SPAM-THANKS@water.sannet.ne.jp>
	 * @date 2003/07/03 5:00:04
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) All rights reserved.
	 */
	class Table : public Value {
	protected:
		typedef std::vector<Key> KeyArray;
		KeyArray keys_;

		virtual int eval() const
		{
			bool first = true;
			for(KeyArray::const_iterator i = keys_.begin(); i != keys_.end(); ++i) {
				switch(i->type) {
				case NIL: break;
				case NONE:
					if(first) lua_rawgeti(lua_, index_, i->i);
					else if(lua_istable(lua_, -1)) {
						lua_rawgeti(lua_, -1, i->i);
						lua_remove(lua_, -2);
					}
					else {
						lua_pop(lua_, 1);
						lua_pushnil(lua_);
					}
					break;
				default:
					i->push(lua_);
					if(first) lua_gettable(lua_, index_);
					else if(lua_istable(lua_, -2)) {
						lua_gettable(lua_, -2);
						lua_remove(lua_, -2);
					}
					else {
						lua_pop(lua_, 2);
						lua_pushnil(lua_);
					}
					break;
				}
				first = false;
				if(lua_isnil(lua_, -1)) break;
			}
			if(first) return index_;
			else return lua_gettop(lua_);
		}
		virtual void pop() const { if(hasKeyArray()) lua_pop(lua_, 1); }

		class AutoPop {
		public:
			lua_State *lua_;
			int npop_;
			AutoPop(lua_State *lua, int npop) : lua_(lua), npop_(npop) {}
			~AutoPop() { lua_pop(lua_, npop_); }
		};
		friend class AutoPop;
		friend class TableItr;

	public:
		/// �R���X�g���N�^
		Table(lua_State *lua, int index) : Value(lua, index) {}
		/// �R���X�g���N�^
		Table(const Value &src) : Value(src) {}
		/// �R�s�[�R���X�g���N�^
		Table(const Table &src) : Value(src), keys_(src.keys_) {}
		/// �R���X�g���N�^
		Table(const Table &src, const char *key) : Value(src.lua_, src.index_), keys_(src.keys_)
		{
			keys_.push_back(Key(key));
		}
		/// �R���X�g���N�^
		Table(const Table &src, int key) : Value(src.lua_, src.index_), keys_(src.keys_)
		{
			keys_.push_back(Key(key));
		}
		/// �R���X�g���N�^
		Table(const Table &src, const Key &key) : Value(src.lua_, src.index_), keys_(src.keys_)
		{
			keys_.push_back(key);
		}

		/// �L�[��ǉ�
		Table &addKey(const Key &key)
		{
			keys_.push_back(key);
			return *this;
		}
		
		/// Table�ɔh�����Ă���C���X�^���X�Ȃ�A�|�C���^��Ԃ�
		virtual Table *dynamicCast() { return this; }

		/// Table�ɔh�����Ă���C���X�^���X�Ȃ�A�|�C���^��Ԃ�
		virtual const Table *dynamicCast() const { return this; }

		/// �L�[��0�ȏォ�H
		bool hasKeyArray() const
		{
			return keys_.size() > 0;
		}

		/// �L�[�A�N�Z�X
		Table operator[](const std::string &key) const
		{
			return Table(*this, key.c_str());
		}

		/// �L�[�A�N�Z�X
		Table operator[](const char *key) const
		{
			return Table(*this, key);
		}

		/// �L�[�A�N�Z�X
		Table operator[](int key) const
		{
			return Table(*this, key);
		}

		/// �L�[�A�N�Z�X
		Table operator[](const Key &key) const
		{
			return Table(*this, key);
		}

		/// �L�[�A�N�Z�X
		Table operator[](const Table &table) const
		{
			return Table(*this, Key(table));
		}

		/// Value����e�[�u����(�̂���ŊԈ���ČĂяo�����Ƃ�����̂�)
		Table toTable() const
		{
			return *this;
		}

		/// ���̃e�[�u�����ɁA�V�����e�[�u�����w��̃L�[�ō��
		Table newTable(const Key &key)
		{
			EvalAndPop _eval_(*this);
			key.push(lua_);
			lua_newtable(lua_);
			key.set(lua_, _eval_.index());
			return (*this)[key];
		}

		/// ���̃e�[�u�����ɁA�V�����e�[�u�����w��̖��O�ō��
		Table newTable(const char *name)
		{
			EvalAndPop _eval_(*this);
			lua_checkstack(lua_, 2);
			lua_pushstring(lua_, name);
			lua_newtable(lua_);
			lua_settable(lua_, _eval_.index());
			return (*this)[name];
		}

		/// ���̃e�[�u�����ɁA�X�^�b�N�g�b�v�̒l���w��̃L�[�Ŋi�[����
		void declare(const Key &key)
		{
			int top = lua_gettop(lua_);
			EvalAndPop _eval_(*this);
			key.push(lua_);
			lua_checkstack(lua_, 1);
			lua_pushvalue(lua_, top);
			key.set(lua_, _eval_.index());
		}

		/// ���̃e�[�u�����ɁA�w��̃u�[���l���w��̃L�[�Ŋi�[����
		void declare(const Key &key, bool b)
		{
			EvalAndPop _eval_(*this);
			key.push(lua_);
			lua_checkstack(lua_, 1);
			lua_pushboolean(lua_, b);
			key.set(lua_, _eval_.index());
		}

		/// ���̃e�[�u�����ɁA�w��̒l���w��̃L�[�Ŋi�[����
		void declare(const Key &key, int i)
		{
			EvalAndPop _eval_(*this);
			key.push(lua_);
			lua_checkstack(lua_, 1);
			lua_pushnumber(lua_, static_cast<float>(i));
			key.set(lua_, _eval_.index());
		}

		/// ���̃e�[�u�����ɁA�w��̒l���w��̃L�[�Ŋi�[����
		void declare(const Key &key, lua_Number n)
		{
			EvalAndPop _eval_(*this);
			key.push(lua_);
			lua_checkstack(lua_, 1);
			lua_pushnumber(lua_, n);
			key.set(lua_, _eval_.index());
		}

		/// ���̃e�[�u�����ɁA�w��̕�������w��̃L�[�Ŋi�[����
		void declare(const Key &key, const char *str)
		{
			EvalAndPop _eval_(*this);
			key.push(lua_);
			lua_checkstack(lua_, 1);
			lua_pushstring(lua_, str);
			key.set(lua_, _eval_.index());
		}

		/// ���̃e�[�u�����ɁA�w��̃��C�g���[�U�[�f�[�^���w��̃L�[�Ŋi�[����
		void declare(const Key &key, void *ud)
		{
			EvalAndPop _eval_(*this);
			key.push(lua_);
			lua_checkstack(lua_, 1);
			lua_pushlightuserdata(lua_, ud);
			key.set(lua_, _eval_.index());
		}

		/// ���̃e�[�u�����ɁA�w��̊֐����w��̃L�[�Ŋi�[����
		inline void declare(const Key &key, lua_CFunction f)
		{
			EvalAndPop _eval_(*this);
			key.push(lua_);
			lua_checkstack(lua_, 1);
			lua_pushcfunction(lua_, f);
			key.set(lua_, _eval_.index());
		}

		/// ���̃e�[�u�����ɁA�w��̊֐����w��̖��O���L�[�Ɋi�[����
		inline void declare(const char *name, lua_CFunction f)
		{
			EvalAndPop _eval_(*this);
			lua_checkstack(lua_, 2);
			lua_pushstring(lua_, name);
			lua_pushcfunction(lua_, f);
			lua_settable(lua_, _eval_.index());
		}

		/// ���̃e�[�u�����ɁA�w��̊֐����w��̃L�[�Ŋi�[����
		void declare(const Key &key, Function f);

		/// ���̃e�[�u�����ɁA�w��̊֐����w��̖��O���L�[�Ɋi�[����
		void declare(const char *name, Function f);

		/// ���̃e�[�u�����ɁA�w��̒l���w��̃L�[�Ŋi�[����
		void declare(const Key &key, const Value &val)
		{
			EvalAndPop _eval_table_(*this);
			{
				EvalAndPop _eval_value_(val);
				key.push(lua_);
				lua_checkstack(lua_, 1);
				lua_pushvalue(lua_, _eval_value_.index());
				key.set(lua_, _eval_table_.index());
			}
		}

		/// ���̃e�[�u�����́A�w��̃L�[����������
		void erase(const Key &key)
		{
			EvalAndPop _eval_(*this);
			key.push(lua_);
			lua_checkstack(lua_, 1);
			lua_pushnil(lua_);
			key.set(lua_, _eval_.index());
		}

		/// ���̃e�[�u�����ɁA�w��̃L�[�����l�����݂��邩�H
		bool hasKey(const Key &key) const
		{
			return !(*this)[key].isNil();
		}

		/// �X�^�b�N�g�b�v�ɐς�
		virtual void push() const
		{
			eval();
			if(!hasKeyArray()) {
				lua_checkstack(lua_, 1);
				lua_pushvalue(lua_, index_);
			}
		}

		/** �e�[�u������́A�����ЂƂ̕��@
		 *
		 * ������̂ق����y�ʂ����A-3�Ƀe�[�u���A-2�ɃL�[�A-1�ɒl�A���ێ�����K�v������
		 * @code
			{
				luapp::Table::AltIterator i(table);
				// ���̎��_�� .... table nil nil] �ɂȂ��Ă���
				while(i.next()) {
					// ���̎��_�� .... table key value] �ɂȂ��Ă���
					i.key();
					i.value();
				}
				// ���̎��_�� .... table] �ɂȂ��Ă���
			}
			// ���̎��_�� ....]
			// �S�ă|�b�v����Ă���
		 * @endcode
		 */
		class AltIterator {
		public:
			AltIterator(const Table &t) : lua_(t.lua_), not_end_(true)
			{
				t.push();
				lua_checkstack(lua_, 2);
				lua_pushnil(lua_);
				lua_pushnil(lua_);
			}
			Value key() { return Value(lua_, -2); }
			Value value() { return Value(lua_, -1); }
			bool next()
			{
				if(not_end_) {
					lua_pop(lua_, 1);
					if(lua_next(lua_, -2)) return true;
					else not_end_ = false;
				}
				return false;
			}
			~AltIterator()
			{
				if(not_end_) lua_pop(lua_, 3);
				else lua_pop(lua_, 1);
			}
		private:
			lua_State *lua_;
			bool not_end_;
		};
		friend class AltIterator;

		/// �C�e���[�^�^�̃G�C���A�X
		typedef TableItr iterator;
		/// �e�[�u���v�f�̐擪���w���C�e���[�^��Ԃ�
		iterator begin();
		/// �e�[�u������I�[�������C�e���[�^��Ԃ�
		iterator end();
	};

	/// �R�s�[�R���X�g���N�^
	inline Key::Key(const Key &src) : type(src.type)
	{
		switch(type) {
		case NIL: break;
		case NONE: case STACKVALUE: i = src.i; break;
		case BOOLEAN: boolean = src.boolean; break;
		case NUMBER: number = src.number; break;
		case STRING: string = src.string; break;
		case TABLE: table = new Table(*src.table); break;
		case THREAD: thread = src.thread; break;
		case LIGHTUSERDATA: userdata = src.userdata; break;
		}
	}

	/** Value����L�[�ɕϊ�
	 *
	 * Table�̏ꍇ�A�R�s�[���ĕێ�����B
	 * in lua :
	 * a = {key = 1, 5, 10}
	 *
	 * foo = L.global()["a"][static_cast<Value&>(L.global()["a"]["key"])];
	 * bar = L.global()["a"][L.global()["a"]["key"]];
	 *
	 * in lua :
	 * a.key = 2
	 *
	 * foo.toNumber() == 5 // Value�ɃL���X�g����ƁA���̂Ƃ���key�̒l�Ƀo�C���h�����
	 * bar.toNumber() == 10 // Key��Table���R�s�[����Ă���̂ŁAa.key�̕ύX�ɒǏ]����
	 *
	 * �e�[�u���A�t�����[�U�[�f�[�^�ALua�֐��ȂǁAlua�o�R�ł����擾�ł��Ȃ��l�́A
	 * �X�^�b�N��ɖ�������Table���R�s�[����B
	 */
	inline Key::Key(const Value &value) {
		switch(type = value.type()) {
		case NIL: break;
		case BOOLEAN: boolean = value.toBoolean(); break;
		case NUMBER: number = value.toNumber(); break;
		case STRING: string = value.toCStr(); break;
		case THREAD: thread = value.toCoroutine(); break;;
		case LIGHTUSERDATA: userdata = value.toUserData(); break;
		case FUNCTION:
			if(value.isCFunction()) {
				cfunction = value.toCFunction();
				break;
			}
		default:
			{
				const Table *table = value.dynamicCast();
				if(table) {
					type = TABLE;
					this->table = new Table(*table);
				}
				else {
					type = STACKVALUE;
					i = value.index();
				}
			}
			break;
		}
	}

	/** Table����L�[�ɕϊ�
	 *
	 * Table�̏ꍇ�A�R�s�[���ĕێ�����B
	 * in lua :
	 * a = {key = 1, 5, 10}
	 *
	 * foo = L.global()["a"][static_cast<Value&>(L.global()["a"]["key"])];
	 * bar = L.global()["a"][L.global()["a"]["key"]];
	 *
	 * in lua :
	 * a.key = 2
	 *
	 * foo.toNumber() == 5 // Value�ɃL���X�g����ƁA���̂Ƃ���key�̒l�Ƀo�C���h�����
	 * bar.toNumber() == 10 // Key��Table���R�s�[����Ă���̂ŁAa.key�̕ύX�ɒǏ]����
	 */
	inline Key::Key(const Table &table) {
		type = TABLE;
		this->table = new Table(table);
	}

	inline Key::~Key()
	{
		if(type == TABLE && table) delete table;
	}

	inline void Key::push(lua_State *lua) const
	{
		if(type!=NONE&&type!=TABLE) lua_checkstack(lua, 1);
		switch(type) {
		case NONE: break;
		case BOOLEAN: lua_pushboolean(lua, boolean); break;
		case NUMBER: lua_pushnumber(lua, number); break;
		case STRING: lua_pushstring(lua, string); break;
		case TABLE: table->push(); break;
		case FUNCTION: lua_pushcfunction(lua, cfunction); break;
		case LIGHTUSERDATA: lua_pushlightuserdata(lua, const_cast<void *>(userdata)); break;
		case STACKVALUE: lua_pushvalue(lua, i); break;
		default: lua_pushnil(lua); break;
		}
	}

	/// Value����e�[�u����
	inline Table Value::toTable() const
	{
		return Table(*this);
	}
	
	/** �e�[�u���v�f�����񂷂�C�e���[�^
	 *
	 * LuaAPI�̐�������A�P�����V�[�P���V�����A�N�Z�X�C�e���[�^
	 *
	 * @code
		for(luapp::Table::iterator i = lua.global()["a"].begin(); i != lua.global()["a"].end(); ++i) {
			dbgout << i.key() << "," << *i << endl;
		}
	 * @endcode
	 *
	 * �܂��C���X�^���X��this�|�C���^���L�[��Lua���W�X�g���ɏ����c���A���̃e�[�u���̒l��
	 * �L�[�ɂ���iluapp::Table��new����ăR�s�[�����j�ȂǁA��|����Ȏ����ɂȂ��Ă���B
	 *
	 * �\�Ȃ�ATable::AltIterator���g�����ق����ǂ��B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/05/21 9:55:00
	 */
	class TableItr {
	public:
		TableItr() : key_(0, 0), value_(0, 0) {}
		TableItr(const Table& table, bool begin)
			: key_(table.lua_, LUA_REGISTRYINDEX), value_(table.lua_, LUA_REGISTRYINDEX)
		{
			lua_State *lua = table.lua_;
			lua_checkstack(lua, 5);
			lua_pushlightuserdata(lua, this); /* this */
			lua_newtable(lua); /* this itr */
			lua_settable(lua, LUA_REGISTRYINDEX); /* */
			lua_pushlightuserdata(lua, this); /* this */
			lua_gettable(lua, LUA_REGISTRYINDEX); /* itr */
			lua_pushnumber(lua, 1); /* itr 1 */
			table.eval(); /* itr 1 table */
			lua_settable(lua, -3); /* itr */
			table.eval(); /* itr table */
			lua_pushnumber(lua, 2); /* itr table 2 */
			lua_pushnil(lua); /* itr table 2 nil */
			if(begin && lua_next(lua, -3)) {
				/* itr table 2 k v */
				lua_pop(lua, 1); /* itr table 2 k */
				lua_settable(lua, -4); /* itr table */
			}
			else {
				/* itr table 2 (nil)*/
				if(begin) lua_pushnil(lua); /* itr table 2 nil */ 
				lua_settable(lua, -4); /* itr table */
			}
			lua_pop(lua, 2); /* */
			key_.addKey(Key(this)).addKey(Key(2));
			value_.addKey(Key(this)).addKey(Key(1)).addKey(Key(key_));
		}
		~TableItr()
		{
			lua_checkstack(key_.lua_, 2);
			lua_pushlightuserdata(key_.lua_, this);
			lua_pushnil(key_.lua_);
			lua_settable(key_.lua_, LUA_REGISTRYINDEX);
		}

		TableItr &operator++() {
			increment();
			return *this;
		}
		TableItr &operator++(int) {
			increment();
			return *this;
		}
		const Table &operator*() const {
			return dereference();
		}
		const Table *operator->() const {
			return &dereference();
		}
		void increment() {
			lua_State *lua = key_.lua_;
			lua_checkstack(lua, 6);
			lua_pushlightuserdata(lua, this); /* this */
			lua_gettable(lua, LUA_REGISTRYINDEX); /* itr */
			lua_pushnumber(lua, 1); /* itr 1 */
			lua_gettable(lua, -2); /* itr table */
			lua_pushnumber(lua, 2); /* itr table 2 */
			lua_gettable(lua, -3); /* itr table key */
			if(lua_next(lua, -2)) {
				/* itr table k v */
				lua_pushnumber(lua, 2); /* itr table k v 2 */
				lua_pushvalue(lua, -3); /* itr table k v 2 k */
				lua_settable(lua, -6); /* itr table k v */
				lua_pop(lua, 4); /* */
			}
			else {
				/* itr table */
				lua_pushnumber(lua, 2); /* itr table 2 */
				lua_pushnil(lua); /* itr table 2 nil */
				lua_settable(lua, -4); /* itr table */
				lua_pop(lua, 2); /* */
			}
		}
		const Table &dereference() const {
			return value_;
		}
		const Table &key() const {
			return key_;
		}
	private:
		Table key_;
		Table value_;
	};

	inline bool operator==(const TableItr &lhs, const TableItr &rhs)
	{
		return lhs->isEqual(*lhs) && lhs.key().isEqual(rhs.key());
	}
	inline bool operator!=(const TableItr &lhs, const TableItr &rhs)
	{
		return !(lhs == rhs);
	}

	inline Table::iterator Table::begin(){ return iterator(*this, true); }
	inline Table::iterator Table::end()  { return iterator(*this, false); }

	/** UserData�̃L���X�g���Z�q
	 *
	 * �w�肵���^�̃|�C���^�[�^�ŕԂ�_�ɒ��ӁB
	 *
	 * Luna&luapp�O��
	 */
	template<typename T, typename V> T *luna_cast(const V &val) {
		return val.check(::Luna<T>::check);
	}

	/** lua_State(�X�^�b�N�Ƃ��Ă̋@�\�̂�)������킷�N���X
	 *
	 * @author SAM (T&GG, Org.) <sowwaNO-SPAM-THANKS@water.sannet.ne.jp>
	 * @date 2003/06/26 3:39:00
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) All rights reserved.
	 */
	class Stack {
	public:
		explicit Stack(lua_State *lua) : lua_(lua) {}

		operator lua_State *() { return lua_; }

		/** lua_State���I�[�v������Ă��邩�H
		 *
		 * @author SAM (T&GG, Org.) <sowwaNO-SPAM-THANKS@water.sannet.ne.jp>
		 * @date 2003/06/26 3:04:04
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) All rights reserved.
		 */
		inline bool isOpen() { return lua_ != 0; }

		/** �X�^�b�N�擪�̃C���f�b�N�X���擾
		 *
		 * @author SAM (T&GG, Org.) <sowwaNO-SPAM-THANKS@water.sannet.ne.jp>
		 * @date 2003/06/26 3:04:04
		 * $Id: lua.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) All rights reserved.
		 */
		inline int top()
		{
			return lua_gettop(lua_);
		}

		/*@{*/
		/** �X�^�b�N����֐� */

		 /** �X�^�b�N�擪��ݒ�
		  *
		  * @author SAM (T&GG, Org.) <sowwaNO-SPAM-THANKS@water.sannet.ne.jp>
		  * @date 2003/06/26 3:05:05
		  *
		  * Copyright (C) 2001,2002 SAM (T&GG, Org.) All rights reserved.
		  */
		inline void setTop(int index)
		{
			lua_settop(lua_, index);
		}
		
		inline void clear()
		{
			lua_settop(lua_, 0);
		}

		inline void pushIt(int index)
		{
			lua_checkstack(lua_, 1);
			lua_pushvalue(lua_, index);
		}

		inline void push()
		{
			lua_checkstack(lua_, 1);
			lua_pushnil(lua_);
		}

		inline void push(bool v)
		{
			lua_checkstack(lua_, 1);
			lua_pushboolean(lua_, v);
		}
		
		inline void push(lua_Number v)
		{
			lua_checkstack(lua_, 1);
			lua_pushnumber(lua_, v);
		}

		inline void push(int v)
		{
			lua_checkstack(lua_, 1);
			lua_pushnumber(lua_, static_cast<lua_Number>(v));
		}

		inline void push(const char *v, size_t len)
		{
			lua_checkstack(lua_, 1);
			lua_pushlstring(lua_, v, len);
		}
		
		inline void push(const std::string &v)
		{
			lua_checkstack(lua_, 1);
			lua_pushlstring(lua_, v.c_str(), v.size());
		}
		
		inline void push(const char *v)
		{
			lua_checkstack(lua_, 1);
			lua_pushstring(lua_, v);
		}
		
		inline void push(lua_CFunction &v)
		{
			lua_checkstack(lua_, 1);
			lua_pushcfunction(lua_, v);
		}
		
		inline void push(void *v)
		{
			lua_checkstack(lua_, 1);
			lua_pushlightuserdata(lua_, v);
		}

		inline void pushTable(const char *v, int index = LUA_GLOBALSINDEX)
		{
			lua_checkstack(lua_, 1);
			lua_pushstring(lua_, v);
			lua_gettable(lua_, index);
		}

		inline void getTable(int index = LUA_GLOBALSINDEX)
		{
			lua_checkstack(lua_, 1);
			lua_gettable(lua_, index);
		}

		inline void setTable(int index = LUA_GLOBALSINDEX)
		{
			lua_settable(lua_, index);
		}

		inline void pushRegistry(const char *v)
		{
			lua_checkstack(lua_, 1);
			lua_pushstring(lua_, v);
			lua_gettable(lua_, LUA_REGISTRYINDEX);
		}

		inline void getRegistry()
		{
			lua_checkstack(lua_, 1);
			lua_gettable(lua_, LUA_REGISTRYINDEX);
		}

		inline void setRegistry()
		{
			lua_settable(lua_, LUA_REGISTRYINDEX);
		}

		template<class T> Stack &operator << (T rhs)
		{
			push(rhs);
			return *this;
		}

		inline void pop(int n = 1)
		{
			lua_pop(lua_, n);
		}

		inline void remove(int index)
		{
			lua_remove(lua_, index);
		}

		inline void insert(int index)
		{
			lua_insert(lua_, index);
		}
		
		inline void replace(int index)
		{
			lua_replace(lua_, index);
		}
		
		inline void concat(int index)
		{
			lua_concat(lua_, index);
		}
		
		/** �X�^�b�N������
		 *
		 * @author SAM (T&GG, Org.) <sowwaNO-SPAM-THANKS@water.sannet.ne.jp>
		 * @date 2003/06/26 3:04:04
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) All rights reserved.
		 */
		inline bool extend(int extra)
		{
			return (lua_checkstack(lua_, extra))?true:false;
		}
		/*@} */
		
		Value get(int index = -1)
		{
			return Value(lua_, (index<0)?top()+1+index:index);
		}

		Value operator[] (int index)
		{
			return get(index);
		}

		Table global()
		{
			return Table(lua_, LUA_GLOBALSINDEX);
		}

		Table registry()
		{
			return Table(lua_, LUA_REGISTRYINDEX);
		}

		/*@{*/
		/** �X�^�b�N�₢���킹�֐� */

		Type type(int index)
		{
			return (Type)lua_type(lua_, index);
		}
		
		inline bool isNil(int index)
		{
			return lua_isnil(lua_, index);
		}

		inline bool isBoolean(int index)
		{
			return lua_isboolean(lua_, index);
		}

		inline bool isNumber(int index)
		{
			return (lua_isnumber(lua_, index))?true:false;
		}
		
		inline bool isString(int index)
		{
			return (lua_isstring(lua_, index))?true:false;
		}
		
		inline bool isTable(int index)
		{
			return lua_istable(lua_, index);
		}

		inline bool isFunction(int index)
		{
			return lua_isfunction(lua_, index);
		}
		
		inline bool isCFunction(int index)
		{
			return (lua_iscfunction(lua_, index))?true:false;
		}
		
		inline bool isUserData(int index)
		{
			return (lua_isuserdata(lua_, index))?true:false;
		}
		
		inline bool isLigntUserData(int index)
		{
			return lua_islightuserdata(lua_, index);
		}
		/*@}*/
		
		/// �ł��ŋ߂̃G���[���b�Z�[�W��\��
		void printAlert()
		{
			lua_getglobal(lua_, "_ALERT");
			if (lua_isfunction(lua_, -1)) {
				lua_insert(lua_, -2);
				lua_call(lua_, 1, 0);
			}
			else {  /* no _ALERT function; print it on stderr */
				fprintf(stderr, "%s\n", lua_tostring(lua_, -2));
				lua_pop(lua_, 2);  /* remove error message and _ALERT */
			}
		}
		
		// �Ăяo������
		static int thunk(lua_State *L) {
			// get function from upvalue
			Function f = static_cast<Function>(lua_touserdata(L, lua_upvalueindex(1)));
			Stack l(L);
			return f(l);  // call member function
		}

	protected:
		lua_State *lua_;
	};

	inline void Table::declare(const Key &key, Function f)
	{
		EvalAndPop _eval_(*this);
		key.push(lua_);
		lua_checkstack(lua_, 2);
		lua_pushlightuserdata(lua_, (void*)f);
		lua_pushcclosure(lua_, Stack::thunk, 1);
		key.set(lua_, _eval_.index());
	}

	inline void Table::declare(const char *name, Function f)
	{
		EvalAndPop _eval_(*this);
		lua_checkstack(lua_, 2);
		lua_pushstring(lua_, name);
		lua_pushlightuserdata(lua_, (void*)f);
		lua_pushcclosure(lua_, Stack::thunk, 1);
		lua_settable(lua_, _eval_.index());
	}

	/** Lua�Ƀ��C�u������o�^���邽�߂̒��ۊ֐��I�u�W�F�N�g
	 *
	 * @author SAM (T&GG, Org.) <sowwaNO-SPAM-THANKS@water.sannet.ne.jp>
	 * @date 2003/06/26 3:39:00
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) All rights reserved.
	 */
	typedef boost::function1<void, lua_State *> Initializer;

	/** lua_State(Lua�̉��z�}�V��)������킷�N���X
	 *
	 * @author SAM (T&GG, Org.) <sowwaNO-SPAM-THANKS@water.sannet.ne.jp>
	 * @date 2003/06/26 3:39:00
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) All rights reserved.
	 */
	class State : public Stack {
	public:
		State(bool do_open = false, lua_Alloc allocf = l_alloc) : Stack((do_open)?lua_newstate(allocf, this):0) { addref(); }
		State(lua_State *lua) : Stack(lua) { addref(); }
		State(const Initializer &ini, lua_Alloc allocf = l_alloc) : Stack(lua_newstate(allocf, this)) { addref(); ini(lua_); }
		State(const State &src) : Stack(src.lua_) { addref(); }
		explicit State(const char *filename, lua_Alloc allocf = l_alloc) : Stack(lua_newstate(allocf, this))
		{
			addref(); run(filename);
		}
		explicit State(const Initializer &ini, const char *filename, lua_Alloc allocf = l_alloc) : Stack(lua_newstate(allocf, this))
		{
			addref(); ini(lua_); run(filename);
		}
		explicit State(const std::string &str, const char *name = 0, lua_Alloc allocf = l_alloc) : Stack(lua_newstate(allocf, this))
		{
			addref(); run(str.c_str(), str.size(), name);
		}
		explicit State(const Initializer &ini, const std::string &str, const char *name = 0, lua_Alloc allocf = l_alloc) : Stack(lua_newstate(allocf, this))
		{
			addref(); ini(lua_); run(str.c_str(), str.size(), name);
		}
		explicit State(const char *buff, size_t size, const char *name = 0, lua_Alloc allocf = l_alloc) : Stack(lua_newstate(allocf, this))
		{
			addref(); run(buff, size, name);
		}
		explicit State(const Initializer &ini, const char *buff, size_t size, const char *name = 0, lua_Alloc allocf = l_alloc) : Stack(lua_newstate(allocf, this))
		{
			addref(); ini(lua_); run(buff, size, name);
		}
		~State() { release(); }

		void registerEnv(const Initializer &ini) { ini(lua_); }

		void open()
		{
			release();
			lua_ = lua_open();
			addref();
		}

		void close()
		{
			release();
			lua_ = 0;
		}

		bool load(const char *filename)
		{
			return (luaL_loadfile(lua_, filename))==0;
		}
		bool load(const std::string &str, const char *name = 0)
		{
			return (luaL_loadbuffer(lua_, str.c_str(), str.size(), name))==0;
		}
		bool load(const char *buff, size_t size, const char *name = 0)
		{
			return (luaL_loadbuffer(lua_, buff, size, name))==0;
		}

		static void callalert(lua_State *L, int status)
		{
			if (status != 0) {
				lua_getglobal(L, "_ALERT");
				if (lua_isfunction(L, -1)) {
					lua_insert(L, -2);
					lua_call(L, 1, 0);
				}
				else {  /* no _ALERT function; print it on stderr */
					fprintf(stderr, "%s\n", lua_tostring(L, -2));
					lua_pop(L, 2);  /* remove error message and _ALERT */
				}
			}
		}

		static bool aux_do(lua_State *L, int status)
		{
			if (status == 0) {  /* parse OK? */
				status = lua_pcall(L, 0, LUA_MULTRET, 0);  /* call main */
			}
			callalert(L, status);
			return status == 0;
		}

		static void *l_alloc(void *, void *ptr, size_t, size_t nsize)
		{
			if(nsize == 0) {
				::free(ptr);
				return 0;
			}
			else return ::realloc(ptr, nsize);
		}

		inline bool run(const char *filename)
		{
			return aux_do(lua_, luaL_loadfile(lua_, filename));
		}
		inline bool run(const std::string &str)
		{
			return aux_do(lua_, luaL_loadstring(lua_, str.c_str()));
		}
		inline bool run(const char *buff, size_t size, const char *name = 0)
		{
			return aux_do(lua_, luaL_loadbuffer(lua_, buff, size, name));
		}

		inline bool main()
		{
			int status = lua_pcall(lua_, 0, LUA_MULTRET, 0);  /* call main */
			if(status != 0) printAlert();
			return status==0;
		}

		State & operator=(State const & rhs)
		{
			State(rhs).swap(*this);
			return *this;
		}
		
		State & operator=(lua_State * rhs)
		{
			State(rhs).swap(*this);
			return *this;
		}

		template<class T> State &operator << (T rhs)
		{
			push(rhs);
			return *this;
		}

		inline void declare(const char *name, lua_CFunction f)
		{
			lua_register(lua_, name, f);
		}

		inline void declare(const char *name, Function f)
		{
			lua_checkstack(lua_, 2);
			lua_pushstring(lua_, name);
			lua_pushlightuserdata(lua_, (void*)f);
			lua_pushcclosure(lua_, thunk, 1);
			lua_settable(lua_, LUA_GLOBALSINDEX);
		}

		inline void call(int nargs, int nresults)
		{
			lua_call(lua_, nargs, nresults);
		}

		int refcount() {
			if(isOpen()) {
				pushRegistry("LUAPP_REF");
				int ret = get();
				pop();
				return ret;
			}
			return -1;
		}

	private:
		void swap(State &rhs)
		{
			lua_State * tmp = lua_;
			lua_ = rhs.lua_;
			rhs.lua_ = tmp;
		}
		void addref()
		{
			if(isOpen()) {
				pushRegistry("LUAPP_REF");
				int ref = get();
				ref += 1;
				pop();
				push("LUAPP_REF");
				push(ref);
				setRegistry();
			}
		}
		void release()
		{
			if(isOpen()) {
				pushRegistry("LUAPP_REF");
				int ref = get();
				ref -= 1;
				pop();
				push("LUAPP_REF");
				push(ref);
				setRegistry();
				if(ref==0) {
					lua_close(lua_);
					lua_ = 0;
				}
			}
		}
	};
	
	/** Lua�̊֐��Ăяo���̂��߂̈����Z�b�g�A�Ԃ�l���o���̂��߂̃R�[���o�b�N�֐��I�u�W�F�N�g
	 *
	 * void operator(Stack &L)���I�[�o�[���C�h���āA�֐��Ăяo���̂��߂̈�����ςނ��A
	 * �Ԃ�l�̒l�����o���B
	 * �����̐��A�߂�l�̐���length���\�b�h���I�[�o�[���C�h���邱�Ƃŕ񍐂���B
	 * �\����������̐��́A���m�Ɉ����̐��ƈ�v���Ă��Ȃ���΂Ȃ�Ȃ��B
	 * �߂�l�́A���o�������Ń|�b�v����K�v�͂Ȃ��B�����Lua�֐��Ăяo�����\�b�h�̓����ŁA
	 * length���\�b�h�ł̐\�������|�b�v����B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/17 18:04:24
	 *
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Callback {
	public:
		/** Lua�̊֐��Ăяo���̂��߂̈����Z�b�g�܂��͕Ԃ�l���o��
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/17 18:04:24
		 *
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		virtual void operator()(Stack &/*L*/) {}
		/** �����܂��͕Ԃ�l�̐���\��
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>S
		 * @date 2004/02/17 18:04:24
		 *
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		virtual int length() { return 0; }
		
		static Callback &dummy()
		{
			static Callback _dummy;
			return _dummy;
		}
	};
	
	/** lua��̊֐����Ăяo��
	 *
	 * @code
		luapp::Return1<bool> ret;
		luapp::Argument2<int, int> args(3, 4);
		L.global()["f"](ret, args);
		dbgout << ret.ret1; // <- �߂�l����������
	 * @endcode
	 */
	inline bool Value::operator()(Callback &rets = Callback::dummy(), Callback &args = Callback::dummy())
	{
		EvalAndPop _eval_(*this);
		lua_checkstack(lua_, 1+(std::max)(args.length(), rets.length()));
		lua_pushvalue(lua_, _eval_.index());
		Stack L(lua_);
		args(L);
/*		int status = lua_pcall(lua_, args.length(), rets.length(), 0);
		if(status != 0) {
			L.printAlert();
			return false;
		}*/
		lua_call(lua_, args.length(), rets.length());
		rets(L);
		if(rets.length()>0) lua_pop(lua_, rets.length());
		return true;
	}
	
	/** lua�֐��Ăяo���̖߂�l�󂯎��R�[���o�b�N���`����e���v���[�g
	 *
	 */
	template<typename _Type1>
	class Return1 : public Callback
	{
	public:
		_Type1 ret1;
		virtual int length() { return 1; }
		virtual void operator()(Stack &L)
		{
			ret1 = L[-1];
		}
	};
	/** lua�֐��Ăяo���̖߂�l�󂯎��R�[���o�b�N���`����e���v���[�g
	 *
	 */
	template<typename _Type1, typename _Type2>
	class Return2 : public Callback
	{
	public:
		_Type1 ret1;
		_Type2 ret2;
		virtual int length() { return 2; }
		virtual void operator()(Stack &L)
		{
			ret1 = L[-2];
			ret2 = L[-1];
		}
	};
	/** lua�֐��Ăяo���̖߂�l�󂯎��R�[���o�b�N���`����e���v���[�g
	 *
	 */
	template<typename _Type1, typename _Type2, typename _Type3>
	class Return3 : public Callback
	{
	public:
		_Type1 ret1;
		_Type2 ret2;
		_Type3 ret3;
		virtual int length() { return 3; }
		virtual void operator()(Stack &L)
		{
			ret1 = L[-3];
			ret2 = L[-2];
			ret3 = L[-1];
		}
	};
	
	/** lua�֐��Ăяo���̈����󂯓n���R�[���o�b�N���`����e���v���[�g
	 *
	 */
	template<typename _Type1>
	class Argument1 : public Callback
	{
	public:
		Argument1(_Type1 arg1) : arg1(arg1) {}
		virtual int length() { return 1; }
		virtual void operator()(Stack &L)
		{
			L << arg1;
		}
	private:
		_Type1 arg1;
	};
	/** lua�֐��Ăяo���̈����󂯓n���R�[���o�b�N���`����e���v���[�g
	 *
	 */
	template<typename _Type1, typename _Type2>
	class Argument2 : public Callback
	{
	public:
		Argument2(_Type1 arg1, _Type2 arg2) : arg1(arg1), arg2(arg2) {}
		virtual int length() { return 2; }
		virtual void operator()(Stack &L)
		{
			L << arg1 << arg2;
		}
	private:
		_Type1 arg1;
		_Type2 arg2;
	};
	/** lua�֐��Ăяo���̈����󂯓n���R�[���o�b�N���`����e���v���[�g
	 *
	 */
	template<typename _Type1, typename _Type2, typename _Type3>
	class Argument3 : public Callback
	{
	public:
		Argument3(_Type1 arg1, _Type2 arg2, _Type3 arg3) : arg1(arg1), arg2(arg2), arg3(arg3) {}
		virtual int length() { return 3; }
		virtual void operator()(Stack &L)
		{
			L << arg1 << arg2 << arg3;
		}
	private:
		_Type1 arg1;
		_Type2 arg2;
		_Type3 arg3;
	};
	/** lua�֐��Ăяo���̈����󂯓n���R�[���o�b�N���`����e���v���[�g
	 *
	 */
	template<typename _Type1, typename _Type2, typename _Type3, typename _Type4>
	class Argument4 : public Callback
	{
	public:
		Argument4(_Type1 arg1, _Type2 arg2, _Type3 arg3, _Type4 arg4) : arg1(arg1), arg2(arg2), arg3(arg3), arg4(arg4) {}
		virtual int length() { return 4; }
		virtual void operator()(Stack &L)
		{
			L << arg1 << arg2 << arg3 << arg4;
		}
	private:
		_Type1 arg1;
		_Type2 arg2;
		_Type3 arg3;
		_Type4 arg4;
	};
	/** lua�֐��Ăяo���̈����󂯓n���R�[���o�b�N���`����e���v���[�g
	 *
	 */
	template<typename _Type1, typename _Type2, typename _Type3, typename _Type4, typename _Type5>
	class Argument5 : public Callback
	{
	public:
		Argument5(_Type1 arg1, _Type2 arg2, _Type3 arg3, _Type4 arg4, _Type5 arg5) : arg1(arg1), arg2(arg2), arg3(arg3), arg4(arg4), arg5(arg5) {}
		virtual int length() { return 5; }
		virtual void operator()(Stack &L)
		{
			L << arg1 << arg2 << arg3 << arg4 << arg5;
		}
	private:
		_Type1 arg1;
		_Type2 arg2;
		_Type3 arg3;
		_Type4 arg4;
		_Type5 arg5;
	};
	
	/** �o�^������Z�߂��֐��I�u�W�F�N�g
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/17 18:04:24
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Initializers {
		std::vector<Initializer> initializers_;
		static void dummy(lua_State *) {}
	public:
		Initializers() {}
		Initializers(Initializer const &i0,
			Initializer const &i1 = dummy, Initializer const &i2 = dummy, Initializer const &i3 = dummy,
			Initializer const &i4 = dummy, Initializer const &i5 = dummy, Initializer const &i6 = dummy,
			Initializer const &i7 = dummy, Initializer const &i8 = dummy, Initializer const &i9 = dummy )
		{
			(*this) << i0 << i1 << i2 << i3 << i4 << i5 << i6 << i7 << i8 << i9;
		}

		Initializers &operator<<(Initializer const &initializer)
		{
			initializers_.push_back(initializer);
			return *this;
		}

		void operator()(lua_State *lua) const
		{
			for(std::vector<Initializer>::const_iterator i = initializers_.begin(); i != initializers_.end(); ++i)
			{
				if(*i) (*i)(lua);
			}
		}
	};

} // namespace luapp

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //_LUA_HPP_
