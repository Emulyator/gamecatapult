/** @file
 * GameCatapult çﬁéøèÓïÒÉNÉâÉX
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: material.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/graphic/material.hpp>

using namespace std;

namespace gctp { namespace graphic {

	void Material::setUp()
	{
		diffuse = Color(1.0f,1.0f,1.0f);
		ambient = Color(0.2f,0.2f,0.2f);
		specular = emissive = Color(0.0f,0.0f,0.0f);
		power = 1.0f;
		blend = ALPHA;
	}

}} // namespace gctp
