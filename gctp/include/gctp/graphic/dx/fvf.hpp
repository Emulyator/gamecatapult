#ifndef _GCTP_GRAPHIC_DX_FVF_HPP_
#define _GCTP_GRAPHIC_DX_FVF_HPP_
/** @file
 * GameCatapult 頂点バッファクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 3:16:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/def.hpp>

namespace gctp { namespace graphic { namespace dx {

	/** DirectX頂点フォーマット
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 4:15:56
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct FVF {
		ulong val;
		FVF() {}
		FVF(ulong _val) : val(_val) {}
		ulong stride() const { return D3DXGetFVFVertexSize(val); }
	};

}}} //namespace gctp

#endif //_GCTP_GRAPHIC_DX_FVF_HPP_