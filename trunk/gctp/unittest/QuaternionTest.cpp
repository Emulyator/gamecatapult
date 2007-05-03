#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <gctp/quat.hpp>
#include <gctp/matrix.hpp>
#include <gctp/dbgout.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

//CUPPA:namespace=+
namespace math {
//CUPPA:namespace=-

using namespace gctp;

class QuaternionTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(QuaternionTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testCalc);
  CPPUNIT_TEST(testRot);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
private:

  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
  void testCalc() {
#ifdef __ee__
    CPPUNIT_FAIL("no implementation");
#endif
  	Matrix m;
  	m.rot(VectorC(1,1,0).normalize(), 30);
    Quat q1, q2, q3;
    q1.set(m);
	q2.set(VectorC(1,1,0).normalize(), 30);
    //dbgout << q1 << std::endl;
    //dbgout << q2 << std::endl;
//#ifdef DIRECT3D_VERSION
#ifdef _MSC_VER
	q2.conjugate();
#endif
    CPPUNIT_ASSERT_DOUBLES_EQUAL(q1.x, q2.x, 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(q1.y, q2.y, 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(q1.z, q2.z, 0.000001);
    
    q3 = q1 + q2;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(q3.x, q1.x+q2.x, 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(q3.y, q1.y+q2.y, 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(q3.z, q1.z+q2.z, 0.000001);
    q3 = 0.25 * q1;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(q3.x, q1.x*0.25, 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(q3.y, q1.y*0.25, 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(q3.z, q1.z*0.25, 0.000001);
  }
  
  void testRot() {
    Quat q = QuatC(VectorC(1,0,0), g_pi/2.0f);
    Quat q0 = ~q;
	Quat a, b;
	a = q * QuatC(0, 0, 0, 1) * q0;
	b = q0 * QuatC(0, 0, 0, 1) * q;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(a.x, b.x, 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(a.y, -b.y, 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(a.z, b.z, 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(a.w, b.w, 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(a.w, 0.0, 0.000001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(b.w, 0.0, 0.000001);
	CPPUNIT_ASSERT(a.vector().length() >= 0.9f);
	CPPUNIT_ASSERT(b.vector().length() >= 0.9f);
  }
//CUPPA:decl=-
};

}

namespace math {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(QuaternionTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(QuaternionTest,"math");

}
