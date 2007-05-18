#ifdef _MSC_VER
#	pragma warning(disable:4786 4503)
#endif
//CUPPA:include=+
#include <gctp/bfstream.hpp>
#include <gctp/bbstream.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

//CUPPA:namespace=+
namespace gctp {
//CUPPA:namespace=-

class ArchiveTest : public CppUnit::TestFixture {
private:
  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
  void test_zip() {
	  {
		  obfstream bos("iofbstreamtest");
		  CPPUNIT_ASSERT(bos.is_open());
		  int i = 0xDEADBEAF;
		  float f = 3.14f;
		  const char *str = "bstreamtest";
		  bos << i << f << str;
	  }
	  {
		  ibfstream bis("iofbstreamtest");
		  CPPUNIT_ASSERT(bis.is_open());
		  int i = 0;
		  float f = 0;
		  std::string str;
		  bis >> i >> f >> str;
		  CPPUNIT_ASSERT(i == 0xDEADBEAF);
		  CPPUNIT_ASSERT(f == 3.14f);
		  CPPUNIT_ASSERT(std::string(str) == "bstreamtest");
	  }
  }
//CUPPA:decl=-
  CPPUNIT_TEST_SUITE( ArchiveTest );
//CUPPA:suite=+
  CPPUNIT_TEST(test_zip);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace gctp {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(ArchiveTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ArchiveTest,"gctp");

}
