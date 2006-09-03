#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <gctp/matrix.hpp>
#include <gctp/vector.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

//CUPPA:namespace=+
namespace math {
//CUPPA:namespace=-

using namespace gctp;

class MatrixTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(MatrixTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testCalc);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
private:

  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
  void testCalc() {
    //CPPUNIT_FAIL("no implementation");
    MatrixC m(true);
    CPPUNIT_ASSERT(m);
    m(2, 3) = 5.0f;
    CPPUNIT_ASSERT(!m);
#ifdef RWPLCORE_H
    RwMatrix *mm = RwMatrixCreate();
    RwMatrixSetIdentity(mm);
    //CPPUNIT_ASSERT(m);
#else
    m.identify();
    CPPUNIT_ASSERT(m);
#endif
    Matrix m1 = MatrixC(true);
    Matrix m2;
    m2 = m + m1;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m2(0,0), 2.0, FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m2(1,1), 2.0, FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m2(2,2), 2.0, FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m2(3,3), 2.0, FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m2(3,2), 0.0, FLT_EPSILON);
    m1.rot(VectorC(1, 1, 0).normalize(), g_pi/4);
  }
//CUPPA:decl=-
#undef __PI
};

}

namespace math {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(MatrixTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(MatrixTest,"math");

}
