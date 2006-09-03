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

class BinaryStreamTest : public CppUnit::TestFixture {
private:
  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
  void test_iobfstream() {
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
  void test_bfstream() {
	  bfstream bs("fbstreamtest", std::ios::trunc);
	  CPPUNIT_ASSERT(bs.is_open());
	  {
		  int i = 0xDEADBEAF;
		  float f = 3.14f;
		  const char *str = "bstreamtest";
		  bs << i << f << str;
	  }
	  bs.flush();
	  bs.seekg(0);
	  {
		  int i = 0;
		  float f = 0;
		  std::string str;
		  bs >> i >> f >> str;
		  CPPUNIT_ASSERT(i == 0xDEADBEAF);
		  CPPUNIT_ASSERT(f == 3.14f);
		  CPPUNIT_ASSERT(std::string(str) == "bstreamtest");
	  }
  }
//CUPPA:decl=-
  CPPUNIT_TEST_SUITE( BinaryStreamTest );
//CUPPA:suite=+
  CPPUNIT_TEST(test_iobfstream);
  CPPUNIT_TEST(test_bfstream);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace gctp {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(BinaryStreamTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(BinaryStreamTest,"gctp");

}
