#ifndef _GCTP_SCENE_MOTIONMIXER_HPP_
#define _GCTP_SCENE_MOTIONMIXER_HPP_
/** @file
 * GameCatapult モーション制御クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointer.hpp>
#include <gctp/scene/motion.hpp>

namespace gctp {
	class Skeleton;
}
namespace gctp { namespace scene {

	class Pose;

	/** gctp::Stance と gctp::Coord と gctp::Matrix の共用体
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/12/08 2:37:38
	 */
	struct CoordUnion {
		Stance &stance() { return *reinterpret_cast<Stance *>(__m); }
		Coord  &coord()  { return *reinterpret_cast<Coord *>(__m); }
		Matrix &matrix() { return *reinterpret_cast<Matrix *>(__m); }
		const Stance &stance() const { return *reinterpret_cast<const Stance *>(__m); }
		const Coord  &coord() const { return *reinterpret_cast<const Coord *>(__m); }
		const Matrix &matrix() const { return *reinterpret_cast<const Matrix *>(__m); }
		float __m[16];
		bool is_valid;
	};

	/** モーショントラッククラス
	 *
	 * モーションのトランジションを行う
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 21:52:43
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class MotionTrack {
	public:
		MotionTrack() : custom_postype_(MotionChannel::DEFAULT_POSTYPE), custom_is_open_(MotionChannel::DEFAULT_ISOPEN) {}
		MotionTrack(Handle<Motion> motion, MotionChannel::PosType postype = MotionChannel::DEFAULT_POSTYPE, MotionChannel::IsOpen is_open = MotionChannel::DEFAULT_ISOPEN)
			: motion_(motion), custom_postype_(postype), custom_is_open_(is_open)
			, keytime_(0), speed_(1.0f), accel_(0), weight_(0), weight_delta_(0), do_loop_(false), forward_(false) {}

		void set(Handle<Motion> motion, MotionChannel::PosType postype = MotionChannel::DEFAULT_POSTYPE, MotionChannel::IsOpen is_open = MotionChannel::DEFAULT_ISOPEN)
		{
			motion_ = motion;
			custom_postype_ = postype;
			custom_is_open_ = is_open;
		}

		HRslt update(float delta);

		bool setWork(const Stance &src, const char *name) const;
		bool setWork(const Coord &src, const char *name) const;
		bool setWork(const Matrix &src, const char *name) const;

		const CoordUnion &work() const { return work_; }

		void setPosType(MotionChannel::PosType postype) { custom_postype_ = postype; }
		void setIsOpen(MotionChannel::IsOpen is_open) { custom_is_open_ = is_open; }
		MotionChannel::IsOpen isOpen() const { return custom_is_open_; }
		MotionChannel::PosType posType() const { return custom_postype_; }

		float keytime() const { return keytime_; }
		float setKeytime(float keytime) { std::swap(keytime_, keytime); return keytime; }
		float speed() const { return speed_; }
		float setSpeed(float speed) { std::swap(speed_, speed); return speed; }
		float accel() const { return accel_; }
		float setAccel(float accel) { std::swap(accel_, accel); return accel; }
		
		float weight() const { return weight_; }
		float setWeight(float weight) { std::swap(weight_, weight); return weight; }
		float weightDelta() const { return weight_delta_; }
		float setWeightDelta(float delta) { std::swap(weight_delta_, delta); return delta; }
		bool doLoop() const { return do_loop_; }
		bool setLoop(bool do_loop) { std::swap(do_loop_, do_loop); return do_loop; }

		Handle<Motion> motion() const { return motion_; }

	private:
		mutable CoordUnion work_; // ブレンドのためのワークバッファ
		MotionChannel::IsOpen custom_is_open_;
		MotionChannel::PosType custom_postype_;
		Handle<Motion> motion_;
		float keytime_;
		float speed_;
		float accel_;
		float weight_;
		float weight_delta_;
		bool do_loop_;
		bool forward_;
	};

	/// MotionTrackのvector(そのまんま)
	typedef std::vector<MotionTrack> MotionTrackVector;

	/** モーションミキサークラス
	 *
	 * モーションブレンドして最終的な結果をPoseまたはSkeletonに入れる
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 21:52:43
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class MotionMixer : public Object {
	public:
		MotionMixer();

		HRslt update(float delta);
		void apply(Pose &pose) const;
		void apply(Skeleton &skl) const;
		void applyEx(Skeleton &skl) const;

		void add(Handle<Motion> motion, MotionChannel::PosType postype = MotionChannel::DEFAULT_POSTYPE, MotionChannel::IsOpen is_open = MotionChannel::DEFAULT_ISOPEN);
		MotionTrackVector &tracks() { return tracks_; }
		const MotionTrackVector &tracks() const { return tracks_; }
		bool isExist(MotionTrackVector::size_type idx) const { return (0 <= idx && idx < tracks_.size()); }
		
		float speed() const { return speed_; }
		float setSpeed(float speed) { std::swap(speed_, speed); return speed; }

	private:
		float speed_;
		MotionTrackVector tracks_;
	};

}} // namespace gctp

#endif //_GCTP_SCENE_MOTIONMIXER_HPP_