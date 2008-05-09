#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <gctp/vector.hpp>
#include <gctp/math/vector2d.hpp>
#include <gctp/tree.hpp>
#include <gctp/dbgout.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

//CUPPA:namespace=+
namespace math {
//CUPPA:namespace=-

using namespace gctp;

class VectorTest : public CppUnit::TestFixture {
private:

  // your stuff...

public:

  virtual void setUp() {
  }
  virtual void tearDown() {
  }

//CUPPA:decl=+
  void testSimpleCase() {
    //CPPUNIT_FAIL("no implementation");
	Vector vec0 = {2, 0, 0};
    vec0.normalize();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(vec0.x, 1.0f, 0.0001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(vec0.y, 0.0f, 0.0001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(vec0.z, 0.0f, 0.0001f);
    
    vec0 = VectorC(3, 4, 5);
	Vector vec1 = {1, 2, 3};
    Vector vec2;
    vec2 = vec0 + vec1;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(vec2.x, 4.0f, 0.0001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(vec2.y, 6.0f, 0.0001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(vec2.z, 8.0f, 0.0001f);
    
    CPPUNIT_ASSERT_DOUBLES_EQUAL(vec2.length(), 10.770329614269008062501420983081f, 0.00001f);
    
    Vector vec3 = vec2 * 3.0f;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(vec3.x, 4.0f*3.0f, 0.0001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(vec3.y, 6.0f*3.0f, 0.0001f);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(vec3.z, 8.0f*3.0f, 0.0001f);
  }

  void test2d() {
    //CPPUNIT_FAIL("no implementation");
	gctp::math::Vector2d<float> vec0 = {2, 0};
    gctp::math::Vector2dC<float> vec1(0, 3);
    gctp::math::Vector2d<float> vec2 = vec0 + vec1;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(vec2.x, 2, FLT_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(vec2.y, 3, FLT_EPSILON);
	gctp::math::Vector2d<float> vec3 = {0, 0};
	gctp::math::Vector2d<float> vec4 = gctp::math::Vector2d<float>().setHermite(vec0, vec1, vec2, vec3, 0.6f);
  }
  
  class Foo : public Vector {
  public:
	  Foo(const Vector &src) : Vector(src) {}
	  ~Foo() { PRNN(*this<<" destroyed"); }
  };

  void testTree() {
    //CPPUNIT_FAIL("no implementation");
	  PRNN("Tree");
	  Tree<Foo> tree(VectorC(0,0,0));
	  PRNN("???");
	  Tree<Foo>::SiblingItr i = tree.root();
	  i->push(VectorC(0,0,1));
	  PRNN("???");
	  Tree<Foo>::SiblingItr ii = i->push(VectorC(0,1,0));
	  PRNN("???");
	  i->push(VectorC(1,0,0));
	  PRNN("???");
	  ii->push(VectorC(0,0,2));
	  PRNN("???");
	  ii->push(VectorC(0,2,0));
	  PRNN("???");
	  ii->push(VectorC(2,0,0));
	  PRNN("???");
	  //PRNN(tree);
	  tree.print(dbgout);
  }
//CUPPA:decl=-
  CPPUNIT_TEST_SUITE(VectorTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testSimpleCase);
  CPPUNIT_TEST(test2d);
  CPPUNIT_TEST(testTree);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace math {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(VectorTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(VectorTest,"math");

}
