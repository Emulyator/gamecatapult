/** @file
 * GameCatapult 協調型スレッド（ファイバ）クラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 *
 * Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
#include "common.h"
#include <gctp/fiber.hpp>
#include <assert.h>

#define	DEBUG_BREAK()	__asm { int 3 }

namespace gctp {

	Fiber *Fiber::current_ = NULL;

	Fiber::StackType *Fiber::stack_top_ = NULL;

	// thread属性つけると、mov命令で扱えない…
	/* GCTP_TLS */ static Fiber::StackType *global_stack_pointer = NULL;

	static GCTP_NAKED void switchContext()
	{
		// Six registers need to be preserved and restored, and this routine
		// is twelve instructions long. Therefore, without using multi-register
		// move instructions, this routine is probably as short as it can be.
		// We could use the multi-register push/pop instructions pushad and popad
		// but that would mean pushing and popping three extra registers. I'm
		// not sure if that would be faster.
		__asm
		{
			// Preserve all of the registers that VC++ insists we preserve.
			// VC++ does not care if we preserve eax, ecx and edx.
			push	ebx
			push	ebp
			push	esi
			push	edi

			// Swap esp and global_stack_pointer
			mov		eax, esp
			mov		esp, global_stack_pointer
			mov		global_stack_pointer, eax

			// Restore all of the registers that we previously preserved.
			// Yes, they're coming off of a different stack - they were
			// carefully placed there earlier.
			pop		edi
			pop		esi
			pop		ebp
			pop		ebx
			ret
		}
	}

	bool Fiber::canContinue()
	{
		return (current_ && !current_->joinable_ && current_->runnable_)? true : false;
	}

	void Fiber::yield()
	{
		if(current_) switchContext();
	}

	// Depending on how you use micro threads it might be useful to implement
	// a true Sleep(n) function that doesn't keep waking up only to go right
	// back to sleep. If your threads frequently sleep for multiple frames this
	// would improve efficiency.
	void Fiber::sleep(int frames)
	{
		if(current_) while(frames--) switchContext();
	}

	void Fiber::exit()
	{
		if(current_) {
			current_->joinable_ = true;
			// We either need to loop forever here or we need to tell the scheduler
			// to not schedule dead threads. It is wasteful to check before scheduling
			// each thread whether it is dead, since this will also need to be checked
			// at a higher level (to see if it should be deleted), so I choose to loop
			// endlessly. If you remove your dead threads promptly, then this is the
			// best choice.
			for(;;) switchContext();
			DEBUG_BREAK();	// We should never get here.
		}
	}

	void Fiber::kill()
	{
		runnable_ = false;
	}

	void Fiber::join()
	{
		while(!joinable_) run();
	}

	// Returns true if the script is still running. If it returns false then you
	// should whoever called SwitchToMicroThread() is responsible for deleting the
	// script.
	bool Fiber::run()
	{
		// Dead threads will be scheduled, but they won't do anything.
		// This is very bad. Don't schedule one thread from another.
		// The RestoreStack function assumes that it can use the
		// Fiber::current_ area of memory, but the currently executing
		// thread is already using it. It is possible to orchestrate a switch,
		// but I have chosen not too.
		if(joinable_) return false;
		#ifdef	GCTP_DEBUG
		if (current_)
			DEBUG_BREAK();
		#endif
		current_ = this;
		restoreStack();
		// This function is pure magic. It switches to the thread's stack, and implicitly
		// to the thread, and when it calls MicroThreadSleep() it switches right back to
		// here.
		yield();
		preserveStack();
		current_ = NULL;
		return !joinable_;
	}

	void Fiber::preserveStack()
	{
	#ifdef COPY_MICRO_THREAD_STACKS
		size_t stack_size = stack_top_ - global_stack_pointer;
		// For some reason a resize and then memmove is much faster than a
		// clear and then insert.
		stack_.resize(stack_size);
		memmove(&stack_[0], global_stack_pointer, stack_size);
		//stack_.clear();
		//stack_.insert(stack_.begin(), global_stack_pointer, stack_top_);
	#else
		current_->sp_ = global_stack_pointer;
		if(*reinterpret_cast<uint32_t*>(&current_->sp_[0]) != MAGICNUMBER)
			DEBUG_BREAK();	// Stack overwrite! Memory corruption! Very serious problem!
	#endif
	}

	void Fiber::restoreStack()
	{
	#ifdef COPY_MICRO_THREAD_STACKS
		#if defined(PUT_MICRO_THREADS_ON_MAIN_THREAD_STACK) && defined(_DEBUG)
			int local_variable;
			// Check for stack conflicts.
			// The current stack pointer is detected by the address of a
			// local variable.
			StackType* current_stack_pointer = (StackType*)&local_variable;
			const int safety_margin = 1000;
			// If our current stack is close to or below our micro thread working
			// area then we will overwrite it if we proceed. We MUST stop immediately.
			if(current_stack_pointer - safety_margin < stack_top_)
				DEBUG_BREAK();
		#endif
		global_stack_pointer = stack_top_ - stack_.size();
		memmove(global_stack_pointer, &(stack_[0]), stack_.size() * sizeof(StackType));
	#else
		global_stack_pointer = current_->sp_;
	#endif
	}

#ifdef	PUT_MICRO_THREADS_ON_MAIN_THREAD_STACK
	const int usage_per_fiber = 1000;
	// Temporarily turn off optimizations, because otherwise this function
	// will be optimized away completely.
	#pragma warning( disable:4748 )
	#pragma optimize( "", off )
	static char* useless_buffer;
	static void primeStack(int x)
	{
		// Use up big chunks of stack.
		char unused_array[usage_per_fiber];
		useless_buffer = unused_array;	// Avoid unreferenced local variable warning.
		if(x > 0) primeStack(x - 1);
	}
	#pragma optimize( "", on ) 
	#pragma warning( default:4748 )
#endif

	//Fiber::Fiber(const boost::function0<void>& func) : req_quit_(false), joinable_(false)
	//Fiber::Fiber(const boost::function0<void>& func) : stack_(STACKSIZE), req_quit_(false), joinable_(false)
#ifdef COPY_MICRO_THREAD_STACKS
	Fiber::Fiber(void (* const func)(void *), void* const data) : runnable_(true), joinable_(false)
#else
	Fiber::Fiber(void (* const func)(void *), void* const data) : stack_(STACKSIZE), runnable_(true), joinable_(false)
#endif
	{
	#ifdef	PUT_MICRO_THREADS_ON_MAIN_THREAD_STACK
		static bool primed = false;
		if (!primed) {
			// Make the OS commit memory far enough down the stack.
			primeStack(MAINSTACKSIZE / usage_per_fiber);
			int local_variable;
			// Initialize the script stack top to an appropriate location.
			// The current stack pointer is detected by the address of a
			// local variable.
			stack_top_ = ((StackType*)&local_variable) - MAINSTACKSIZE;
			primed = true;
		}
	#endif
		// All we have to do to initialize a new micro thread is create a plausible initial
		// stack, containing the needed registers. That's it.

		// At the bottom of the stack we have the four registers that we have to preserve and
		// restore. Above that we have the address of our startup function that we fall back
		// into. Once we've returned to that function we want to look like we just called
		// that function, so the next value down is the return address. We put MicroThreadReturn
		// there so that if the users thread routine returns they will automatically call
		// it. Lastly we push on the user specified parameter. Done.
		// There is something vaguely distressing about returning to the beginning of a
		// function, but there's nothing wrong with it.
		void* initial_stack[] = {0, 0, 0, 0, func, exit, data};
	#ifdef COPY_MICRO_THREAD_STACKS
		stack_.insert(stack_.begin(), reinterpret_cast<StackType*>(initial_stack), (reinterpret_cast<StackType*>(initial_stack)) + sizeof(initial_stack));
	#else
		assert(stack_.size() == STACKSIZE);
		*(reinterpret_cast<uint32_t*>(&stack_[0])) = MAGICNUMBER;	// Stack overwrite detection.
		sp_ = &stack_[STACKSIZE - sizeof(initial_stack)];
		memcpy(sp_, initial_stack, sizeof(initial_stack));
	#endif
	}

} // namespace gctp
