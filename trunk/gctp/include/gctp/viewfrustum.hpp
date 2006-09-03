#ifndef _GCTP_VIEWFRUSTUM_HPP_
#define _GCTP_VIEWFRUSTUM_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 視錐台クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/plane.hpp>
#include <gctp/sphere.hpp>

namespace gctp {

	/// 視錐体クラス
	struct ViewFrustum {
		enum {
			LEFT,
			RIGHT,
			BOTTOM,
			TOP,
			NEARCLIP,
			FARCLIP,
			PLANE_NUM
		};
		Plane planes[PLANE_NUM];
		
		/// コンストラクタ
		ViewFrustum() {}

		ViewFrustum(const Matrix &wvp_mat)
		{
			set(wvp_mat);
		}
		
		void setRH(const Matrix &wvp_mat)
		{
			// Left clipping plane
			planes[LEFT].a = wvp_mat._41 + wvp_mat._11;
			planes[LEFT].b = wvp_mat._42 + wvp_mat._12;
			planes[LEFT].c = wvp_mat._43 + wvp_mat._13;
			planes[LEFT].d = wvp_mat._44 + wvp_mat._14;
			// Right clipping plane
			planes[RIGHT].a = wvp_mat._41 - wvp_mat._11;
			planes[RIGHT].b = wvp_mat._42 - wvp_mat._12;
			planes[RIGHT].c = wvp_mat._43 - wvp_mat._13;
			planes[RIGHT].d = wvp_mat._44 - wvp_mat._14;
			// Top clipping plane
			planes[TOP].a = wvp_mat._41 - wvp_mat._21;
			planes[TOP].b = wvp_mat._42 - wvp_mat._22;
			planes[TOP].c = wvp_mat._43 - wvp_mat._23;
			planes[TOP].d = wvp_mat._44 - wvp_mat._24;
			// Bottom clipping plane
			planes[BOTTOM].a = wvp_mat._41 + wvp_mat._21;
			planes[BOTTOM].b = wvp_mat._42 + wvp_mat._22;
			planes[BOTTOM].c = wvp_mat._43 + wvp_mat._23;
			planes[BOTTOM].d = wvp_mat._44 + wvp_mat._24;
			// Near clipping plane
			planes[NEARCLIP].a = wvp_mat._41 + wvp_mat._31;
			planes[NEARCLIP].b = wvp_mat._42 + wvp_mat._32;
			planes[NEARCLIP].c = wvp_mat._43 + wvp_mat._33;
			planes[NEARCLIP].d = wvp_mat._44 + wvp_mat._34;
			// Far clipping plane
			planes[FARCLIP].a = wvp_mat._41 - wvp_mat._31;
			planes[FARCLIP].b = wvp_mat._42 - wvp_mat._32;
			planes[FARCLIP].c = wvp_mat._43 - wvp_mat._33;
			planes[FARCLIP].d = wvp_mat._44 - wvp_mat._34;

			for(int i = 0; i < ViewFrustum::PLANE_NUM; i++) {
				planes[i].normalize();
			}
		}

		void setLH(const Matrix &wvp_mat)
		{
			// Left clipping plane
			planes[LEFT].a = wvp_mat._41 + wvp_mat._11;
			planes[LEFT].b = wvp_mat._42 + wvp_mat._12;
			planes[LEFT].c = wvp_mat._43 + wvp_mat._13;
			planes[LEFT].d = wvp_mat._44 + wvp_mat._14;
			// Right clipping plane
			planes[RIGHT].a = wvp_mat._41 - wvp_mat._11;
			planes[RIGHT].b = wvp_mat._42 - wvp_mat._12;
			planes[RIGHT].c = wvp_mat._43 - wvp_mat._13;
			planes[RIGHT].d = wvp_mat._44 - wvp_mat._14;
			// Top clipping plane
			planes[TOP].a = wvp_mat._41 - wvp_mat._21;
			planes[TOP].b = wvp_mat._42 - wvp_mat._22;
			planes[TOP].c = wvp_mat._43 - wvp_mat._23;
			planes[TOP].d = wvp_mat._44 - wvp_mat._24;
			// Bottom clipping plane
			planes[BOTTOM].a = wvp_mat._41 + wvp_mat._21;
			planes[BOTTOM].b = wvp_mat._42 + wvp_mat._22;
			planes[BOTTOM].c = wvp_mat._43 + wvp_mat._23;
			planes[BOTTOM].d = wvp_mat._44 + wvp_mat._24;
			// Near clipping plane
			planes[NEARCLIP].a = wvp_mat._31;
			planes[NEARCLIP].b = wvp_mat._32;
			planes[NEARCLIP].c = wvp_mat._33;
			planes[NEARCLIP].d = wvp_mat._34;
			// Far clipping plane
			planes[FARCLIP].a = wvp_mat._41 - wvp_mat._31;
			planes[FARCLIP].b = wvp_mat._42 - wvp_mat._32;
			planes[FARCLIP].c = wvp_mat._43 - wvp_mat._33;
			planes[FARCLIP].d = wvp_mat._44 - wvp_mat._34;

			for(int i = 0; i < ViewFrustum::PLANE_NUM; i++) {
				planes[i].normalize();
			}
		}

		void set(const Matrix &wvp_mat)
		{
#ifdef GCTP_COORD_RH
			setRH(wvp_mat);
#else
			setLH(wvp_mat);
#endif
		}

		bool isColliding(const Sphere &to) const
		{
			for(int i = 0; i < ViewFrustum::PLANE_NUM; i++) {
				gctp::Real dot = planes[i] * to.c;
				if(dot < -to.r) return false; // 外側
				if(abs(dot) <= to.r) return true; // 交差
			}
			return true; // 錘台の内側
		}

	};


} //namespace gctp

#endif //_GCTP_VIEWFRUSTUM_HPP_
