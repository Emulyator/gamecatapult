#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します
#include <windows.h>
#endif

#include <algorithm>
#include <string>
#include <gctp/allocmap.hpp>
#include <gctp/oct.hpp>
#include <gctp/crypt.hpp>
#include <gctp/bitfieldarray.hpp>
#define GCTP_MEMORY_CHECK
#define GCTP_MEMORY_CHECK_CPPUNIT
#include <gctp/memory.hpp>
#include <gctp/dbgout.hpp>
#include <time.h>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

//CUPPA:namespace=+
namespace util {
//CUPPA:namespace=-

using namespace gctp;

class UtilsTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(UtilsTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testQuadFunc);
  CPPUNIT_TEST(testOctFunc);
  CPPUNIT_TEST(testAllocMap);
  CPPUNIT_TEST(testMemory);
  CPPUNIT_TEST(testMemoryAllocator);
  CPPUNIT_TEST(testCrypt);
  CPPUNIT_TEST(testBitFieldArray);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
private:

  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
  void testQuadFunc() {
	  CPPUNIT_ASSERT(quadsq(0)==1);
	  CPPUNIT_ASSERT(quadsq(1)==4);
	  CPPUNIT_ASSERT(quadsq(2)==16);
	  CPPUNIT_ASSERT(quadsq(3)==64);
	  CPPUNIT_ASSERT(quadprog(0)==1);
	  CPPUNIT_ASSERT(quadprog(1)==5);
	  CPPUNIT_ASSERT(quadprog(2)==21);
	  CPPUNIT_ASSERT(quadprog(3)==85);
	  CPPUNIT_ASSERT(quadpos(0)==Point2C(0, 0));
	  CPPUNIT_ASSERT(quadpos(6)==Point2C(2, 1));
	  CPPUNIT_ASSERT(quadpos(33)==Point2C(1, 4));
	  CPPUNIT_ASSERT(quadpos(57)==Point2C(5, 6));
	  CPPUNIT_ASSERT(quadpos(67)==Point2C(9, 1));
	  CPPUNIT_ASSERT(quadidx(Point2C(0, 0))==0);
	  CPPUNIT_ASSERT(quadidx(Point2C(2, 1))==6);
	  CPPUNIT_ASSERT(quadidx(Point2C(1, 4))==33);
	  CPPUNIT_ASSERT(quadidx(Point2C(5, 6))==57);
	  CPPUNIT_ASSERT(quadidx(Point2C(9, 1))==67);
  }
  void testOctFunc() {
	  CPPUNIT_ASSERT(octcube(0)==1);
	  CPPUNIT_ASSERT(octcube(1)==8);
	  CPPUNIT_ASSERT(octcube(2)==64);
	  CPPUNIT_ASSERT(octcube(3)==512);
	  CPPUNIT_ASSERT(octprog(0)==1);
	  CPPUNIT_ASSERT(octprog(1)==9);
	  CPPUNIT_ASSERT(octprog(2)==73);
	  CPPUNIT_ASSERT(octprog(3)==585);
	  for(int i = 0; i < 256; i++) {
		  Point3 pos = octpos(i);
		  PRNN(i << ":" << pos.x << "," << pos.y << "," << pos.z);
	  }
	  CPPUNIT_ASSERT(octpos(0)==Point3C(0, 0, 0));
/*	  CPPUNIT_ASSERT(octpos(6)==Point3(2, 1));
	  CPPUNIT_ASSERT(octpos(33)==Point3(1, 4));
	  CPPUNIT_ASSERT(octpos(57)==Point3(5, 6));
	  CPPUNIT_ASSERT(octpos(67)==Point3(9, 1));
	  CPPUNIT_ASSERT(octidx(Point3(0, 0))==0);
	  CPPUNIT_ASSERT(octidx(Point3(2, 1))==6);
	  CPPUNIT_ASSERT(octidx(Point3(1, 4))==33);
	  CPPUNIT_ASSERT(octidx(Point3(5, 6))==57);
	  CPPUNIT_ASSERT(octidx(Point3(9, 1))==67);*/
  }
  void testAllocMap() {
	  AllocBitMap<3> am;
	  CPPUNIT_ASSERT(am.isFree(0));
	  CPPUNIT_ASSERT(am.isFree(6));
	  CPPUNIT_ASSERT(am.isFree(33));
	  unsigned int idx[32];
	  CPPUNIT_ASSERT(am.alloc(0, idx[0]));
	  CPPUNIT_ASSERT(idx[0] == 0);
	  CPPUNIT_ASSERT(!am.isFree(0));
	  CPPUNIT_ASSERT(!am.isFree(6));
	  CPPUNIT_ASSERT(!am.isFree(33));
	  CPPUNIT_ASSERT(!am.alloc(1, idx[0]));
	  am.reset();
	  CPPUNIT_ASSERT(am.alloc(1, idx[0]));
	  CPPUNIT_ASSERT(idx[0] == 1);
	  CPPUNIT_ASSERT(am.alloc(1, idx[1]));
	  CPPUNIT_ASSERT(idx[1] == 2);
	  CPPUNIT_ASSERT(am.alloc(1, idx[2]));
	  CPPUNIT_ASSERT(idx[2] == 3);
	  CPPUNIT_ASSERT(am.alloc(1, idx[3]));
	  CPPUNIT_ASSERT(idx[3] == 4);
	  CPPUNIT_ASSERT(!am.alloc(2, idx[4]));
	  am.free(idx[3]);
	  CPPUNIT_ASSERT(am.isFree(idx[3]));
	  CPPUNIT_ASSERT(am.alloc(2, idx[3]));
	  CPPUNIT_ASSERT(idx[3] == 17);
	  CPPUNIT_ASSERT(am.alloc(3, idx[4]));
	  CPPUNIT_ASSERT(idx[4] == 21+52);
	  for(int i = 0; i < 5; i++) am.free(idx[i]);
	  CPPUNIT_ASSERT(am.isFree(0));

	  AllocBitMap<3>::Block block;
	  block.set(0);
	  CPPUNIT_ASSERT(block.w == 8);
	  CPPUNIT_ASSERT(block.pos.x == 0);
	  CPPUNIT_ASSERT(block.pos.y == 0);
	  CPPUNIT_ASSERT(block.index() == 0);
	  block.set(2);
	  CPPUNIT_ASSERT(block.w == 4);
	  CPPUNIT_ASSERT(block.pos.x == 4);
	  CPPUNIT_ASSERT(block.pos.y == 0);
	  CPPUNIT_ASSERT(block.index() == 2);
	  block.set(5+13);
	  CPPUNIT_ASSERT(block.w == 2);
	  CPPUNIT_ASSERT(block.pos.x == 6);
	  CPPUNIT_ASSERT(block.pos.y == 4);
	  CPPUNIT_ASSERT(block.index() == 5+13);
	  block.set(21+35);
	  CPPUNIT_ASSERT(block.w == 1);
	  CPPUNIT_ASSERT(block.pos.x == 1);
	  CPPUNIT_ASSERT(block.pos.y == 5);
	  CPPUNIT_ASSERT(block.index() == 21+35);
  }
  void testMemory() {
	  char *bytes = new char[1024*1024*16];
	  Memory<> memory(bytes, 1024*1024*16);
	  Memory<>::Info info;

	  void *p1 = memory.malloc(128);
	  CPPUNIT_ASSERT(p1);
	  CPPUNIT_ASSERT(memory.msize(p1)==128);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 2);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 1);

	  void *p2 = memory.malloc(240);
	  CPPUNIT_ASSERT(p2);
	  CPPUNIT_ASSERT(memory.msize(p2)==256);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 3);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 1);

	  memory.free(p1);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 2);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 2);

	  void *p3 = memory.malloc(256);
	  CPPUNIT_ASSERT(p3);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 3);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 2);

	  memory.free(p2);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 2);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 2);

	  memory.free(p3);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 1);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 1);

	  void *p4 = memory.malloc(32);
	  CPPUNIT_ASSERT(p4);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 2);
	  CPPUNIT_ASSERT(info.small_alloc_count == 1);
	  CPPUNIT_ASSERT(info.free_block_num == 1);

	  memory.free(p4);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 1);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 1);

	  void *p5 = memory.malloc(1024);
	  CPPUNIT_ASSERT(p5);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 2);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 1);

	  memory.free(p5);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 1);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 1);

	  void *p6 = memory.malloc(0);
	  CPPUNIT_ASSERT(p6);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 2);
	  CPPUNIT_ASSERT(info.small_alloc_count == 1);
	  CPPUNIT_ASSERT(info.free_block_num == 1);

	  void *p7 = memory.realloc(p6, 128);
	  CPPUNIT_ASSERT(p7);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 2);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 1);

	  memory.free(p7);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 1);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 1);

	  p1 = memory.malloc(234);
	  p2 = memory.malloc(456);
	  p3 = memory.malloc(789);
	  p4 = memory.malloc(1234);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 5);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 1);

	  memory.free(p1);
	  memory.free(p3);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 3);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 3);

	  memory.free(p2);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 2);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 2);

	  memory.free(p4);
	  info = memory.info();
	  CPPUNIT_ASSERT(info.alloc_count == 1);
	  CPPUNIT_ASSERT(info.small_alloc_count == 0);
	  CPPUNIT_ASSERT(info.free_block_num == 1);

	  delete[] bytes;
  }

  struct _Alloc {
	void *allocate(size_t s) { return malloc(s); }
	void deallocate(void *p, int i) { free(p); }
  };
  class __Foo {
  public:
	  static _Alloc allocator() { return _Alloc(); }
	  __Foo() { PRNN("__Foo"); }
	  ~__Foo() { PRNN("~__Foo"); }
	  int dummy_;
		static void *operator new( size_t size, void *p ) {
			PRNN("__Foo::new");
			return p;
		}
		static void *operator new( size_t size ) {
			PRNN("__Foo::new");
			return allocator().allocate( size );
		}
		static void *operator new( size_t size, _Alloc& allocator ) {
			PRNN("__Foo::new alloc");
			return allocator.allocate( 1 );
		}
		static void operator delete( void *p, _Alloc& allocator ) {
			PRNN("__Foo::delete throw");
			allocator.deallocate( p, 1 );
		}
		static void operator delete( void *p, void *_p ) {
			PRNN("__Foo::delete throw");
			free( p );
		}
		static void operator delete( void *p ) {
			PRNN("__Foo::delete");
			allocator().deallocate( p, 1 ); 
		}
  };
  class __Bar : public __Foo {
  };

  void testMemoryAllocator() {
//	  char *bytes = new char[1024*1024*16];
//	  Memory<> memory(bytes, 1024*1024*16);
//	  Memory<>::Info info;
//
//	  std::list<int, Memory> _list;
//
//	  delete[] bytes;
	  void *_p = malloc(sizeof(__Bar));
	  __Bar *p = new(_p) __Bar;
	  delete p;

	  __Foo *pp = new __Foo;
	  delete pp;
  }
  void testCrypt() {
	    Crypt crypt;

		Crypt::DWord Test_Vect;
		char *Passwd[2] = {"abcdefghijklmnopqrstuvwxyz","Who is John Galt?"};
		unsigned int Clr0[2] = {0x424c4f57,0xfedcba98};
		unsigned int Clr1[2] = {0x46495348,0x76543210};
		unsigned int Crypt0[2] = {0x324ed0fe,0xcc91732b};
		unsigned int Crypt1[2] = {0xf413a203,0x8022f684};
		
		Test_Vect.word0.word = Clr0[0];
		Test_Vect.word1.word = Clr1[0];
		crypt.setKey(Passwd[0]);
		crypt.encode(&Test_Vect, 8);
		CPPUNIT_ASSERT(Test_Vect.word0.word == Crypt0[0] && Test_Vect.word1.word == Crypt1[0]);
		crypt.decode(&Test_Vect, 8);
		CPPUNIT_ASSERT(Test_Vect.word0.word == Clr0[0] && Test_Vect.word1.word == Clr1[0]);

		Test_Vect.word0.word = Clr0[1];
		Test_Vect.word1.word = Clr1[1];
		crypt.setKey(Passwd[1]);
		crypt.encode(&Test_Vect, 8);
		CPPUNIT_ASSERT(Test_Vect.word0.word == Crypt0[1] && Test_Vect.word1.word == Crypt1[1]);
		crypt.decode(&Test_Vect, 8);
		CPPUNIT_ASSERT(Test_Vect.word0.word == Clr0[1] && Test_Vect.word1.word == Clr1[1]);

		if(0) { // BenchMark
			const int BUFF_SIZE = 1048576; // 1MB
			const int NUM_TRIALS = 100;
			char *buff;
			double speed;
			time_t begin,end;

			buff = new char[BUFF_SIZE];
			CPPUNIT_ASSERT(buff);
			
			srand(0);
			for(int i=0; i<BUFF_SIZE; i++) buff[i] = rand()%256;
			crypt.setKey("ianchan");

			begin = time(NULL);
			for(int i=0; i<NUM_TRIALS; i++) crypt.encode(buff, BUFF_SIZE);
			end = time(NULL);
			speed = double(NUM_TRIALS)/(end-begin);
			dbgout << "gctp::Crypt.encode : The throughput is " << speed << "MB/s" << std::endl;

			begin = time(NULL);
			for(int i=0; i<NUM_TRIALS; i++) crypt.decode(buff, BUFF_SIZE);
			end = time(NULL);
			speed = double(NUM_TRIALS)/(end-begin);
			dbgout << "gctp::Crypt.decode : The throughput is " << speed << "MB/s" << std::endl;
			
			delete []buff;
		}
  }
	void testBitFieldArray()
	{
		BitFieldArray<3,6> bfa1;
		memset(bfa1.bytes, 0, sizeof(bfa1.bytes));
		bfa1[2] = 3;
		bfa1[4] = 9;
		bfa1[5] = 1;
		CPPUNIT_ASSERT(bfa1[0] == 0);
		CPPUNIT_ASSERT(bfa1[1] == 0);
		CPPUNIT_ASSERT(bfa1[2] == 3);
		CPPUNIT_ASSERT(bfa1[3] == 0);
		CPPUNIT_ASSERT(bfa1[4] == 1);
		CPPUNIT_ASSERT(bfa1[5] == 1);
		bfa1[2] = 1;
		CPPUNIT_ASSERT(bfa1[2] == 1);
		bfa1[2] = 2;
		CPPUNIT_ASSERT(bfa1[2] == 2);
		bfa1[2] = 3;
		CPPUNIT_ASSERT(bfa1[2] == 3);
		bfa1[2] = 0;
		CPPUNIT_ASSERT(bfa1[2] == 0);
		bfa1[5] = 1;
		CPPUNIT_ASSERT(bfa1[5] == 1);
		bfa1[5] = 2;
		CPPUNIT_ASSERT(bfa1[5] == 2);
		bfa1[5] = 3;
		CPPUNIT_ASSERT(bfa1[5] == 3);
		bfa1[5] = 0;
		CPPUNIT_ASSERT(bfa1[5] == 0);
	}
//CUPPA:decl=-
};

}

namespace util {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(UtilsTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(UtilsTest,"util");

}
