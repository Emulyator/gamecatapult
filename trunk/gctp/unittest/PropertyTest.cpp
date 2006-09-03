#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <functional>
#include <algorithm>
#include <gctp/object.hpp>
#include <gctp/class.hpp>
#include <gctp/property.hpp>
#include <gctp/dbgout.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

/*template <typename AddressMap>
void foo(AddressMap address)
{
  typedef typename boost::property_traits<AddressMap>::value_type value_type;
  typedef typename boost::property_traits<AddressMap>::key_type key_type;

  value_type old_address, new_address;
  key_type fred = "Fred";
  old_address = boost::get(address, fred);
  new_address = "384 Fitzpatrick Street";
  boost::put(address, fred, new_address);

  key_type joe = "Joe";
  value_type& joes_address = address[joe];
  joes_address = "325 Cushing Avenue";
}*/

#ifdef GCTP_USE_PROPERTY

//CUPPA:namespace=+
namespace core {
//CUPPA:namespace=-

using namespace gctp;

namespace {
	class PFoo : public Object {
	public:
		PFoo() {
#ifdef GCTP_USE_DYNAMIC_PROPERTY
			setDynamicProperty(&pmap);
#endif
		}
		int i;
		float f;
		std::string str;

		bool testMethod()
		{
			i = 250;
			return true;
		}
#ifdef GCTP_USE_DYNAMIC_PROPERTY
		DynamicPropertyMap pmap;
#endif
	GCTP_DECLARE_CLASS
	GCTP_DECLARE_PROPERTY
	};
	
	GCTP_IMPLEMENT_CLASS(PFoo, Object);
	GCTP_IMPLEMENT_PROPERTY_BEGIN(PFoo);
		GCTP_PROPERTY(i);
		GCTP_PROPERTY(f);
		GCTP_PROPERTY(str);
		GCTP_METHOD(PFoo, testMethod);
	GCTP_IMPLEMENT_PROPERTY_END;
	
	class PBar : public PFoo {
	public:
		int j;
		std::string barstr;

	GCTP_DECLARE_CLASS
	GCTP_DECLARE_PROPERTY
	};
	
	GCTP_IMPLEMENT_CLASS(PBar, PFoo);
	GCTP_IMPLEMENT_PROPERTY_BEGIN(PBar);
		GCTP_PROPERTY(j);
		GCTP_PROPERTY(barstr);
	GCTP_IMPLEMENT_PROPERTY_END;
}

class PropertyTest : public CppUnit::TestFixture {
private:

  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
/*  void testSimpleCase() {
	  std::map<std::string, std::string> name2address;
	  boost::associative_property_map< std::map<std::string, std::string> > address_map(name2address);

	  name2address.insert(std::make_pair(std::string("Fred"), std::string("710 West 13th Street")));
	  name2address.insert(std::make_pair(std::string("Joe"), std::string("710 West 13th Street")));

	  foo(address_map);

	  for(std::map<std::string, std::string>::iterator i = name2address.begin(); i != name2address.end(); ++i)
		  dbgout << i->first << ": " << i->second << "\n";
  }*/
  void testSimpleCase() {
    //CPPUNIT_FAIL("no implementation");
	  int k = 20;
	  Property bar(typeid(k), &k);
	  CPPUNIT_ASSERT(property_cast<int>(bar) == 20);
	  bar = 10;
	  CPPUNIT_ASSERT(property_cast<int>(bar) == 10);
	  CPPUNIT_ASSERT(k == 10);

	  PFoo w;
	  Object *obj = &w;
	  obj->property("i") = 3;
	  CPPUNIT_ASSERT(property_cast<int>(obj->property("i")) == 3);
	  CPPUNIT_ASSERT(w.i == 3);
	  obj->property("f") = 12.3456f;
	  CPPUNIT_ASSERT(property_cast<float>(obj->property("f")) == 12.3456f);
	  CPPUNIT_ASSERT(w.f == 12.3456f);
	  obj->property("str") = std::string("abcdefg");
	  CPPUNIT_ASSERT(property_cast<std::string>(obj->property("str")) == "abcdefg");
	  CPPUNIT_ASSERT(w.str == "abcdefg");
	  CPPUNIT_ASSERT(obj->method("testMethod"));
//	  CPPUNIT_ASSERT(obj->method<bool (int, int &)>("testMethod")(0, i));
	  CPPUNIT_ASSERT(w.i == 250);
#ifdef GCTP_USE_DYNAMIC_PROPERTY
	  w.pmap.register_("k", bar);
	  CPPUNIT_ASSERT(property_cast<int>(obj->property("k")) == 10);
	  obj->property("k") = 3;
	  CPPUNIT_ASSERT(property_cast<int>(obj->property("k")) == 3);
	  CPPUNIT_ASSERT(k == 3);
#endif
  }
  void testDerived() {
    //CPPUNIT_FAIL("no implementation");
	  PBar w;
	  Object *obj = &w;
	  obj->property("i") = 3;
	  CPPUNIT_ASSERT(property_cast<int>(obj->property("i")) == 3);
	  CPPUNIT_ASSERT(w.i == 3);
	  obj->property("f") = 12.3456f;
	  CPPUNIT_ASSERT(property_cast<float>(obj->property("f")) == 12.3456f);
	  CPPUNIT_ASSERT(w.f == 12.3456f);
	  obj->property("str") = std::string("abcdefg");
	  CPPUNIT_ASSERT(property_cast<std::string>(obj->property("str")) == "abcdefg");
	  CPPUNIT_ASSERT(w.str == "abcdefg");
	  obj->property("j") = 8;
	  CPPUNIT_ASSERT(property_cast<int>(obj->property("j")) == 8);
	  CPPUNIT_ASSERT(w.j == 8);
	  obj->property("barstr") = std::string("vwxyz");
	  CPPUNIT_ASSERT(property_cast<std::string>(obj->property("barstr")) == "vwxyz");
	  CPPUNIT_ASSERT(w.barstr == "vwxyz");
  }
//CUPPA:decl=-
  CPPUNIT_TEST_SUITE(PropertyTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testSimpleCase);
  CPPUNIT_TEST(testDerived);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace core {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(PropertyTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(PropertyTest,"core");

}
#endif
