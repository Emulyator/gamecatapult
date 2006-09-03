/** @file
 * GameCatapult 行列階層クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/pose.hpp>
#include <gctp/skeleton.hpp>

using namespace std;

namespace gctp { namespace scene {

	GCTP_IMPLEMENT_CLASS_NS(gctp, Pose, Object);

	/** 同じボーン名も持つPoseを製作
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 21:58:10
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Pose::Pose(const Skeleton &skel)
	{
		for(Skeleton::NodeIndex::const_iterator i = skel.index.begin(); i != skel.index.begin(); ++i) {
			add(i->first);
		}
	}

	Stance *Pose::add(const char *key)
	{
		return &index_[key];
	}

	Stance *Pose::add(const CStr key)
	{
		return &index_[key];
	}

	const Stance *Pose::find(const char *key) const
	{
		StanceIndex::const_iterator i = index_.find(key);
		if(i != index_.end()) return &i->second;
		return NULL;
	}

	Stance *Pose::find(const char *key)
	{
		StanceIndex::iterator i = index_.find(key);
		if(i != index_.end()) return &i->second;
		return NULL;
	}
	
	void Pose::set(const Motion &motion, MotionChannel::PosType custom_postype, MotionChannel::IsOpen custom_isopen, float keytime)
	{
		for(Motion::const_iterator i = motion.begin(); i != motion.end(); ++i) {
			for(MotionChannelVector::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
				set(i->first.c_str(), **j, custom_postype, custom_isopen, keytime);
			}
		}
	}

	void Pose::set(const char *name, const MotionChannel &channel, MotionChannel::PosType custom_postype, MotionChannel::IsOpen custom_isopen, float keytime)
	{
		Stance *stance = find(name);
		if(stance) channel.get(*stance, custom_postype, custom_isopen, keytime);
	}

	/** 名前が一致するノードに、ポーズの値を書き込む
	 *
	 * @note 未完成
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/26 9:01:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void Pose::set(Skeleton &skeleton)
	{
		skeleton.setIdentity();
	}

}} // namespace gctp
