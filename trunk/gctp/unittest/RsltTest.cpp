#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します
#include <windows.h>
#include <gctp/hrslt.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <iostream>

//CUPPA:namespace=+
namespace util {
//CUPPA:namespace=-

using namespace gctp;

class RsltTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(RsltTest);
//CUPPA:suite=+
  CPPUNIT_TEST(test1);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
private:

  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
  void test1() {
    //CPPUNIT_FAIL("no implementation");
	  HRslt hr;
	  CPPUNIT_ASSERT(hr);
	  hr = 0x88768686;
#ifdef UNICODE
	  std::wcout << hr << L"  ここは'Unknown : n/a'と出れば正常" << std::endl;
#else
	  std::cout << hr << "  ここは'Unknown : n/a'と出れば正常" << std::endl;
#endif
	  CPPUNIT_ASSERT(!hr);
	  hr = 0;
	  CPPUNIT_ASSERT(hr);
	  bool if_cond = false;
	  if(hr) if_cond = true;
	  CPPUNIT_ASSERT(if_cond);
  }
//CUPPA:decl=-
};

}

namespace util {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(RsltTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(RsltTest,"util");

}
