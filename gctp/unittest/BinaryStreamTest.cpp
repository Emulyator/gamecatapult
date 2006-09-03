#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#ifndef __ee__
#include <gctp/bfstream.hpp>
#endif
#include <gctp/bbstream.hpp>
#define _GCTP_ZFILTER_HPP_
#include <gctp/zfilter.hpp>
#include <gctp/archive.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#undef GCTP_USE_ZLIB

//CUPPA:namespace=+
namespace util {
//CUPPA:namespace=-
using namespace gctp;

class BinaryStreamTest : public CppUnit::TestFixture {
private:
  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
#ifndef __ee__
  void test_iobfstream() {
	  {
		  obfstream bos("iobfstreamtest");
		  CPPUNIT_ASSERT(bos.is_open());
		  int i = 0xDEADBEAF;
		  float f = 3.14f;
		  const char *str = "bstreamtest";
		  bos << i << f << str;
	  }
	  {
		  ibfstream bis("iobfstreamtest");
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
	  bfstream bs("bfstreamtest", std::ios::trunc);
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
#endif
  void test_bbstream() {
	  bbstream bs;
	  {
		  const char *str = "bstreamtest";
		  float f = 3.14f;
		  int i = 0xDEADBEAF;
		  bs << str << f << i;
	  }
	  bs.seekg(0);
	  {
		  int i = 0;
		  float f = 0;
		  std::string str;
		  bs >> str >> f >> i;
		  CPPUNIT_ASSERT(i == 0xDEADBEAF);
		  CPPUNIT_ASSERT(f == 3.14f);
		  CPPUNIT_ASSERT(std::string(str) == "bstreamtest");
	  }
  }
  void test_strstream() {
	  std::strstream bs;
	  {
		  int i = 0xDEADBEAF;
		  float f = 3.14f;
		  const char *str = "bstreamtest";
		  bs << i << std::endl << f << std::endl << str << std::endl;
	  }
	  bs.seekg(0);
	  {
		  int i = 0;
		  float f = 0;
		  std::string str;
		  bs >> i >> f >> str;
		  CPPUNIT_ASSERT(i == 0xDEADBEAF);
#ifdef __ee__
		  CPPUNIT_ASSERT_DOUBLES_EQUAL(f, 3.14f, 0.000001f);
#else
		  CPPUNIT_ASSERT(f == 3.14f);
#endif
		  CPPUNIT_ASSERT(std::string(str) == "bstreamtest");
	  }
  }
#ifdef GCTP_USE_ZLIB
  void test_zfilter() {
    const char *test = 
		"à≥èkÉXÉgÉäÅ[ÉÄÉeÉXÉgÇ†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†Ç†";
	// à≥èk
    {
		obfstream ofs("bzfiltertest");
		CPPUNIT_ASSERT(ofs.is_open());
        gctp::obzfilter ozf(ofs);

		ozf << test;
    }

	{
		File f("bzfiltertest");
		CPPUNIT_ASSERT((unsigned)f.length()<(strlen(test)+12));
		std::string str;
		f >> str;
		CPPUNIT_ASSERT(str!=test);
	}

    // êLí∑
    {
		ibfstream ifs("bzfiltertest");
		CPPUNIT_ASSERT(ifs.is_open());
        gctp::ibzfilter izf(ifs);

		std::string str;
		izf >> str;
		CPPUNIT_ASSERT(str == test);
    }
  }
#endif // GCTP_USE_ZLIB
//CUPPA:decl=-
  CPPUNIT_TEST_SUITE(BinaryStreamTest);
//CUPPA:suite=+
#ifndef __ee__
  CPPUNIT_TEST(test_iobfstream);
  CPPUNIT_TEST(test_bfstream);
#endif
  CPPUNIT_TEST(test_bbstream);
  CPPUNIT_TEST(test_strstream);
  CPPUNIT_TEST(test_strstream);
#ifdef GCTP_USE_ZLIB
  CPPUNIT_TEST(test_zfilter);
#endif // GCTP_USE_ZLIB
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace util {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(BinaryStreamTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(BinaryStreamTest,"util");

}
#if defined _MSC_VER && defined GCTP_USE_ZLIB
# ifdef _DEBUG
#  pragma comment(lib, "zlibd.lib")
# else
#  pragma comment(lib, "zlib.lib")
# endif
#endif
