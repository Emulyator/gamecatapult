#ifndef _GCTP_AABB_HPP_
#define _GCTP_AABB_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult AABBクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/03/02 14:48:18
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */

#include <gctp/vector.hpp>
#include <iosfwd>               // for std::basic_ostream

namespace gctp {

	/// Axis Aligned Bouncing Box
	struct AABB {
		Vector upper;
		Vector lower;

		AABB() {}
		AABB(const Vector &src) : upper(src), lower(src) {}

		void initialize(const Vector &vec)
		{
			upper = lower = vec;
		}

		void add(const Vector &vec)
		{
			if(upper.x < vec.x) upper.x = vec.x;
			if(upper.y < vec.y) upper.y = vec.y;
			if(upper.z < vec.z) upper.z = vec.z;
			if(lower.x > vec.x) lower.x = vec.x;
			if(lower.y > vec.y) lower.y = vec.y;
			if(lower.z > vec.z) lower.z = vec.z;
		}

		bool isIncluding(const Vector &it) const
		{
			if(lower.x <= it.x && it.x <= upper.x
			&& lower.y <= it.y && it.y <= upper.y
			&& lower.z <= it.z && it.z <= upper.z) return true;
			return false;
		}

		AABB operator & (const AABB &rhs) const
		{
			AABB ret;
			ret.upper.x = (std::min)(upper.x, rhs.upper.x);
			ret.upper.y = (std::min)(upper.y, rhs.upper.y);
			ret.upper.z = (std::min)(upper.z, rhs.upper.z);
			ret.lower.x = (std::max)(lower.x, rhs.lower.x);
			ret.lower.y = (std::max)(lower.y, rhs.lower.y);
			ret.lower.z = (std::max)(lower.z, rhs.lower.z);
			return ret;
		}

		AABB operator | (const AABB &rhs) const
		{
			AABB ret;
			ret.upper.x = (std::max)(upper.x, rhs.upper.x);
			ret.upper.y = (std::max)(upper.y, rhs.upper.y);
			ret.upper.z = (std::max)(upper.z, rhs.upper.z);
			ret.lower.x = (std::min)(lower.x, rhs.lower.x);
			ret.lower.y = (std::min)(lower.y, rhs.lower.y);
			ret.lower.z = (std::min)(lower.z, rhs.lower.z);
			return ret;
		}

		bool isCorrect() const
		{
			return (lower.x <= upper.x && lower.y <= upper.y && lower.z <= upper.z);
		}

		bool isColliding(const AABB &with) const
		{
			return ((*this) & with).isCorrect();
		}
		
		Vector center() const
		{
			return (upper + lower)/2.0f;
		}
	};

	template<class E, class T> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, AABB const & v)
	{
		os<<"{"<<v.upper<<"-"<<v.lower<<"}";
		return os;
	}

} // namespace gctp

#endif //_GCTP_AABB_HPP_
