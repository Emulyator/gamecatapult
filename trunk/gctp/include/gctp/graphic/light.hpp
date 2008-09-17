#ifndef _GCTP_GRAPHIC_LIGHT_HPP_
#define _GCTP_GRAPHIC_LIGHT_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 光源クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/12 18:39:23
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */

#include <gctp/color.hpp>
#include <gctp/vector.hpp>

namespace gctp { namespace graphic {

	/** 平行光源クラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/12 18:39:29
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct DirectionalLight {
		Color  ambient;
		Color  diffuse;
		Color  specular;
		Vector dir;
	};

	/** 点光源クラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/12 18:39:29
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct PointLight {
		Color  ambient;
		Color  diffuse;
		Color  specular;
		Vector pos;
		float  range;
		float  attenuation[3];
	};

	/** スポットライトクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/12 18:39:29
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct SpotLight {
		Color  ambient;
		Color  diffuse;
		Color  specular;
		Vector pos;
		Vector dir;
		float  range;
		float  attenuation[3];
		float  falloff;
		float  theta;
		float  phi;
	};

}} // namespace gctp

#endif //_GCTP_GRAPHIC_LIGHT_HPP_