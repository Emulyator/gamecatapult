#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <gctp/vector.hpp>
#include <gctp/line.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

//CUPPA:namespace=+
namespace math {
//CUPPA:namespace=-

using namespace gctp;

class LineCapTest : public CppUnit::TestFixture {
private:

  // your stuff...

public:

  virtual void setUp() {
  }
  virtual void tearDown() {
  }

//CUPPA:decl=+
  void testDistance() {
//    CPPUNIT_FAIL("no implementation");
	  LineSeg s1(VectorC(1,1,0),VectorC(1,1,1)), s2(VectorC(-1,-2,0),VectorC(-1,-1,0));
	  float t1, t2;
	  float l = distance(s1, s2, &t1, &t2);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(t1, 0.0f, 0.00001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(t2, 1.0f, 0.00001f);
      CPPUNIT_ASSERT_DOUBLES_EQUAL(l, (s2.e-s1.s).length(), 0.00001f);
  }
//CUPPA:decl=-
  CPPUNIT_TEST_SUITE(LineCapTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testDistance);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace math {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(LineCapTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(LineCapTest,"math");

}
