#ifndef _GCTP_OBOX_HPP_
#define _GCTP_OBOX_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult Oriented Boxクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/03/02 14:48:18
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */

#include <gctp/vector.hpp>
#include <iosfwd>               // for std::basic_ostream

namespace gctp {

	/** 有向ボックスクラス
	 *
	 * 主に境界ボックス(OBB:Oriented Bounding Box)として使われる
	 */
	struct OBox {
		Vector position;
		Vector size;
		Quat   posture;

		OBox() {}
		OBox(const Vector &posiiton, const Vector &size, const Quat &posture) : position(position), size(size), posture(posture) {}
		OBox(const Vector &posiiton, const Vector &right, const Vector &up, const Vector &at) : position(position)
		{
			size.x = right.length();
			size.y = up.length();
			size.z = at.length();
			posture.set(right, up, at);
		}
	};

	template<class E, class T> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, OBox const & v)
	{
		os<<"{"<<v.pos<<","<<v.size<<","<<v.posture<<"}";
		return os;
	}

} // namespace gctp

#endif //_GCTP_OBOX_HPP_