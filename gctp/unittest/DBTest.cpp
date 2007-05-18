#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <gctp/db.hpp>
#include <gctp/class.hpp>
#include <gctp/object.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

//CUPPA:namespace=+
namespace core {
//CUPPA:namespace=-

using namespace gctp;

namespace {
	class FlyWeightA : public Object {
	public:
		FlyWeightA() {}

		uint count_;

	GCTP_DECLARE_CLASS
	};
	GCTP_IMPLEMENT_CLASS(FlyWeightA, Object);
	
	class FlyWeightB : public Object {
	public:
		FlyWeightB() {}

		uint count_;

	GCTP_DECLARE_CLASS
	};
	GCTP_IMPLEMENT_CLASS(FlyWeightB, Object);
}

class DBTest : public CppUnit::TestFixture {
private:

  // your stuff...
	Pointer<FlyWeightA> obja1;
	Pointer<FlyWeightA> obja2;
	Pointer<FlyWeightB> objb1;
	Pointer<FlyWeightB> objb2;
	DB db;

public:

  virtual void setUp()
  {
	obja1 = Factory::create("FlyWeightA");
	obja2 = Factory::create("FlyWeightA");
	objb1 = Factory::create("FlyWeightB");
	objb2 = Factory::create("FlyWeightB");
  }

  virtual void tearDown() {/* initialize */ }

//CUPPA:decl=+
  void testSimpleCase() {
    //CPPUNIT_FAIL("no implementation");
	db.insert(_T("texture/01.bmp"), Handle<FlyWeightA>(obja1));
	db.insert(_T("texture/02.bmp"), Handle<FlyWeightA>(obja2));
	db.insert(_T("texture/03.bmp"), Handle<FlyWeightB>(objb1));
	db.insert(_T("texture/04.bmp"), Handle<FlyWeightB>(objb2));
	CPPUNIT_ASSERT(db.find(_T("texture/01.bmp")));
	CPPUNIT_ASSERT(db.find(_T("texture/02.bmp")));
	CPPUNIT_ASSERT(db.find(_T("texture/03.bmp")));
	CPPUNIT_ASSERT(db.find(_T("texture/04.bmp")));
	
	db.erase(_T("texture/01.bmp"));
	db.erase(_T("texture/02.bmp"));
	db.erase(_T("texture/03.bmp"));
	db.erase(_T("texture/04.bmp"));
	CPPUNIT_ASSERT(!db.find(_T("texture/01.bmp")));
	CPPUNIT_ASSERT(!db.find(_T("texture/02.bmp")));
	CPPUNIT_ASSERT(!db.find(_T("texture/03.bmp")));
	CPPUNIT_ASSERT(!db.find(_T("texture/04.bmp")));
  }
//CUPPA:decl=-
  CPPUNIT_TEST_SUITE(DBTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testSimpleCase);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace core {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(DBTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(DBTest,"core");

}
