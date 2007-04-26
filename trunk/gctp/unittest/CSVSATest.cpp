#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <gctp/csvsa.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <fstream>
#include <strstream>
#include "iffreader.h"

//CUPPA:namespace=+
namespace util {
//CUPPA:namespace=-

using namespace std;

namespace {

  struct _Foo2 {
	  int i1, i2, i3;
  };
  struct _Foo {
	  short  _short;
	  int    _long;
	  char  *str;
	  char   str2[12];
	  _Foo2  *foo2;
  };
  struct Head {
	  int num;
	  _Foo foo[];
  };

}

class CSVSATest : public CppUnit::TestFixture {
private:

  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
  void testSaveAndRead() {
	  ostrstream ss;
	  ifstream ifs("../../../csvsac/test2.csv");
      CPPUNIT_ASSERT(ifs.is_open());
	  gctp::CSVSAChunk::parse(ifs, ss);
	  CPPUNIT_ASSERT(ss.pcount() > 0);
	  iffParseROPS(ss.str(), ss.pcount());
	  Head *head = (Head *)(ss.str()+8);
	  CPPUNIT_ASSERT(head->num == 11);
	  CPPUNIT_ASSERT(head->foo[0]._short == 11);
	  CPPUNIT_ASSERT(head->foo[0]._long == 1212);
	  CPPUNIT_ASSERT(head->foo[10]._short == 1120);
	  CPPUNIT_ASSERT(head->foo[10]._long == 1222);
  }
//CUPPA:decl=-
  CPPUNIT_TEST_SUITE(CSVSATest);
//CUPPA:suite=+
  CPPUNIT_TEST(testSaveAndRead);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace util {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(CSVSATest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CSVSATest,"util");

}
