#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <gctp/dbgout.hpp>
#include <gctp/context.hpp>
#include <gctp/class.hpp>
#include <gctp/turi.hpp>
#include <gctp/fileserver.hpp>
#include <gctp/file.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <gctp/db.hpp>

using namespace std;

//CUPPA:namespace=+
namespace core {
//CUPPA:namespace=-

	using namespace gctp;

	class ContextTestFoo : public Object {
	public:
		typedef Handle<ContextTestFoo> Hndl;
		typedef Pointer<ContextTestFoo> Ptr;
		ContextTestFoo() : data(123) {}
		bool setUp(BufferPtr)
		{
			return true;
		}
		int data;
	GCTP_DECLARE_CLASS
	protected:
	};
	GCTP_IMPLEMENT_CLASS(ContextTestFoo, Object);

	GCTP_REGISTER_REALIZER(ext, ContextTestFoo);

class ContextTest : public CppUnit::TestFixture {
private:

  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
  void testURI() {
	  {
		  TURI uri(_T("host0:/test/leaf.ext"));
		  CPPUNIT_ASSERT(uri.scheme()==_T("host0"));
		  CPPUNIT_ASSERT(uri.path()==_T("host0:/test"));
		  CPPUNIT_ASSERT(uri.directory()==_T("/test"));
		  CPPUNIT_ASSERT(uri.leaf()==_T("leaf.ext"));
		  CPPUNIT_ASSERT(uri.basename()==_T("leaf"));
		  CPPUNIT_ASSERT(uri.extension()==_T("ext"));
	  }
	  {
		  TURI uri(_T("test/leaf"));
		  //dbgout << uri.path() << endl;
		  CPPUNIT_ASSERT(uri.scheme()==_T(""));
		  CPPUNIT_ASSERT(uri.path()==_T("test"));
		  CPPUNIT_ASSERT(uri.leaf()==_T("leaf"));
		  CPPUNIT_ASSERT(uri.basename()==_T("leaf"));
		  CPPUNIT_ASSERT(uri.extension()==_T(""));
	  }
  }
  void testRealizer() {
	  TURI uri(_T("test/leaf.ext"));
	  RealizeMethod f = Extension::get(uri.extension().c_str());
	  CPPUNIT_ASSERT(f);
	  ContextTestFoo::Ptr p = f(BufferPtr());
	  CPPUNIT_ASSERT(p);
  }
  void testContext() {
	  {
		  File f(_T("context.ext"), File::WRITE);
		  f << "wwwwwwwwwwww";
	  }
	  fileserver().mount(_T("."));
	  gctp::core::Context root;
	  {
		  gctp::core::Context mycontext;
		  mycontext.load(_T("context.ext"));
		  ContextTestFoo::Hndl h = context()[_T("context.ext")];
		  CPPUNIT_ASSERT(h);
	  }
	  ContextTestFoo::Hndl h = context()[_T("context.ext")];
	  CPPUNIT_ASSERT(!h);
  }
//CUPPA:decl=-
  CPPUNIT_TEST_SUITE(ContextTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testURI);
  CPPUNIT_TEST(testRealizer);
  CPPUNIT_TEST(testContext);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace core {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(ContextTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ContextTest,"core");

}
