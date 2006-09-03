#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

#include <gctp/fixed.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/def.hpp>

//CUPPA:namespace=+
namespace math {
//CUPPA:namespace=-

using namespace gctp;

	typedef Fixed<12> Real;

class FixedTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(FixedTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testFixed);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
private:

  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
	void testFixed()
	{
		Real f1( 6.0 );
		Real f2( 2.5 );
		
		CPPUNIT_ASSERT( -f1 == Real(0, -(6<<12)) );
		CPPUNIT_ASSERT( (f1 + f2) == Real(8.5f) );
		CPPUNIT_ASSERT( (f1 - f2) == Real(3.5f) );
		CPPUNIT_ASSERT( (f1 * f2) == Real(15.0f) );
		CPPUNIT_ASSERT( (f1 / f2) == Real(2.4f) );
		CPPUNIT_ASSERT( (f1 % f2) == Real(1.0f) );
		
		CPPUNIT_ASSERT( (f1 + Real(3)) == Real(9) );
		CPPUNIT_ASSERT( (f1 - Real(3)) == Real(3) );
		CPPUNIT_ASSERT( (f1 * Real(3)) == Real(18) );
		CPPUNIT_ASSERT( (f1 / Real(3)) == Real(2) );
		CPPUNIT_ASSERT( (f1 % Real(4)) == Real(2) );
	}
//CUPPA:decl=-
};

}

namespace math {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(FixedTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FixedTest,"math");

}
