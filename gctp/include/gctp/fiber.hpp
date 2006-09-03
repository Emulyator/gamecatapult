#ifndef _GCFIBER_HPP_
#define _GCFIBER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 協調型簡易スレッド（ファイバ）クラス
 *
 * スレッドセーフじゃありません。
 * あとスレッド関数として関数オブジェクトを渡せるようにしたいけど、どうしていいかわからん。
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/08/12 3:00:53
 * $Id: fiber.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
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
#include <boost/utility.hpp>
//#include <boost/function.hpp>
#include <vector>

namespace gctp {

	class Fiber : private boost::noncopyable {
	public:
		// Stacks on Win32 are almost always DWORD aligned and DWORD size. However,
		// this is not actually guaranteed. It is possible to push two bytes onto
		// the stack, which is why I'm not using an uint32_t for my stack type.
		// Since there is no cost for this flexibility, I might as well use it.
		typedef unsigned char StackType;
		typedef std::vector<StackType> Stack;

		Fiber();
		//explicit Fiber(const boost::function0<void>& func);
		explicit Fiber(void (* const func)(void *), void* const data = 0);

		bool operator==(const Fiber& other) const;
		bool operator!=(const Fiber& other) const;

		bool run();
		void kill();
		void join();

		static bool canContinue();
		static void exit();
		static void yield();
		static void sleep(int frames);

	private:
		// どうせMT-SAFEじゃないんで
		/* GCTP_TLS */ static Fiber *current_;
		bool runnable_;
		bool joinable_;
		
		Stack stack_;
		void preserveStack();
		void restoreStack();

#define	COPY_MICRO_THREAD_STACKS
#ifdef	COPY_MICRO_THREAD_STACKS
		// When this is defined you are sharing your main thread stack
		// with the micro threads. You will waste a bit of memory for
		// the unused stack buffer between the two uses.
		// When this is not defined you can not use C++ exceptions, OutputDebugString
		// or structured exception handling inside micro threads. This also means
		// that exceptions from micro threads will not propagate to handlers defined
		// outside of them - unless this is defined.
# define	PUT_MICRO_THREADS_ON_MAIN_THREAD_STACK
# ifdef	PUT_MICRO_THREADS_ON_MAIN_THREAD_STACK
		// どうせMT-SAFEじゃないんで
		/* GCTP_TLS */ static StackType *stack_top_;
		// Specify how much memory you expect to need for your main thread stack.
		// The main disadvantage to having this large is that you are effectively
		// allocating that much memory. Don't specify a number larger than the
		// amount of address space reserved for your stack - one megabyte by default.
		enum {
			MAINSTACKSIZE = 120000,
		};
# else
		// どうせMT-SAFEじゃないんで
		/* GCTP_TLS */ static StackType stack_bottom_[16384];	// Must be big enough for dprintf(), etc.
		/* GCTP_TLS */ static StackType *stack_top_ = stack_bottom_ + NUMELEMENTS(stack_bottom_);
# endif
#else
# pragma message("\tWarning - this option is not recommended. Some types of stack overwriting")
# pragma message("\tmay be detected, but not all. Your code may crash in very hard to")
# pragma message("\tdiagnose ways. Use stack copying. It's very fast.")
		enum {
			// Put your micro-thread stack size here. There is no safe size.
			// It takes a value of several thousand to make SimpleGame run properly.
			// At that point, the memory savings of micro threads have disappeared.
			STACKSIZE = 5000,
			// I put this at the bottom of each micro-thread stack, and watch for it. If it
			// goes missing, it means we've had a stack overwrite.
			MAGICNUMBER = 0xDEADBEEF,
		}
		StackType *sp_;
#endif
	};

} //namespace gctp

#endif //_GCFIBER_HPP_
