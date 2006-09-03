#ifndef _GCTP_SCENE_POSE_HPP_
#define _GCTP_SCENE_POSE_HPP_
/** @file
 * GameCatapult 行列階層クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/object.hpp>
#include <gctp/class.hpp>
#include <gctp/cstr.hpp>
#include <gctp/stance.hpp>
#include <gctp/scene/motion.hpp>
#include <map>
#include <string>
#include <iosfwd>               // for std::basic_ostream

namespace gctp { 
	class Skeleton;
}

namespace gctp { namespace scene {

	class Motion;
	/** ポーズクラス
	 *
	 * 階層名インデックス付きの位置情報(Stance)クラス
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 21:52:43
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Pose : public Object
	{
	public:
		Pose() {}
		Pose(const Skeleton &skel);

		typedef std::map<CStr, Stance> StanceIndex;
		Stance *add(const char *key);
		Stance *add(const CStr key);
		const Stance *find(const char *key) const;
		Stance *find(const char *key);

		void set(const Motion &motion, MotionChannel::PosType custom_postype, MotionChannel::IsOpen custom_isopen, float keytime);
		void set(const char *name, const MotionChannel &motion, MotionChannel::PosType custom_postype, MotionChannel::IsOpen custom_isopen, float keytime);
		void set(Skeleton &skeleton);

		StanceIndex::const_iterator begin() const { return index_.begin(); }
		StanceIndex::iterator begin() { return index_.begin(); }
		StanceIndex::const_iterator end() const { return index_.end(); }
		StanceIndex::iterator end() { return index_.end(); }

	GCTP_DECLARE_CLASS

	protected:
		StanceIndex index_;
	};

	template<class E, class T>
	std::basic_ostream<E, T> &operator<< (std::basic_ostream<E, T> & os, const Pose & p)
	{
		for(Pose::StanceIndex::const_iterator i = p.begin(); i != p.end(); ++i) {
			os << i->first << ":" << i->second << ";";
		}
		return os;
	}

}} // namespace gctp

#endif //_GCTP_SCENE_POSE_HPP_