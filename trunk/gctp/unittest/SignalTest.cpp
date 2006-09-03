#ifdef _MSC_VER
#	pragma warning(disable:4786 4503 4290)
#endif
//CUPPA:include=+
#include <gctp/signal.hpp>
#include <gctp/shiftable.hpp>
#include <gctp/pointer.hpp>
//CUPPA:include=-
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <iostream>
#include <boost/function.hpp>
#include <gctp/handlelist.hpp>

//CUPPA:namespace=+
namespace core {
//CUPPA:namespace=-

using namespace gctp;

namespace {
	
	int run_count;

	class TaskA : public Object, public InvokeShiftable<TaskA>
	{
		typedef InvokeShiftable<TaskA> Super;
	public:
		TaskA() : Super(&TaskA::firsttask), count_(0) {}

		uint count_;

		bool test()
		{
			// Ç±ÇÍÇÕÇnÇjÇ»ÇÒÇæÇÊÇ»Å`
			// Ç»Ç∫InvokeShiftable::runÇåƒÇ—èoÇπÇ»Ç¢ÇÒÇæÇÎÅH
			run_count++;
			return false;
		}

		bool test2(int &_ret)
		{
			_ret = 123;
			return false;
		}

	private:
		bool firsttask()
		{
			CPPUNIT_ASSERT(count_==0);
			count_++;
			shiftTo(&TaskA::secondtask);
			run_count++;
			return true;
		}

		bool secondtask()
		{
			CPPUNIT_ASSERT(count_==1);
			count_++;
			run_count++;
			return false;
		}
	};
	
	int _priority;

	class PriTask : public Object
	{
	public:
		PriTask(uint16_t pri) : slot(pri) { slot.bind(this); }
		bool test() {
			CPPUNIT_ASSERT(_priority==slot.priority());
			_priority--;
			run_count++;
			return true;
		}
		MemberSlot0<PriTask, &PriTask::test> slot;
	};
	
	class PriTask2 : public Object
	{
	public:
		PriTask2()
		{
			slot1.bind(this);
			slot2.bind(this);
			slot3.bind(this);
		}
		bool test1() {
			CPPUNIT_ASSERT(_priority==slot1.priority());
			_priority--;
			run_count++;
			return true;
		}
		bool test2() {
			CPPUNIT_ASSERT(_priority==slot2.priority());
			_priority--;
			run_count++;
			return true;
		}
		bool test3() {
			CPPUNIT_ASSERT(_priority==slot3.priority());
			_priority--;
			run_count++;
			return true;
		}
		MemberSlot0<PriTask2, &test1> slot1;
		MemberSlot0<PriTask2, &test2> slot2;
		MemberSlot0<PriTask2, &test3> slot3;
	};

	class FooFunc {
	public:
		int test(int i, int &j)
		{
			j += i;
			return i;
		}
	};
	
	class FooFuncWrapper {
	public:
		FooFunc *this_; // Ç‚Ç¡ÇœÇËÉÅÉìÉoä÷êîÇìnÇ∑Ç…ÇÕÇ±ÇÍÇµÇ©Ç»Ç¢
		int operator()(int i, int &j)
		{
			return this_->test(i, j);
		}
	};
	
	class AltInvoker : public Object {
	public:
		class Invokee : public Object {
		public:
			virtual bool operator()(int i, int &j) = 0;
		};
		HandleList<Invokee> invokies_;
		bool operator()(int n, int &m)
		{
			for(HandleList<Invokee>::iterator i = invokies_.begin(); i != invokies_.end();)
			{
				if(*i && (**i)(n, m)) ++i;
				else i = invokies_.erase(i);
			}
			return !invokies_.empty();
		}
	};

	class TestFoo : public Object {
	public:
		bool test(int i, int &j)
		{
			j += i;
			return true;
		}
	};
	
	template<class _T, bool (_T::*Method)(int, int &)>
	class AltSlot : public AltInvoker::Invokee {
	public:
		Handle<_T> this_; // Ç‚Ç¡ÇœÇËÉÅÉìÉoä÷êîÇìnÇ∑Ç…ÇÕÇ±ÇÍÇµÇ©Ç»Ç¢
		virtual bool operator()(int i, int &j)
		{
			if(this_) return (this_.get()->*Method)(i, j);
			return false;
		}
	};

}

class InvokerTest : public CppUnit::TestFixture {
private:

  // your stuff...

public:

  virtual void setUp() { /* initialize */ }
  virtual void tearDown() { /* terminate */ }

//CUPPA:decl=+
  void testSimpleCase() {
	Signal0<false> invoker;
	TaskA task;
	MemberSlot0<TaskA, (bool (TaskA::*)())&TaskA::run> slot(&task);
	invoker.connect(slot);
	invoker();
	CPPUNIT_ASSERT(run_count==1);
	invoker();
	CPPUNIT_ASSERT(run_count==2);
	CPPUNIT_ASSERT(invoker.num()==0);
	MemberSlot0<TaskA, &TaskA::test> slot2(&task);
	invoker.connect(slot2);
	invoker();
	CPPUNIT_ASSERT(run_count==3);
  }
  void testPriority() {
	Signal0<false> invoker;
	PriTask task1(2), task2(1), task3(0);
	invoker.connect(task1.slot);
	invoker.connect(task2.slot);
	invoker.connect(task3.slot);
	run_count = 0;
	_priority = 2;
	invoker();
	CPPUNIT_ASSERT(run_count==3);
  }
  void testComplexCase() {
	Signal0<false> invoker1, invoker2, invoker3;
	PriTask2 task1, task2, task3;
	task1.slot1.setPriority(0); task2.slot1.setPriority(1); task3.slot1.setPriority(2);
	task1.slot2.setPriority(0); task2.slot2.setPriority(1); task3.slot2.setPriority(2);
	task1.slot3.setPriority(0); task2.slot3.setPriority(1); task3.slot3.setPriority(2);
	invoker1 << task1.slot1 << task2.slot1 << task3.slot1;
	invoker2 << task1.slot2 << task2.slot2 << task3.slot2;
	invoker3 << task1.slot3 << task2.slot3 << task3.slot3;
	run_count = 0;
	_priority = 2;
	invoker1();
	CPPUNIT_ASSERT(run_count==3);
	run_count = 0;
	_priority = 2;
	invoker2();
	CPPUNIT_ASSERT(run_count==3);
	run_count = 0;
	_priority = 2;
	invoker3();
	CPPUNIT_ASSERT(run_count==3);
  }
  
  void testFunction() {
	  boost::function<int (int i, int &j)> f;
	  FooFunc foo;
	  FooFuncWrapper foow;
	  foow.this_ = &foo;
	  f = foow;
	  int j = 6;
	  CPPUNIT_ASSERT(f(5, j) == 5);
	  CPPUNIT_ASSERT(j == 11);

	  AltInvoker invoker;
	  TestFoo foo2;
	  AltSlot<TestFoo, &TestFoo::test> foo2w;
	  foo2w.this_ = &foo2;
	  invoker.invokies_.push_back(Handle< AltSlot<TestFoo, &TestFoo::test> >(&foo2w));
	  invoker(9, j);
	  CPPUNIT_ASSERT(j == 20);
  }

  void testSignal() {
	  TaskA foo;
	  MemberSlot1<TaskA, int &, &TaskA::test2> slot(&foo);
	  Signal1<false, int &> signal;
	  signal << slot;
	  int ret = 0;
	  signal(ret);
	  CPPUNIT_ASSERT(ret == 123);
  }

//CUPPA:decl=-
  CPPUNIT_TEST_SUITE(InvokerTest);
//CUPPA:suite=+
  CPPUNIT_TEST(testSimpleCase);
  CPPUNIT_TEST(testPriority);
  CPPUNIT_TEST(testComplexCase);
  CPPUNIT_TEST(testFunction);
  CPPUNIT_TEST(testSignal);
//CUPPA:suite=-
  CPPUNIT_TEST_SUITE_END();
};

}

namespace core {
//CUPPA:impl=+
//CUPPA:impl=-

CPPUNIT_TEST_SUITE_REGISTRATION(InvokerTest);
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(InvokerTest,"core");

}
