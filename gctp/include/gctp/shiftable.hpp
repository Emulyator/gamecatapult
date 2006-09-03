#ifndef _GCTP_SHIFTABLE_HPP_
#define _GCTP_SHIFTABLE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 状態遷移テンプレート
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/13 23:00:48
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */

namespace gctp {

	/** 状態遷移テンプレートクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 23:00:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template <typename T>
	class Shiftable {
	protected:
		typedef bool (T::*Method)();
	public:

		/** コンストラクタ
		 *
		 * 初期メソッドを指定する。
		 * 
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/13 23:00:23
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		Shiftable(const Method method) : do_(method) {}

		bool run()
		{
			if(do_) return (((T*)this)->*do_)();
			return false;
		}

	protected:
		/** 遷移
		 *
		 * runで呼び出されるメンバメソッドを指定する。
		 */
		void shiftTo(const Method method)
		{
			do_ = method;
		}

		Method do_;
	};

	/** 状態遷移テンプレートクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 23:00:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template <typename T>
	class InvokeShiftable {
	protected:
		typedef bool (T::*Method)();
	public:

		/** コンストラクタ
		 *
		 * 初期メソッドを指定する。
		 * 
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/02/13 23:00:23
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		InvokeShiftable(const Method method) : do_(method) {}

		bool run()
		{
			if(do_) return (((T*)this)->*do_)();
			return false;
		}

	protected:
		/** 遷移
		 *
		 * runで呼び出されるメンバメソッドを指定する。
		 */
		void shiftTo(const Method method)
		{
			do_ = method;
		}

		Method do_;
	};

} // namespace gctp

#endif //_GCTP_SHIFTABLE_HPP_