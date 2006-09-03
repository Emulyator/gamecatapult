#ifndef _GCTP_OBB_HPP_
#define _GCTP_OBB_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult OBBクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/03/02 14:48:18
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */

#include <gctp/vector.hpp>
#include <iosfwd>               // for std::basic_ostream

namespace gctp {

	/// Oriented Bouncing Box
	struct OBB {
		Vector pos;
		Quat   ort;

		OBB() {}
		OBB(const Vector &center, const Quat &orientation) : pos(center), ort(orientation) {}
		OBB(const Vector &center, const Vector &right, const Vector &up, const Vector &at) : pos(center)
		{
			setOrientation(right, up, at);
		}

		void setOrientation(const Vector &right, const Vector &up, const Vector &at)
		{
			ort = Matrix(right, up, at, center);
		}

		const Vector &center() const
		{
			return pos;
		}
		Vector &center()
		{
			return pos;
		}
	};

	template<class E, class T> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, OBB const & v)
	{
		os<<"{"<<v.pos<<","<<v.ort<<"}";
		return os;
	}

} // namespace gctp

#endif //_GCTP_OB_HPP_