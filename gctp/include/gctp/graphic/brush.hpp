#ifndef _GCTP_GRAPHIC_BRUSH_HPP_
#define _GCTP_GRAPHIC_BRUSH_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �`�揈���N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:43:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic.hpp>
#include <gctp/class.hpp>
#include <gctp/graphic/model.hpp>

namespace gctp { namespace graphic {

	/// �`���i�N���X
	class Brush : public Object {
	public:
		Brush(Model &target) : target_(target) {}
		virtual ~Brush() {}
		virtual HRslt begin(Handle<Shader> shader, const Skeleton &skl) const
		{
			return S_OK;
		}
		virtual HRslt end() const
		{
			return S_OK;
		}
		virtual HRslt draw(uint subset_no, const Skeleton &skl) const = 0;
		//virtual HRslt draw(uint subset_no, const Matrix &mat) const = 0;
		/// ���b�V���f�Џ��
		virtual const std::vector<SubsetInfo> &subsets() const { return target_.subsets(); }
		Model &target_;
	};

}} //namespace gctp

#endif // _GCTP_GRAPHIC_BRUSH_HPP_