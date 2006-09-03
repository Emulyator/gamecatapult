#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <gctp/object.hpp>
#include <gctp/class.hpp>
#include <gctp/serializer.hpp>
#include <gctp/bfstream.hpp>
#include <gctp/dbgout.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

//CUPPA:namespace=+
namespace core {
//CUPPA:namespace=-

using namespace gctp;

namespace {

	bool override_allocator_ok = false;

	class Foo : public Object
	{
	public:
		int data;
	GCTP_DECLARE_CLASS
	protected:
		Foo() : data(123) {}
		static void *allocate(std::size_t n) { override_allocator_ok = true; return ::operator new(n); }
		static AbstractDeleter *deleter() { return 0; }
	};
	GCTP_IMPLEMENT_CLASS_NS(my, Foo, Object);

	class Bar : public Foo
	{
	protected:
		Bar() : Foo(), sss(0) {}
	public:
		int sss;
	GCTP_DECLARE_CLASS
	};
	GCTP_IMPLEMENT_CLASS(Bar, Foo);

	class HBar;
	class HFoo : public Object
	{
	public:
		HFoo() : data(0) {}
		int data;
		float real;
		std::string str;
		Handle<HBar> hndl;
		Pointer<Foo> ptr;
	GCTP_DECLARE_CLASS
	GCTP_DECLARE_SERIALIZE
	};
	GCTP_IMPLEMENT_CLASS(HFoo, Object);
	GCTP_IMPREMENT_SERIALIZE_BEGIN(0, HFoo, Object)
		GCTP_SERIALIZE(<< data << real << str >> hndl >> ptr)
	GCTP_IMPREMENT_SERIALIZE_END;
	
	class HBar : public Object
	{
	public:
		HBar() : data(0) {}
		int data;
		float real;
		std::string str;
		Handle<HFoo> hndl;
		Pointer<Foo> ptr;
	GCTP_DECLARE_CLASS
	GCTP_DECLARE_SERIALIZE
	};
	GCTP_IMPLEMENT_CLASS(HBar, Object);
	GCTP_IMPREMENT_SERIALIZE_BEGIN(0, HBar, Object)
		GCTP_SERIALIZE(<< data << real << str >> hndl >> ptr)
	GCTP_IMPREMENT_SERIALIZE_END;
}

class ObjectTest : public CppUnit::TestFixture {
private:

  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
  void testPointer() {
	Pointer<Bar> w1;
	if(w1) CPPUNIT_FAIL ("w1 must be null pointer");
	Handle<Foo> w2;
	if(w2) CPPUNIT_FAIL ("w2 must be null handle");
  }
  void testCreation() {
	Pointer<Bar> w1 = Factory::create("Bar");
	CPPUNIT_ASSERT (w1);
	CPPUNIT_ASSERT (GCTP_THISTYPEID(*w1) == GCTP_TYPEID(Bar));
	Pointer<Foo> w2 = Factory::create("my.Foo");
	CPPUNIT_ASSERT (override_allocator_ok);
	CPPUNIT_ASSERT (w2);
	CPPUNIT_ASSERT (GCTP_THISTYPEID(*w2) == GCTP_TYPEID(Foo));

    PRNN("sizeof(gctp::Object) = "<<sizeof(Object));
  }
  void testSubstance() {
	Handle<Foo> wh;
	{
		Pointer<Foo> w = Factory::create("Bar");
		CPPUNIT_ASSERT (w);
		CPPUNIT_ASSERT (GCTP_THISTYPEID(*w)==GCTP_TYPEID(Bar));
		wh = w;
		CPPUNIT_ASSERT (wh);
	}
	CPPUNIT_ASSERT (!wh);
	
	Pointer<const Bar> w = Factory::create("Bar");
	CPPUNIT_ASSERT (w);
	Pointer<const Bar> w1 = w;
	w = NULL;
	CPPUNIT_ASSERT (!w);
	CPPUNIT_ASSERT (w1);
  }
  void testSerialize() {
#ifdef __ee__
#define PREFIX "host0:"
#else
#define PREFIX ""
#endif
	{
		Pointer<HFoo> w1 = Factory::create("HFoo");
		CPPUNIT_ASSERT (w1);
		Pointer<HBar> w2 = Factory::create("HBar");
		CPPUNIT_ASSERT (w2);
		Pointer<Foo> w3 = Factory::create("my.Foo");
		CPPUNIT_ASSERT (w3);

		w1->data = 2;
		w1->real = 5;
		w1->str = "foo";
		w1->hndl = w2;
		w1->ptr = w3;
		CPPUNIT_ASSERT (w1->hndl == w2);

		w2->data = -8;
		w2->real = 3.14f;
		w2->str = "bar";
		w2->hndl = w1;
		w2->ptr = w3;
		CPPUNIT_ASSERT (w2->hndl == w1);
		CPPUNIT_ASSERT (w1->ptr && w2->ptr && w1->ptr == w2->ptr);
		w3 = NULL;
		CPPUNIT_ASSERT (!w3);
		obfstream stream(PREFIX"testserialize");
		Serializer serializer(stream);
		serializer >> w1 >> w2;
	}

	Pointer<HFoo> w1;
	Pointer<HBar> w2;
	{
		ibfstream stream(PREFIX"testserialize");
		CPPUNIT_ASSERT (stream.is_open());
		Serializer serializer(stream);
		serializer >> w1 >> w2;
	}
	CPPUNIT_ASSERT (w1);
	CPPUNIT_ASSERT (w2);

	CPPUNIT_ASSERT (w1->data == 2);
	CPPUNIT_ASSERT (w1->real == 5);
	CPPUNIT_ASSERT (w1->str == "foo");
	CPPUNIT_ASSERT (w1->hndl.lock() == w2);
	CPPUNIT_ASSERT (w1->ptr);
	
	CPPUNIT_ASSERT (w2->data == -8);
	CPPUNIT_ASSERT (w2->real == 3.14f);
	CPPUNIT_ASSERT (w2->str == "bar");
	CPPUNIT_ASSERT (w2->hndl.lock() == w1);
	CPPUNIT_ASSERT (w2->ptr);
	
	CPPUNIT_ASSERT (w1->ptr == w2->ptr);
#undef PREFIX
  }
//CUPPA:decl=-
  CPPUNIT_TEST_SUITE(ObjectTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testPointer);
  CPPUNIT_TEST(testCreation);
  CPPUNIT_TEST(testSubstance);
  CPPUNIT_TEST(testSerialize);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace core {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(ObjectTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ObjectTest,"core");

}
