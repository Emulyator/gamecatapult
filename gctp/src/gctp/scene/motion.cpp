/** @file
 * GameCatapult （３Ｄモデルの）モーションクラス
 *
 * @todo XDataを取らないようにする（特定のファイルフォーマットへの依存をなくす）。
 *   代わりにデータ追加のインターフェースを用意、SceneFile側からこれが構築されるようにする。
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 22:41:04
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#ifdef _MSC_VER
#pragma warning(disable:4675) // 仕方ないのでここで全体的に...これでも消えないって何なんだ？
#endif
#include "common.h"
#include <gctp/scene/motion.hpp>
#include <gctp/skeleton.hpp>
#include <gctp/dbgout.hpp>
#include <string>
#include <list>
#include <dxfile.h>
#include <rmxfguid.h>
#include <algorithm>

using namespace std;

namespace gctp { namespace scene {

	GCTP_IMPLEMENT_TYPEINFO(MotionChannel, Object);

	void MotionChannel::get(Stance &stance, PosType custom_postype, IsOpen custom_is_open, float keytime) const
	{
		Keys *ks = const_cast<Keys *>(getKeys());
		if(ks) {
			PosType postype;
			bool is_open;
			if(custom_postype == DEFAULT_POSTYPE) postype = posType();
			else postype = custom_postype;
			if(custom_is_open == DEFAULT_ISOPEN) is_open = isOpen();
			else is_open = (custom_is_open == OPEN);
			if(ks->type()==Keys::POSTURE) {
				Quat q;
				if(SPLINE==postype) {
					((PostureKeys *)ks)->get4PInterpolation(q, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((PostureKeys *)ks)->get2PInterpolation(q, is_open, keytime);
				}
				else {
					((PostureKeys *)ks)->getKeyData(q, is_open, keytime);
				}
				stance.posture = q;
			}
			else if(ks->type()==Keys::YPR) {
				Vector v;
				if(SPLINE==postype) {
					((YPRKeys *)ks)->get4PInterpolation(v, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((YPRKeys *)ks)->get2PInterpolation(v, is_open, keytime);
				}
				else {
					((YPRKeys *)ks)->getKeyData(v, is_open, keytime);
				}
				stance.posture = QuatC(v);
			}
			else if(ks->type()==Keys::POSITION) {
				Vector v;
				if(SPLINE==postype) {
					((PositionKeys *)ks)->get4PInterpolation(v, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((PositionKeys *)ks)->get2PInterpolation(v, is_open, keytime);
				}
				else {
					((PositionKeys *)ks)->getKeyData(v, is_open, keytime);
				}
				stance.position = v;
			}
			else if(ks->type()==Keys::MATRIX) {
				Matrix m;
				if(SPLINE==postype) {
					((MatrixKeys *)ks)->get4PInterpolation(m, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((MatrixKeys *)ks)->get2PInterpolation(m, is_open, keytime);
				}
				else {
					((MatrixKeys *)ks)->getKeyData(m, is_open, keytime);
				}
				stance = m;
			}
		}
	}

	void MotionChannel::get(Coord &coord, PosType custom_postype, IsOpen custom_is_open, float keytime) const
	{
		Keys *ks = const_cast<Keys *>(getKeys());
		if(ks) {
			PosType postype;
			bool is_open;
			if(custom_postype == DEFAULT_POSTYPE) postype = posType();
			else postype = custom_postype;
			if(custom_is_open == DEFAULT_ISOPEN) is_open = isOpen();
			else is_open = (custom_is_open == OPEN);
			if(ks->type()==Keys::SCALING) {
				Vector v;
				if(SPLINE==postype) {
					((ScalingKeys *)ks)->get4PInterpolation(v, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((ScalingKeys *)ks)->get2PInterpolation(v, is_open, keytime);
				}
				else {
					((ScalingKeys *)ks)->getKeyData(v, is_open, keytime);
				}
				coord.scale = v;
			}
			else if(ks->type()==Keys::POSTURE) {
				Quat q;
				if(SPLINE==postype) {
					((PostureKeys *)ks)->get4PInterpolation(q, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((PostureKeys *)ks)->get2PInterpolation(q, is_open, keytime);
				}
				else {
					((PostureKeys *)ks)->getKeyData(q, is_open, keytime);
				}
				coord.posture = q;
			}
			else if(ks->type()==Keys::YPR) {
				Vector v;
				if(SPLINE==postype) {
					((YPRKeys *)ks)->get4PInterpolation(v, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((YPRKeys *)ks)->get2PInterpolation(v, is_open, keytime);
				}
				else {
					((YPRKeys *)ks)->getKeyData(v, is_open, keytime);
				}
				coord.posture = QuatC(v);
			}
			else if(ks->type()==Keys::POSITION) {
				Vector v;
				if(SPLINE==postype) {
					((PositionKeys *)ks)->get4PInterpolation(v, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((PositionKeys *)ks)->get2PInterpolation(v, is_open, keytime);
				}
				else {
					((PositionKeys *)ks)->getKeyData(v, is_open, keytime);
				}
				coord.position = v;
			}
			else if(ks->type()==Keys::MATRIX) {
				Matrix m;
				if(SPLINE==postype) {
					((MatrixKeys *)ks)->get4PInterpolation(m, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((MatrixKeys *)ks)->get2PInterpolation(m, is_open, keytime);
				}
				else {
					((MatrixKeys *)ks)->getKeyData(m, is_open, keytime);
				}
				coord = m;
			}
		}
	}

	void MotionChannel::get(Matrix &mat, PosType custom_postype, IsOpen custom_is_open, float keytime) const
	{
		Keys *ks = const_cast<Keys *>(getKeys());
		if(ks) {
			PosType postype;
			bool is_open;
			if(custom_postype == DEFAULT_POSTYPE) postype = posType();
			else postype = custom_postype;
			if(custom_is_open == DEFAULT_ISOPEN) is_open = isOpen();
			else is_open = (custom_is_open==OPEN);
			if(ks->type()==Keys::SCALING) {
				Vector v;
				if(SPLINE==postype) {
					((ScalingKeys *)ks)->get4PInterpolation(v, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((ScalingKeys *)ks)->get2PInterpolation(v, is_open, keytime);
				}
				else {
					((ScalingKeys *)ks)->getKeyData(v, is_open, keytime);
				}
				mat *= Matrix().scale(v);
			}
			else if(ks->type()==Keys::POSTURE) {
				Quat q;
				if(SPLINE==postype) {
					((PostureKeys *)ks)->get4PInterpolation(q, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((PostureKeys *)ks)->get2PInterpolation(q, is_open, keytime);
				}
				else {
					((PostureKeys *)ks)->getKeyData(q, is_open, keytime);
				}
				mat *= q.inverse().toMatrix();
			}
			else if(ks->type()==Keys::YPR) {
				Vector v;
				if(SPLINE==postype) {
					((YPRKeys *)ks)->get4PInterpolation(v, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((YPRKeys *)ks)->get2PInterpolation(v, is_open, keytime);
				}
				else {
					((YPRKeys *)ks)->getKeyData(v, is_open, keytime);
				}
				mat *= Matrix().rot(v.x, v.y, v.z);
			}
			else if(ks->type()==Keys::POSITION) {
				Vector v;
				if(SPLINE==postype) {
					((PositionKeys *)ks)->get4PInterpolation(v, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((PositionKeys *)ks)->get2PInterpolation(v, is_open, keytime);
				}
				else {
					((PositionKeys *)ks)->getKeyData(v, is_open, keytime);
				}
				mat *= Matrix().trans(v);
			}
			else if(ks->type()==Keys::MATRIX) {
				if(SPLINE==postype) {
					((MatrixKeys *)ks)->get4PInterpolation(mat, is_open, keytime);
				}
				else if(LINEAR==postype) {
					((MatrixKeys *)ks)->get2PInterpolation(mat, is_open, keytime);
				}
				else {
					((MatrixKeys *)ks)->getKeyData(mat, is_open, keytime);
				}
			}
		}
	}

	namespace {
		bool predMotionChannel(Pointer<MotionChannel> lhs, Pointer<MotionChannel> rhs)
		{
			return lhs->getKeys()->type() < rhs->getKeys()->type();
		}
	}

	GCTP_IMPLEMENT_TYPEINFO(Motion, Object);

	void Motion::set(CStr name, MotionChannelVector &channels)
	{
#if defined _MSC_VER && _MSC_VER < 1400
#pragma warning(push)
#pragma warning(disable:4675) // 何でこの警告つぶせないんだ？ＯＫなのに！！！！
#endif
		std::sort(channels.begin(), channels.end(), predMotionChannel); // モーションの処理順にソート
#if defined _MSC_VER && _MSC_VER < 1400
#pragma warning(pop)
#endif
		map_[name] = channels;
	}

	void Motion::setPosType(const MotionChannel::PosType _new) {
		for(iterator i = begin(); i != end(); ++i)
			for(MotionChannelVector::iterator j = i->second.begin(); j != i->second.end(); ++j)
				(*j)->setPosType(_new);
	}

	void Motion::setIsOpen(bool is_open) {
		for(iterator i = begin(); i != end(); ++i)
			for(MotionChannelVector::iterator j = i->second.begin(); j != i->second.end(); ++j)
				(*j)->setIsOpen(is_open);
	}

	void Motion::setTicksPerSec(float ticks_per_sec)
	{
		ticks_per_sec_ = ticks_per_sec;
	}

	float Motion::ticksPerSec() const
	{
		return ticks_per_sec_;
	}

	float Motion::time() const
	{
		return ticktime()/ticks_per_sec_;
	}

	ulong Motion::ticktime() const {
		const_iterator i = begin();
		if(i != end()) {
			MotionChannelVector::const_iterator j = i->second.begin();
			if(j != i->second.end()) {
				return (*j)->time();
			}
		}
		return 0;
	}

	
	/** モーションを適用する。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/26 11:42:50
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void Motion::applyTo(Skeleton &skeleton, Real keytime)
	{
		for(Motion::const_iterator i = begin(); i != end(); ++i) {
			Pointer<Skeleton::NodeType> node = skeleton.get(i->first);
			if(node) {
				if(i->second.size() > 0) {
					if(i->second.size() == 1 && i->second[0]->keyType() == MotionChannel::Keys::MATRIX) {
						i->second[0]->get(node->val.getLCM(), MotionChannel::DEFAULT_POSTYPE, MotionChannel::DEFAULT_ISOPEN, keytime);
					}
					else {
						Coord coord(node->val.lcm());
						for(MotionChannelVector::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
							(*j)->get(coord, MotionChannel::DEFAULT_POSTYPE, MotionChannel::DEFAULT_ISOPEN, keytime);
						}
						node->val.getLCM() = coord.toMatrix();
					}
				}
			}
		}
	}

}} // namespace gctp::scene
