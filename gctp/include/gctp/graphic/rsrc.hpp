#ifndef _GCTP_GRAPHIC_RSRC_HPP_
#define _GCTP_GRAPHIC_RSRC_HPP_
/** @file
 * GameCatapult グラフィックリソース基底クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: rsrc.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/object.hpp>
#include <gctp/class.hpp>
#include <gctp/hrslt.hpp>

namespace gctp { namespace graphic {

	namespace dx {
		TYPEDEF_DXCOMPTR(IDirect3D9);
		TYPEDEF_DXCOMPTR(IDirect3DDevice9);
		TYPEDEF_DXCOMPTR(IDirect3DSwapChain9);
		TYPEDEF_DXCOMPTR(IDirect3DStateBlock9);

		TYPEDEF_DXCOMPTR(IDirect3DBaseTexture9);
		TYPEDEF_DXCOMPTR(IDirect3DTexture9);
		TYPEDEF_DXCOMPTR(IDirect3DVertexBuffer9);
		TYPEDEF_DXCOMPTR(IDirect3DIndexBuffer9);
		TYPEDEF_DXCOMPTR(IDirect3DSurface9);
	}

	/** グラフィックデバイス自己管理リソース基本クラス
	 *
	 * デバイス消失時にcleanUp/restoreが必要なリソースは、これを継承し、
	 * デバイス消失時のリセット前の処理（リソース解放）をcleanUpに
	 * リセット後の処理（リソース再構築）をrestoreに記述する。
	 *
	 * 全てがDirect3DのMANAGEDであれば、これを継承せずただのObjectでよい。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/28 13:09:36
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Rsrc : public Object {
	public:
		Rsrc();
		virtual void cleanUp();
		virtual HRslt restore();

	GCTP_DECLARE_CLASS
	};

}} // namespace gctp

#endif //_GCTP_GRAPHIC_RSRC_HPP_