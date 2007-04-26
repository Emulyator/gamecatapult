#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <lua.hpp>
#include <luna.hpp>
#include <gctp/class.hpp>
#include <gctp/object.hpp>
#include <gctp/tuki.hpp>
#include <gctp/dbgout.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <iostream>
#include <float.h>

using namespace std;

//CUPPA:namespace=+
namespace lua {
//CUPPA:namespace=-

using namespace gctp;

namespace {
	int average(lua_State *L) {
		int n = lua_gettop(L);               /* 引数の数 */
		lua_Number sum = 0;
		int i;
		for (i = 1; i <= n; i++) {
			if (!lua_isnumber(L, i)) {
				 lua_pushstring(L, "関数`average´の引数が正しくない");
				 lua_error(L);
			}
			sum += lua_tonumber(L, i);
		}
		lua_pushnumber(L, sum/n);        /* 最初の戻り値 */
		lua_pushnumber(L, sum);        /* 二番目の戻り値 */
		return 2;                          /* 戻り値の数 */
	}

	class Account {
	public:
		lua_Number m_balance;

		LUNA_DECLARE(Account);

		Account(lua_State *L)      { m_balance = luaL_checknumber(L, 1); }
		int deposit (lua_State *L) { m_balance += luaL_checknumber(L, 1); return 0; }
		int withdraw(lua_State *L) { m_balance -= luaL_checknumber(L, 1); return 0; }
		int balance (lua_State *L) { lua_pushnumber(L, m_balance); return 1; }
		int test1(lua_State *L) {
#ifndef __ee__
			CPPUNIT_ASSERT_DOUBLES_EQUAL(m_balance,125.2,FLT_EPSILON);
#endif
			return 0;
		}
		int test2(lua_State *L) {
#ifndef __ee__
			CPPUNIT_ASSERT_DOUBLES_EQUAL(m_balance,30.0,FLT_EPSILON);
#endif
			return 0;
		}
		~Account() { printf("deleted Account (%p)\n", this); }
	};

	LUNA_IMPLEMENT_BEGIN(Account)
		LUNA_METHOD(Account, deposit)
		LUNA_METHOD(Account, withdraw)
		LUNA_METHOD(Account, balance)
		LUNA_METHOD(Account, test1)
		LUNA_METHOD(Account, test2)
	LUNA_IMPLEMENT_END;

	class Foo : public Object
	{
	public:
		~Foo() {
			cout << "~名無し" << endl;
			//cout << "~FooLua" << endl;
		}
		static bool done_;
		static bool done2_;
		lua_Number data_;
		bool setUp(luapp::Stack &L) { return true; }
		int deposit (luapp::Stack &L) { data_ += L[1].toNumber(); return 0; }
		int withdraw(luapp::Stack &L) { data_ -= L[1].toNumber(); return 0; }
		int test(luapp::Stack &L) {
			done_ = true;
			cout << "test : " << data_ << endl;
			CPPUNIT_ASSERT_DOUBLES_EQUAL(data_,223,FLT_EPSILON);
			return 0;
		}
		static int static_test(luapp::Stack &L) {
			done2_ = true;
			return 0;
		}
	GCTP_DECLARE_CLASS
	TUKI_DECLARE(Foo)

	protected:
		Foo() : data_(123) {}
	};
	bool Foo::done_ = false;
	bool Foo::done2_ = false;
	GCTP_IMPLEMENT_CLASS(Foo, Object);
	TUKI_IMPLEMENT_BEGIN_EX("モジュール.名無し", Foo)
		TUKI_METHOD(Foo, deposit)
		TUKI_METHOD(Foo, withdraw)
		TUKI_METHOD(Foo, test)
		TUKI_METHOD(Foo, static_test)
	TUKI_IMPLEMENT_END(Foo)

	luapp::Initializers registers(TukiInitializer(), Luna<Account>::registerMe);
}

class LuaTest : public CppUnit::TestFixture {
private:

  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
  void testStack() {
	  //cout.imbue(locale(""));
	  cout << "########test0 begin###########" << endl;
	  cout << "ななし" << endl;
	  cout << "名無し" << endl;
	  cout << "######## test0 end ###########" << endl;
	  cout << "######## test0 end ###########" << endl;
	  cout << "######## test0 end ###########" << endl;
	  cout << "######## test0 end ###########" << endl;
	  cout << "######## test0 end ###########" << endl;
	  cout << "######## test0 end ###########" << endl;
	  cout << "######## test0 end ###########" << endl;
	  cout << "######## test0 end ###########" << endl;
	  printf("########test1 begin###########\n");
	  printf("ななし\n");
	  printf("名無し\n");
	  printf("######## test1 end ###########\n");
	  dbgout << "########test0 begin###########" << endl;
	  dbgout << "ななし" << endl;
	  dbgout << "名無し" << endl;
	  dbgout << "######## test0 end ###########" << endl;

	  luapp::State lua(true);
	  lua.declare("average", average);
	  lua.pushTable("average");
	  lua << 1 << 2 << 3 << 4 << 5;
	  lua.call(5, 2);
	  CPPUNIT_ASSERT(lua[-1].isNumber());
	  CPPUNIT_ASSERT(lua[-2].isNumber());
	  CPPUNIT_ASSERT(lua[-1].toNumber() == 15);
	  CPPUNIT_ASSERT(lua[-2].toNumber() == 3);
	  lua << "ave" << "sum";
	  lua.insert(-4);
	  lua.insert(-2);
	  lua.setTable();
	  lua.setTable();
	  CPPUNIT_ASSERT(lua.top() == 0);
	  lua.pushTable("ave");
	  lua.pushTable("sum");
	  CPPUNIT_ASSERT(lua[-1].isNumber());
	  CPPUNIT_ASSERT(lua[-2].isNumber());
	  CPPUNIT_ASSERT(lua[-1].toNumber() == 3);
	  CPPUNIT_ASSERT(lua[-2].toNumber() == 15);
	  lua.pop(2);
	  CPPUNIT_ASSERT(lua.top() == 0);
  }
  void testTable() {
	luapp::State lua(std::string("a = {d = 20, e = 'field', 15, 16, w = {23, 11}}"));
	int _c = 0;
	for(luapp::Table::iterator i = lua.global()["a"].begin(); i != lua.global()["a"].end(); ++i) {
		dbgout << i.key() << "," << *i << endl;
		switch(_c) {
		case 0:
			CPPUNIT_ASSERT(i.key().isNumber());
			CPPUNIT_ASSERT(i.key().toNumber() == 1);
			CPPUNIT_ASSERT((*i).isNumber());
			CPPUNIT_ASSERT((*i).toNumber() == 15);
			break;
		case 1:
			CPPUNIT_ASSERT(i.key().isNumber());
			CPPUNIT_ASSERT(i.key().toNumber() == 2);
			CPPUNIT_ASSERT((*i).isNumber());
			CPPUNIT_ASSERT((*i).toNumber() == 16);
			break;
		case 2:
			CPPUNIT_ASSERT(i.key().isString());
			CPPUNIT_ASSERT(i.key().toString() == "e");
			CPPUNIT_ASSERT((*i).type() == luapp::STRING);
			CPPUNIT_ASSERT((*i).toString() == "field");
			break;
		case 3:
			CPPUNIT_ASSERT(i.key().isString());
			CPPUNIT_ASSERT(i.key().toString() == "d");
			CPPUNIT_ASSERT((*i).type() == luapp::NUMBER);
			CPPUNIT_ASSERT((*i).toNumber() == 20);
			break;
		case 4:
			CPPUNIT_ASSERT(i.key().isString());
			CPPUNIT_ASSERT(i.key().toString() == "w");
			CPPUNIT_ASSERT((*i).isTable());
			break;
		}
		_c++;
	}
	CPPUNIT_ASSERT(_c == 5);

	_c = 0;
	for(luapp::Table::AltIterator itr(lua.global()["a"]); itr.next();) {
		dbgout << lua[-2] << "," << lua[-1] << endl;
		switch(_c) {
		case 0:
			CPPUNIT_ASSERT(itr.key().isNumber());
			CPPUNIT_ASSERT(itr.key().toNumber() == 1);
			CPPUNIT_ASSERT(itr.value().isNumber());
			CPPUNIT_ASSERT(itr.value().toNumber() == 15);
			break;
		case 1:
			CPPUNIT_ASSERT(itr.key().isNumber());
			CPPUNIT_ASSERT(itr.key().toNumber() == 2);
			CPPUNIT_ASSERT(itr.value().isNumber());
			CPPUNIT_ASSERT(itr.value().toNumber() == 16);
			break;
		case 2:
			CPPUNIT_ASSERT(itr.key().isString());
			CPPUNIT_ASSERT(itr.key().toString() == "e");
			CPPUNIT_ASSERT(itr.value().type() == luapp::STRING);
			CPPUNIT_ASSERT(itr.value().toString() == "field");
			break;
		case 3:
			CPPUNIT_ASSERT(itr.key().isString());
			CPPUNIT_ASSERT(itr.key().toString() == "d");
			CPPUNIT_ASSERT(itr.value().type() == luapp::NUMBER);
			CPPUNIT_ASSERT(itr.value().toNumber() == 20);
			break;
		case 4:
			CPPUNIT_ASSERT(itr.key().isString());
			CPPUNIT_ASSERT(itr.key().toString() == "w");
			CPPUNIT_ASSERT(itr.value().isTable());
			break;
		}
		_c++;
	}
	CPPUNIT_ASSERT(_c == 5);
	CPPUNIT_ASSERT(lua.top()==0);

	lua.global()["a"].declare("average", average);
	lua.run(string("i = a.average(1, 2, 3, 4, 5)"));
	CPPUNIT_ASSERT(lua.global()["i"].toNumber() == 3);
	CPPUNIT_ASSERT(lua.top() == 0);
  }
  void testScript() {
	luapp::State lua(std::string("a = 10 b = 'string' c = {d = 20, e = 'field'} f = {15, 40} g = { {x = 1}, {y = 2} }"));
	CPPUNIT_ASSERT(lua.global()["a"].toNumber() == 10);
	CPPUNIT_ASSERT(lua.global()["b"].toString() == "string");
	luapp::Table t = lua.global()["c"];
	CPPUNIT_ASSERT(t["d"].toNumber() == 20);
	CPPUNIT_ASSERT(t["e"].toString() == "field");
	t = lua.global()["f"];
	luapp::Table w = t; 
	CPPUNIT_ASSERT(w[1].toNumber() == 15);
	CPPUNIT_ASSERT(w[2].toNumber() == 40);
	CPPUNIT_ASSERT(lua.global()["g"][1]["x"].toNumber() == 1);
	CPPUNIT_ASSERT(lua.global()["g"][2]["y"].toNumber() == 2);
	dbgout << "top = " << lua.top() << std::endl;
	CPPUNIT_ASSERT(lua.top() == 0);
  }
  void testCallFunction() {
	luapp::State lua(std::string("function f(x, y) return x + y end"));
	luapp::Return1<lua_Number> ret;
	luapp::Argument2<lua_Number, lua_Number> arg(12, 24);
	lua.global()["f"](ret, arg);
	CPPUNIT_ASSERT(ret.ret1 == 36);
  }
  void testLua() {
	luapp::State lua1;
	CPPUNIT_ASSERT(!lua1.isOpen());
	luapp::State lua2(true);
	CPPUNIT_ASSERT(lua2.isOpen());
	lua1 = lua2;
	CPPUNIT_ASSERT(lua1.isOpen() && lua2.isOpen());
	lua_State *luap1 = lua1, *luap2 = lua2;
	CPPUNIT_ASSERT(luap1 == luap2);
	CPPUNIT_ASSERT(lua1.refcount() == 2);
	CPPUNIT_ASSERT(lua2.refcount() == 2);
	lua1 = NULL;
	CPPUNIT_ASSERT(!lua1.isOpen() && lua2.isOpen());
	CPPUNIT_ASSERT(lua2.refcount() == 1);
  }
  void testLuna() {
#ifdef __ee__
#define PREFIX "host0:"
#else
#define PREFIX "../../../unittest/"
#endif
	luapp::State lua(registers, PREFIX"account.lua");
	Account *a = luapp::luna_cast<Account>(lua.global()["a"]);
#ifndef __ee__
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a->m_balance,125.2,FLT_EPSILON);
#else
	CPPUNIT_ASSERT_DOUBLES_EQUAL(a->m_balance,125.2,0.001);
	Account *b = luapp::luna_cast<Account>(lua.global()["b"]);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(b->m_balance,30.0,FLT_EPSILON);
#endif
	dbgout << PREFIX"account.lua end" << endl;
  }
  void testTuki() {
	luapp::State lua(registers);
	bool lua_run_ok = lua.run(string(
		"register('モジュール')\n"
		"print('こんにちわ！')\n"
		"a = モジュール.名無し()\n"
		"a:deposit(100)\n"
		"a.static_test()\n"
		"a:test()\n"
		"for k, v in pairs(getmetatable(モジュール.名無し)) do\n"
		" print(k, v)\n"
		"end"
	));
	if(!lua_run_ok) dbgout << lua[0].toCStr() << endl;
	CPPUNIT_ASSERT(lua_run_ok);

	CPPUNIT_ASSERT(Foo::done2_);
	CPPUNIT_ASSERT(Foo::done_);
	CPPUNIT_ASSERT(lua.global()["a"].toUserData());
	Pointer<Foo> luaa = tuki_cast<Foo>(lua.global()["a"]);
	CPPUNIT_ASSERT(luaa);
	CPPUNIT_ASSERT(luaa->done_);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(luaa->data_,223,FLT_EPSILON);
	dbgout << "end of testTuki" << endl;
  }
  void testTukiDump() {
	TukiInitializer ini;
	{
		luapp::State lua(ini);
		bool lua_run_ok = lua.run(string(
			"register('モジュール')\n"
			"t = {a = 10, b = true, o = モジュール.名無し(), tt = {ee = 'wwww'}, c = 'abracadabra'}\n"
			"tuki_dump('tukidumptest', t)\n"
			));
		CPPUNIT_ASSERT(lua_run_ok);
	}
	{
		luapp::State lua(ini);
		bool lua_run_ok = lua.run(string(
			"t = {}\n"
			"tuki_load('tukidumptest', t)\n"
			));
		CPPUNIT_ASSERT(lua_run_ok);
		CPPUNIT_ASSERT(lua.global()["t"]["a"].isNumber());
		CPPUNIT_ASSERT(lua.global()["t"]["a"].toNumber() == 10);
		CPPUNIT_ASSERT(lua.global()["t"]["b"].isBoolean());
		CPPUNIT_ASSERT(lua.global()["t"]["b"].toBoolean() == true);
		CPPUNIT_ASSERT(lua.global()["t"]["o"].isUserData());
		CPPUNIT_ASSERT(lua.global()["t"]["tt"].isTable());
		CPPUNIT_ASSERT(lua.global()["t"]["tt"]["ee"].isString());
		CPPUNIT_ASSERT(lua.global()["t"]["tt"]["ee"].toString() == "wwww");
		CPPUNIT_ASSERT(lua.global()["t"]["c"].isString());
		CPPUNIT_ASSERT(lua.global()["t"]["c"].toString() == "abracadabra");
	}
  }
//CUPPA:decl=-
  CPPUNIT_TEST_SUITE(LuaTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testStack);
  CPPUNIT_TEST(testTable);
  CPPUNIT_TEST(testScript);
  CPPUNIT_TEST(testCallFunction);
  CPPUNIT_TEST(testLua);
  CPPUNIT_TEST(testLuna);
  CPPUNIT_TEST(testTuki);
  CPPUNIT_TEST(testTukiDump);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace lua {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(LuaTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(LuaTest,"lua");

}
