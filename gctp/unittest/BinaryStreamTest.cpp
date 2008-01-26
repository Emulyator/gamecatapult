#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#ifndef __ee__
#include <gctp/bfstream.hpp>
#endif
#include <gctp/bbstream.hpp>
#include <gctp/zfilter.hpp>
#include <gctp/cryptfilter.hpp>
#include <gctp/archive.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
//#undef GCTP_USE_ZLIB

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
		"圧縮ストリームテストあああああああああああああああああああああああああああああああああああああああああああああああ";
	// 圧縮
    {
		std::filebuf ofb;
		ofb.open("bzfiltertest", std::ios::out|std::ios::binary);
		CPPUNIT_ASSERT(ofb.is_open());
		basic_zfilter<16, char> ozf(&ofb);
		obstream ofs(&ozf);

		ofs << test;
    }

	{
		File f(_T("bzfiltertest"));
		CPPUNIT_ASSERT((unsigned)f.length()<(strlen(test)+12));
		std::string str;
		f >> str;
		CPPUNIT_ASSERT(str!=test);
	}

    // 伸長
    {
		std::filebuf ifb;
		ifb.open("bzfiltertest", std::ios::in|std::ios::binary);
		CPPUNIT_ASSERT(ifb.is_open());
        basic_zfilter<16, char> izf(&ifb);
		ibstream ifs(&izf);

		std::string str;
		ifs >> str;
		CPPUNIT_ASSERT(str == test);
    }
  }
#endif // GCTP_USE_ZLIB
  void test_cryptfilter() {
    const char *test = 
		"暗号ストリームテストあああああああああああああああああああああああああああああああああああああああああああああああ";
	// 暗号
    {
		std::filebuf ofb;
		ofb.open("cryptfiltertest", std::ios::out|std::ios::binary);
		CPPUNIT_ASSERT(ofb.is_open());
		Crypt cpypt("Who is John Galt?");
		basic_cryptfilter<16, char> ocf(cpypt, &ofb);
		obstream ofs(&ocf);

		ofs << test;
    }

	{
		File f(_T("cryptfiltertest"));
		CPPUNIT_ASSERT((unsigned)f.length()<(strlen(test)+12));
		std::string str;
		f >> str;
		CPPUNIT_ASSERT(str!=test);
	}

    // 復号
    {
		std::filebuf ifb;
		ifb.open("cryptfiltertest", std::ios::in|std::ios::binary);
		CPPUNIT_ASSERT(ifb.is_open());
		Crypt cpypt("Who is John Galt?");
        basic_cryptfilter<16, char> icf(cpypt, &ifb);
		ibstream ifs(&icf);

		std::string str;
		ifs >> str;
		CPPUNIT_ASSERT(str == test);
    }
  }
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
  CPPUNIT_TEST(test_cryptfilter);
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
