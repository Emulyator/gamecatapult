/** @file
 * GameCatapult モーション制御クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/motionmixer.hpp>
#include <gctp/scene/pose.hpp>
#include <gctp/scene/motion.hpp>
#include <gctp/skeleton.hpp>
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
		if(speed_!=0) {
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
		}
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
				work_.type = CoordUnion::TYPE_STANCE;
			}
		}
		else work_.type = CoordUnion::TYPE_NONE;
		return work_.type != CoordUnion::TYPE_NONE;
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
				work_.type = CoordUnion::TYPE_COORD;
			}
		}
		else work_.type = CoordUnion::TYPE_NONE;
		return work_.type != CoordUnion::TYPE_NONE;
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
					work_.matrix() = coord.toMatrix();
				}
				work_.type = CoordUnion::TYPE_MATRIX;
			}
		}
		else work_.type = CoordUnion::TYPE_NONE;
		return work_.type != CoordUnion::TYPE_NONE;
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
	 * @param pose モーションを適用するPose
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
					if(j->work().type != CoordUnion::TYPE_NONE) {
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
	 * @param skl モーションを適用するSkelton
	 * @param initial_skl 初期値を持っているSkelton
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/05 0:18:41
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void MotionMixer::apply(Skeleton &skl, const Skeleton &initial_skl) const
	{
		for(Skeleton::NodeIndex::iterator i = skl.index.begin(); i != skl.index.end(); ++i) {
			float weight_sum = 0;
			bool ok = false;
			for(MotionTrackVector::const_iterator j = tracks_.begin(); j != tracks_.end(); ++j) {
				if(j->setWork(Coord(reinterpret_cast<Skeleton::NodeType *>(initial_skl[i->first])->val.lcm()), i->first.c_str())) {
					weight_sum += j->weight();
					ok = true;
				}
			}
			if(ok) {
				bool first = true;
				Coord coord;
				for(MotionTrackVector::const_iterator j = tracks_.begin(); j != tracks_.end(); ++j) {
					if(j->work().type != CoordUnion::TYPE_NONE) {
						if(first) {
							coord = j->work().coord()*(j->weight()/weight_sum);
							first = false;
						}
						else coord += j->work().coord()*(j->weight()/weight_sum);
					}
				}
				reinterpret_cast<Skeleton::NodeType *>(i->second)->val.getLCM() = coord.toMatrix();
			}
		}
	}

	/** トラック間のモーションブレンドをした値を適用
	 *
	 * 初期値をあらわすSkeltonが必要ない代わりに、ブレンドをCoordではなくMatrixで行うため、
	 * フリップが発生する可能性あり。//uso その代わり、せん断など、直交行列でない行列キーも処理できる。
	 * @param skl モーションを適用するSkelton
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
				if(j->setWork(reinterpret_cast<Skeleton::NodeType *>(i->second)->val.lcm(), i->first.c_str())) {
					weight_sum += j->weight();
					ok = true;
				}
			}
			if(ok) {
				bool first = true;
				for(MotionTrackVector::const_iterator j = tracks_.begin(); j != tracks_.end(); ++j) {
					if(j->work().type != CoordUnion::TYPE_NONE) {
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

	bool MotionMixer::setUp(luapp::Stack &L)
	{
		// Entityからの取得のみ
		return false;
	}

	void MotionMixer::setMasterSpeed(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			speed_ = (float)L[1].toNumber();
		}
	}

	int MotionMixer::getMasterSpeed(luapp::Stack &L)
	{
		L << speed_;
		return 1;
	}

	int MotionMixer::trackNum(luapp::Stack &L)
	{
		L << (int)tracks().size();
		return 1;
	}

	int MotionMixer::getTotalTime(luapp::Stack &L)
	{
		if(L.top() >= 1 && tracks()[L[1].toInteger()].motion()) {
			L << tracks()[L[1].toInteger()].motion()->time();
			return 1;
		}
		return 0;
	}

	void MotionMixer::setKeyTime(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			tracks()[L[1].toInteger()].setKeytime((float)L[2].toNumber());
		}
		else if(L.top() >= 1) {
			tracks()[L[1].toInteger()].setKeytime(0);
		}
	}

	int MotionMixer::getKeyTime(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			L << tracks()[L[1].toInteger()].keytime();
			return 1;
		}
		return 0;
	}

	void MotionMixer::setWeight(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			tracks()[L[1].toInteger()].setWeight((float)L[2].toNumber());
		}
		else if(L.top() >= 1) {
			tracks()[L[1].toInteger()].setWeight(0);
		}
	}

	int MotionMixer::getWeight(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			L << tracks()[L[1].toInteger()].weight();
			return 1;
		}
		return 0;
	}

	void MotionMixer::setSpeed(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			tracks()[L[1].toInteger()].setSpeed((float)L[2].toNumber());
		}
		else if(L.top() >= 1) {
			tracks()[L[1].toInteger()].setSpeed(0);
		}
	}

	int MotionMixer::getSpeed(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			L << tracks()[L[1].toInteger()].speed();
			return 1;
		}
		return 0;
	}

	void MotionMixer::setLoop(luapp::Stack &L)
	{
		if(L.top() >= 2) {
			tracks()[L[1].toInteger()].setLoop(L[2].toBoolean());
		}
		else if(L.top() >= 1) {
			tracks()[L[1].toInteger()].setLoop(false);
		}
	}

	int MotionMixer::getLoop(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			L << tracks()[L[1].toInteger()].doLoop();
			return 1;
		}
		return 0;
	}

	void MotionMixer::toOpen(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			tracks()[L[1].toInteger()].setIsOpen(MotionChannel::OPEN);
		}
	}

	void MotionMixer::toClose(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			tracks()[L[1].toInteger()].setIsOpen(MotionChannel::CLOSE);
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, MotionMixer, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, MotionMixer)
		TUKI_METHOD(MotionMixer, trackNum)
		TUKI_METHOD(MotionMixer, setMasterSpeed)
		TUKI_METHOD(MotionMixer, getMasterSpeed)
		TUKI_METHOD(MotionMixer, getTotalTime)
		TUKI_METHOD(MotionMixer, setKeyTime)
		TUKI_METHOD(MotionMixer, getKeyTime)
		TUKI_METHOD(MotionMixer, setWeight)
		TUKI_METHOD(MotionMixer, getWeight)
		TUKI_METHOD(MotionMixer, setSpeed)
		TUKI_METHOD(MotionMixer, getSpeed)
		TUKI_METHOD(MotionMixer, setLoop)
		TUKI_METHOD(MotionMixer, getLoop)
		TUKI_METHOD(MotionMixer, toOpen)
		TUKI_METHOD(MotionMixer, toClose)
	TUKI_IMPLEMENT_END(MotionMixer)

}} // namespace gctp
