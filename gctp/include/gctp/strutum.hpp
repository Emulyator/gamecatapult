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
	 * 変更があった場合だけ再計算するためのフラグを用意
	 */
	class Strutum {
	public:
		/// flagメンバのフラグ値
		enum Flag {
			DIRTY =  1, 
			UPDATED =  1<<1,
			FORCE_UINT_SIZE = sizeof(uint)*8-1,
		};
		/// コンストラクタ
		Strutum() : flags_(0) { wtm_.identify(); lcm_.identify();}
		/// コピーコンストラクタ
		Strutum(const Strutum &src) : wtm_(src.wtm_), lcm_(src.lcm_), flags_(DIRTY) {}
		/// コンストラクタ
		Strutum(const Matrix &src) : lcm_(src), flags_(DIRTY) { wtm_.identify(); }
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
		/// 再計算が必要か？
		bool isDirty() { return (flags_ & DIRTY)>0; }
		/// ダーティーフラグを寝かす
		void refresh() { flags_ &= ~DIRTY; }
		/// ダーティーフラグを立てる
		void dirty() { flags_ |= DIRTY; }
		/// アップデートされたか？
		bool isUpdated() { return (flags_ & UPDATED)>0; }
		/// 更新フラグを寝かす
		void preupdate() { flags_ &= ~UPDATED; }
		/// 更新
		void update(const Matrix &parent)
		{
			wtm_ = lcm_ * parent;
			refresh();
			flags_ |= UPDATED;
		}
		
	private:
		uint flags_;
		Matrix wtm_;
		Matrix lcm_;
		//Vector bone_; ///< ボーンベクトル
		//Vector lbone_; ///< ローカルでのボーンベクトル
	};

	template<class E, class T>
	std::basic_ostream<E, T> &operator<< (std::basic_ostream<E, T> & os, const Strutum & strutum)
	{
		os << "[" << strutum.wtm() << "<-" << strutum.lcm() << "]";
		return os;
	}

} // namespace gctp

#endif //_GCTP_STRUTUM_HPP_