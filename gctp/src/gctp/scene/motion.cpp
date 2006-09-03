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

	/** XFileからの読みこみ
	 */
	HRslt MotionChannel::setUp(const XData &dat/**< カレントの位置を示すXDataオブジェクト*/)
	{
		HRslt hr;
		const XKeys *keys = dat.keys();
		if(keys) {
			if(keys->type == XKEY_SCALING) {
				const XScalingKeys *anim_keys = dat.sclkeys();
				ScalingKeys *w = new ScalingKeys(Keys::SCALING, anim_keys->num);
				p_ = auto_ptr<Keys>(w);
				//個数分コピー
				for(uint i = 0; i < anim_keys->num; i++) (*w)[i] = anim_keys->keys[i];
				//PRNN("ScalingKey read");
			}
			else if(keys->type == XKEY_POSTURE) {
				const XPostureKeys *anim_keys = dat.posturekeys();
				PostureKeys *w = new PostureKeys(Keys::POSTURE, anim_keys->num);
				p_ = auto_ptr<Keys>(w);
				//個数分コピー
				for(uint i = 0; i < anim_keys->num; i++) (*w)[i] = anim_keys->keys[i];
				//PRNN("PostureKey read");
			}
			else if(keys->type == XKEY_YPR) {
				const XYPRKeys *anim_keys = dat.yprkeys();
				YPRKeys *w = new YPRKeys(Keys::YPR, anim_keys->num);
				p_ = auto_ptr<Keys>(w);
				//個数分コピー
				for(uint i = 0; i < anim_keys->num; i++) (*w)[i] = anim_keys->keys[i];
				//PRNN("YPRKey read");
			}
			else if(keys->type == XKEY_POSITION) {
				const XPositionKeys *anim_keys = dat.positionkeys();
				PositionKeys *w = new PositionKeys(Keys::POSITION, anim_keys->num);
				p_ = auto_ptr<Keys>(w);
				//個数分コピー
				for(uint i = 0; i < anim_keys->num; i++) (*w)[i] = anim_keys->keys[i];
				//PRNN("PositionKey read");
			}
			else if(keys->type == XKEY_MATRIX) {
				const XMatrixKeys *anim_keys = dat.matkeys();
				MatrixKeys *w = new MatrixKeys(Keys::MATRIX, anim_keys->num);
				p_ = auto_ptr<Keys>(w);
				//個数分コピー
				for(uint i = 0; i < anim_keys->num; i++) (*w)[i] = anim_keys->keys[i];
				//PRNN("MatrixKey read");
			}
			else {
				PRNN("Motion:"<<keys->type<<":対応していないキータイプです。");
				hr = E_FAIL;
			}
		}
		else hr = E_FAIL;
		return hr;
	}

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

	/**
	 * XFileの読みこみ
	 */
	HRslt Motion::setUp(const XData &cur/**< カレントの位置を示すXDataオブジェクト*/)
	{
		HRslt hr;
		for(uint i = 0; i < cur.size(); i++) {
			XData setdat = cur[i];
			if(TID_D3DRMAnimation == setdat.type()) {
				bool is_open = true;
				MotionChannel::PosType postype = MotionChannel::LINEAR;
				CStr framename;
				MotionChannelVector channels;
				//PRNN("chunk name : "<<cur.name());
				for(uint i = 0; i < setdat.size(); i++) {
					XData dat = setdat[i];
					if(!dat.isRef()) {
						if( TID_D3DRMFrame == dat.type() ) {
							// 参照でなく、直接フレームが格納されていることがあるようだ……
							// 対応しなきゃだめか……
							/*hr = LoadFrames(pxofobjChild, pde, options, fvf, graphic(), pframeCur);
							if (FAILED(hr))
								goto e_Exit;*/
							PRNN("Motion::フレームの指定は参照以外対応していません。");
						}
						else if( TID_D3DRMAnimationOptions == dat.type() ) {
							const XAnimOption *opt = dat.animoption();
							if(opt) {
								is_open = (opt->openclosed==1)? true : false;
								postype = (opt->positionquality == 1)? MotionChannel::SPLINE : MotionChannel::LINEAR;
							}
						}
						else if( TID_D3DRMAnimationKey == dat.type() ) {
							MotionChannel *channel = new MotionChannel;
							if(channel) {
								hr = channel->setUp(dat);
								if(!hr) delete channel;
								else channels.push_back(Pointer<MotionChannel>(channel));
							}
							else hr = E_FAIL;
						}
					}
					else {
						if(TID_D3DRMFrame == dat.type()) {
							framename = dat.name();
						}
						else {
							PRNN("AnimationRsrc::対応していない参照データです。");
							hr = E_FAIL;
						}
					}
				}

				for(MotionChannelVector::iterator i = channels.begin(); i != channels.end(); ++i) {
					(*i)->setIsOpen(is_open);
					(*i)->setPosType(postype);
				}
#if defined _MSC_VER && _MSC_VER < 1400
#pragma warning(push)
#pragma warning(disable:4675) // 何でこの警告つぶせないんだ？ＯＫなのに！！！！
#endif
				std::sort(channels.begin(), channels.end(), predMotionChannel); // モーションの処理順にソート
#if defined _MSC_VER && _MSC_VER < 1400
#pragma warning(pop)
#endif
				if(!framename.empty()) map_[framename] = channels;
			}
		}
		return hr;
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
