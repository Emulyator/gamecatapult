#ifndef _GCTP_SHIFTABLE_HPP_
#define _GCTP_SHIFTABLE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ��ԑJ�ڃe���v���[�g
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/13 23:00:48
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */

namespace gctp {

	/** ��ԑJ�ڃe���v���[�g�N���X
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

		/** �R���X�g���N�^
		 *
		 * �������\�b�h���w�肷��B
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
		/** �J��
		 *
		 * run�ŌĂяo����郁���o���\�b�h���w�肷��B
		 */
		void shiftTo(const Method method)
		{
			do_ = method;
		}

		Method do_;
	};

	/** ��ԑJ�ڃe���v���[�g�N���X
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

		/** �R���X�g���N�^
		 *
		 * �������\�b�h���w�肷��B
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
		/** �J��
		 *
		 * run�ŌĂяo����郁���o���\�b�h���w�肷��B
		 */
		void shiftTo(const Method method)
		{
			do_ = method;
		}

		Method do_;
	};

} // namespace gctp

#endif //_GCTP_SHIFTABLE_HPP_