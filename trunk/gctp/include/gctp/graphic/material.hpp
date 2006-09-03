#ifndef _GCTP_MATERIAL_HPP_
#define _GCTP_MATERIAL_HPP_
/** @file
 * GameCatapult �ގ����N���X�w�b�_�t�@�C��
 *
 * �v����Ƀ}�e���A���B
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: material.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/pointer.hpp>
#include <gctp/color.hpp>

namespace gctp { namespace graphic {

	class Texture;
	/** �}�e���A�����
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/12/08 2:29:24
	 */
	struct Material
	{
		enum BlendMode {
			NONE,
			ALPHA,
			ADD,
			SUB,
			MUL,
		};
		Color diffuse;
		Color ambient;
		Color specular;
		Color emissive;
		float power;
		BlendMode blend;
		Handle<Texture> tex;
		Handle<Texture> tex1;
		Handle<Texture> tex2;
//		Handle<Blush> blush;

		/// �W���I�ȏ����l��^����
		void setUp();
	};

#if 0
	class SortPacket
	{
	public:
		StateBlock	sb_;
		bool		is_transparent_;	// ����������H
		float		z_order_;			// Z�l�i�������̂̎��A�L���j
		bool operator==(const SortPacket &rhs) {
			if( sb_.get() == rhs.sb_.get()
				&& is_transparent_ == rhs.is_transparent_
				&& z_order_ == rhs.z_order_) return true;
			else return false;
		}
		bool operator<(const SortPacket &rhs) {
			if(is_transparent_ != rhs.is_transparent_){
				return is_transparent_;
			}
			else {
				if(is_transparent_) return z_order_ < rhs.z_order_;
				else return sb_.get() < rhs.sb_.get();
			}
		}
	};
#endif
}} //namespace gctp

#endif //_GCTP_MATERIAL_HPP_