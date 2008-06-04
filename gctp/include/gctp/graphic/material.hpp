#ifndef _GCTP_MATERIAL_HPP_
#define _GCTP_MATERIAL_HPP_
/** @file
 * GameCatapult 材質情報クラスヘッダファイル
 *
 * 要するにマテリアル。
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
	class Shader;

	/** マテリアル情報
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/12/08 2:29:24
	 */
	struct Material
	{
		enum BlendMode {
			OPEQUE,		// 不透明（パンチスルー含む）
			TRANSLUCENT, // アルファブレンド
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
		bool double_side;

		Handle<Texture> tex;
		Handle<Texture> tex1;
		Handle<Texture> tex2;

		Handle<Shader> shader;
		
		/// 標準的な初期値を与える
		void setUp();

		// 代入演算子
		Material &operator+=(const Material &src) {
			diffuse += src.diffuse;
			ambient += src.ambient;
			specular += src.specular;
			emissive += src.emissive;
			power += src.power;
			return *this;
		}
		// 代入演算子
		Material &operator*=(Real rhs) {
			diffuse *= rhs;
			ambient *= rhs;
			specular *= rhs;
			emissive *= rhs;
			power *= rhs;
			return *this;
		}

		Material operator+(const Material &rhs) const {
			return Material(*this)+=rhs;
		}

		Material operator*(Real rhs) const {
			return Material(*this)*=rhs;
		}
	};

}} //namespace gctp

#endif //_GCTP_MATERIAL_HPP_