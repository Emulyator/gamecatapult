/** @file
 * GameCatapult バッファクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/12/30
 * $Id: binary.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/buffer.hpp>
#include <gctp/serializer.hpp>

using namespace std;

namespace gctp {

	GCTP_IMPLEMENT_CLASS_NS(gctp, Buffer, Object);

	void Buffer::serialize(Serializer &serializer)
	{
		if(serializer.isLoading()) {
			uint size;
			serializer.istream() >> size;
			resize(size);
			serializer.istream().read(buf(), size);
		}
		else if(serializer.isWriting()) {
			uint size = static_cast<uint>(this->size());
			serializer.ostream() << size;
			serializer.ostream().write(buf(), size);
		}
	}

} // namespace gctp
