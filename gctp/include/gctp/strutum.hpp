#ifndef _GCTP_STRUTUM_HPP_
#define _GCTP_STRUTUM_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 階層クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/matrix.hpp>
#include <gctp/sphere.hpp>
#include <gctp/stance.hpp>
#include <gctp/coord.hpp>
#include <iosfwd>               // for std::basic_ostream

namespace gctp {

	/** 階層情報
	 *
	 */
	class Strutum {
		/// flagメンバのフラグ値
		enum Flag {
			DIRTY			=  1,
			UPDATED			=  1<<1,
			INDEPENDENT		=  1<<2,
			CONTINUITY		=  1<<3,
			FORCE_UINT_SIZE	= sizeof(uint)*8-1,
		};
	public:
		/// コンストラクタ
		Strutum() : bone_(VectorC(0,0,0)), flags_(0) { wtm_.identify(); lcm_.identify();}
		/// コピーコンストラクタ
		Strutum(const Strutum &src) : wtm_(src.wtm_), lcm_(src.lcm_), bone_(src.bone_), flags_(DIRTY) {}
		/// コンストラクタ
		Strutum(const Matrix &src) : lcm_(src), bone_(VectorC(0,0,0)), flags_(DIRTY) { wtm_.identify(); }
		/// コンストラクタ
//		Strutum(const Stance &src) : lcm_(src), flags_(DIRTY) { wtm_.identify(); }
		/// コンストラクタ
//		Strutum(const Coord &src) : lcm_(src), flags_(DIRTY) { wtm_.identify(); }
		
		/// 代入演算子
		Strutum &operator=(const Matrix &src) { dirty(); lcm_ = src; return *this; }
		/// 代入演算子
//		Strutum &operator=(const Stance &src) { dirty(); lcm_ = src; return *this; }
		/// 代入演算子
//		Strutum &operator=(const Coord &src) { dirty(); lcm_ = src; return *this; }

		/// ワールド変換行列
		const Matrix &wtm() const { return wtm_; }
		/// ワールド変換行列
		Matrix &wtm() { return wtm_; }
		/// 局所座標系行列
		const Matrix &lcm() const { return lcm_; }
		/// 局所座標系行列(ダーティーフラグがセットされる)
		Matrix &getLCM() { dirty(); return lcm_; }
		/// ボーンベクトル
		const Vector &bone() const { return bone_; }
		/// ボーンベクトル
		Vector &bone() { return bone_; }
		/// 再計算が必要か？
		bool isDirty() const { return (flags_ & DIRTY) > 0; }
		/// ダーティーフラグを寝かす
		void refresh() { flags_ &= ~DIRTY; }
		/// ダーティーフラグを立てる
		void dirty() { flags_ |= DIRTY; }
		/// アップデートされたか？
		bool isUpdated() const { return (flags_ & UPDATED) > 0; }
		/// 更新フラグを寝かす
		void preupdate() { flags_ &= ~UPDATED; }
		/// 更新
		void update(const Matrix &parent)
		{
			if(!isDependent()) {
				if(isContinuos()) prev_wtm_ = wtm_;
				wtm_ = lcm_ * parent;
				if(!isContinuos()) prev_wtm_ = wtm_;
				flags_ = UPDATED|CONTINUITY;
			}
		}
		/// wtmを強制セット
		void updateWTM(const Matrix &wtm)
		{
			if(isContinuos()) prev_wtm_ = wtm_;
			wtm_ = wtm;
			if(!isContinuos()) prev_wtm_ = wtm_;
			flags_ = UPDATED|INDEPENDENT|CONTINUITY;
		}

		/// 前回ワールド変換行列
		const Matrix &prevWTM() const { return isContinuos() ? prev_wtm_ : wtm_; }
		/// 前回ワールド変換行列
		Matrix &prevWTM() { return isContinuos() ? prev_wtm_ : wtm_; }
		/// 連続か？(つまり、前回wtmに有効な値が入っているか？)
		bool isContinuos() const { return (flags_ & CONTINUITY) > 0; }
		/// 連続フラグを寝かす
		void discreet() { flags_ &= ~CONTINUITY; }
		/// 独立か？
		bool isDependent() { return (flags_ & INDEPENDENT) > 0; }
		/// 独立フラグを寝かす
		void depend() { flags_ &= ~INDEPENDENT; }

	private:
		Matrix prev_wtm_;
		Matrix wtm_;
		Matrix lcm_;
		Vector bone_; // ボーンベクトル
		uint flags_;
	};

	template<class E, class T>
	std::basic_ostream<E, T> &operator<< (std::basic_ostream<E, T> & os, const Strutum & strutum)
	{
		os << "[" << strutum.wtm() << "<-" << strutum.lcm() << "]";
		return os;
	}

} // namespace gctp

#endif //_GCTP_STRUTUM_HPP_