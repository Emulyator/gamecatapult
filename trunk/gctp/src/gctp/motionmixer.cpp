/** @file
 * GameCatapult モーション制御クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/motionmixer.hpp>
#include <gctp/pose.hpp>
#include <gctp/skeleton.hpp>
#include <gctp/motion.hpp>
#include <gctp/coord.hpp>

using namespace std;

namespace gctp { namespace scene {

	template<typename _T> _T clump(_T src, _T min, _T max)
	{
		if(src < min) return min;
		if(src > max) return max;
		return src;
	}

	/** 更新
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/06 2:06:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt MotionTrack::update(float delta)
	{
		Pointer<Motion> motion = motion_.lock();
		forward_ = (speed_*delta >= 0);
		keytime_ += speed_*delta;
		if(do_loop_) {
			keytime_ = fmodf(keytime_, motion->time());
			if(keytime_ < 0) keytime_ += motion->time();
		}
		else if(keytime_ < 0) keytime_ = 0;
		else if(keytime_ > motion->time()) keytime_ = motion->time();
		speed_ += accel_*fabsf(delta);
		weight_ += weight_delta_*fabsf(delta);
		weight_ = clump(weight_, 0.0f, 1.0f);
		return S_OK;
	}

	/** Stanceにモーションを適用
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/06 2:17:50
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool MotionTrack::setWork(const Stance &src, const char *name) const
	{
		if(weight_ > 0) {
			Pointer<Motion> motion = motion_.lock();
			work_.stance() = src;
			float ticks_per_sec = motion->ticksPerSec();
			Motion::const_iterator i = motion->find(name);
			if(i != motion->end() && i->second.size() > 0) {
				for(MotionChannelVector::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
					(*j)->get(work_.stance(), custom_postype_, custom_is_open_, keytime_*ticks_per_sec);
				}
				work_.is_valid = true;
				return true;
			}
		}
		work_.is_valid = false;
		return false;
	}

	/** Coordにモーションを適用
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/06 2:17:50
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool MotionTrack::setWork(const Coord &src, const char *name) const
	{
		if(weight_ > 0) {
			work_.coord() = src;
			Pointer<Motion> motion = motion_.lock();
			float ticks_per_sec = motion->ticksPerSec();
			Motion::const_iterator i = motion->find(name);
			if(i != motion->end() && i->second.size() > 0) {
				for(MotionChannelVector::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
					(*j)->get(work_.coord(), custom_postype_, custom_is_open_, keytime_*ticks_per_sec);
				}
				work_.is_valid = true;
				return true;
			}
		}
		work_.is_valid = false;
		return false;
	}

	/** Matrixにモーションを適用
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/06 2:17:50
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool MotionTrack::setWork(const Matrix &src, const char *name) const
	{
		if(weight_ > 0) {
			work_.matrix() = src;
			Pointer<Motion> motion = motion_.lock();
			float ticks_per_sec = motion->ticksPerSec();
			Motion::const_iterator i = motion->find(name);
			if(i != motion->end() && i->second.size() > 0) {
				if(i->second.size() == 1 && i->second[0]->keyType() == MotionChannel::Keys::MATRIX) {
					i->second[0]->get(work_.matrix(), custom_postype_, custom_is_open_, keytime_*ticks_per_sec);
				}
				else {
					Coord coord(work_.matrix());
					for(MotionChannelVector::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
						(*j)->get(coord, custom_postype_, custom_is_open_, keytime_*ticks_per_sec);
					}
					work_.matrix() = coord;
				}
				work_.is_valid = true;
				return true;
			}
		}
		work_.is_valid = false;
		return false;
	}

	MotionMixer::MotionMixer() : speed_(1.0f) {}

	/** トラックを追加。
	 *
	 * デフォルトでは
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/08 0:42:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void MotionMixer::add(Handle<Motion> motion, MotionChannel::PosType postype, MotionChannel::IsOpen is_open)
	{
		tracks_.push_back(MotionTrack(motion, postype, is_open));
	}

	/** 更新
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/06 2:06:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt MotionMixer::update(float delta)
	{
		for(MotionTrackVector::iterator i = tracks_.begin(); i != tracks_.end(); ++i) {
			i->update(delta*speed_);
		}
		return S_OK;
	}

	/** トラック間のモーションブレンドをした値を適用
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/05 0:18:09
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void MotionMixer::apply(Pose &pose) const
	{
		for(Pose::StanceIndex::iterator i = pose.begin(); i != pose.end(); ++i) {
			float weight_sum = 0;
			bool ok = false;
			for(MotionTrackVector::const_iterator j = tracks_.begin(); j != tracks_.end(); ++j) {
				if(j->setWork(i->second, i->first.c_str())) {
					weight_sum += j->weight();
					ok = true;
				}
			}
			if(ok) {
				bool first = true;
				for(MotionTrackVector::const_iterator j = tracks_.begin(); j != tracks_.end(); ++j) {
					if(j->work().is_valid) {
						if(first) {
							i->second = j->work().stance()*(j->weight()/weight_sum);
							first = false;
						}
						else i->second += j->work().stance()*(j->weight()/weight_sum);
					}
				}
			}
		}
	}

	/** トラック間のモーションブレンドをした値を適用
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/05 0:18:41
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void MotionMixer::apply(Skeleton &skl) const
	{
		for(Skeleton::NodeIndex::iterator i = skl.index.begin(); i != skl.index.end(); ++i) {
			float weight_sum = 0;
			bool ok = false;
			for(MotionTrackVector::const_iterator j = tracks_.begin(); j != tracks_.end(); ++j) {
				if(j->setWork(Coord(reinterpret_cast<Skeleton::NodeType *>(i->second)->val.lcm()), i->first.c_str())) {
					weight_sum += j->weight();
					ok = true;
				}
			}
			if(ok) {
				bool first = true;
				Coord coord;
				for(MotionTrackVector::const_iterator j = tracks_.begin(); j != tracks_.end(); ++j) {
					if(j->work().is_valid) {
						if(first) {
							coord = j->work().coord()*(j->weight()/weight_sum);
							first = false;
						}
						else coord += j->work().coord()*(j->weight()/weight_sum);
					}
				}
				coord.posture.normalize();
				reinterpret_cast<Skeleton::NodeType *>(i->second)->val.getLCM() = coord;
			}
		}
	}

	/** トラック間のモーションブレンドをした値を適用
	 *
	 * 直交行列でない行列キーが含まれる場合に使用する。
	 *
	 * せん断など、行列キー特有のモーションを適用する場合、こちらを使用する。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/05 0:18:41
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void MotionMixer::applyEx(Skeleton &skl) const
	{
		for(Skeleton::NodeIndex::iterator i = skl.index.begin(); i != skl.index.end(); ++i) {
			float weight_sum = 0;
			bool ok = false;
			for(MotionTrackVector::const_iterator j = tracks_.begin(); j != tracks_.end(); ++j) {
				if(j->setWork(reinterpret_cast<Skeleton::NodeType *>(i->second)->val.lcm(), i->first.c_str())) {
					weight_sum += j->weight();
					ok = true;
				}
			}
			if(ok) {
				bool first = true;
				for(MotionTrackVector::const_iterator j = tracks_.begin(); j != tracks_.end(); ++j) {
					if(j->work().is_valid) {
						if(first) {
							reinterpret_cast<Skeleton::NodeType *>(i->second)->val.getLCM() = j->work().matrix()*(j->weight()/weight_sum);
							first = false;
						}
						else reinterpret_cast<Skeleton::NodeType *>(i->second)->val.getLCM() += j->work().matrix()*(j->weight()/weight_sum);
					}
				}
			}
		}
	}

}} // namespace gctp
