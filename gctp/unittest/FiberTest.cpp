#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <gctp/fiber.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <gctp/dbgout.hpp>

using namespace std;

//CUPPA:namespace=+
namespace core {
//CUPPA:namespace=-
	
using namespace gctp;

	static int count0;
	static int count1;
	void fiber_test0(void * /* data */)
	{
		count0++;
		dbgout << "enter func" << endl;
		int i;
		i = 1;
		Fiber::yield();
		count0++;
		CPPUNIT_ASSERT(i==1);
		i = 100;
		std::string str = "string class";
		Fiber::yield();
		count0++;
		CPPUNIT_ASSERT(i==100);
		CPPUNIT_ASSERT(str=="string class");
		dbgout << "end of func" << endl;
	}

	void fiber_test1(void * /* data */)
	{
		count1++;
		dbgout << "enter func" << endl;
		int i;
		i = 1;
		Fiber::yield();
		count1++;
		CPPUNIT_ASSERT(i==1);
		i = 100;
		std::string str = "string class";
		Fiber::yield();
		count1++;
		CPPUNIT_ASSERT(i==100);
		CPPUNIT_ASSERT(str=="string class");
		dbgout << "end of func" << endl;
	}

class FiberTest : public CppUnit::TestFixture {
private:

  // your stuff...

public:

  virtual void setUp()
  {
	count0 = 0;
	count1 = 0;
  }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
  void testRun() {
	CPPUNIT_ASSERT(count0==0);
	Fiber fiber(&fiber_test0);
	fiber.run();
	CPPUNIT_ASSERT(count0==1);
	fiber.run();
	CPPUNIT_ASSERT(count0==2);
	fiber.join();
	CPPUNIT_ASSERT(count0==3);
  }
  void testMultiRun() {
	CPPUNIT_ASSERT(count0==0);
	CPPUNIT_ASSERT(count1==0);
	Fiber fiber0(&fiber_test0);
	Fiber fiber1(&fiber_test1);
	fiber0.run();
	fiber1.run();
	CPPUNIT_ASSERT(count0==1);
	CPPUNIT_ASSERT(count1==1);
	fiber0.run();
	fiber1.run();
	CPPUNIT_ASSERT(count0==2);
	CPPUNIT_ASSERT(count1==2);
	fiber0.join();
	fiber1.join();
	CPPUNIT_ASSERT(count0==3);
	CPPUNIT_ASSERT(count1==3);
  }
//CUPPA:decl=-
  CPPUNIT_TEST_SUITE(FiberTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testRun);
  CPPUNIT_TEST(testMultiRun);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace core {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(FiberTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(FiberTest,"core");

}
