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
#ifdef GCTP_LITE
# include <d3d8.h>
# include <d3d8types.h>
#else
# include <d3d9.h>
# include <d3d9types.h>
#endif

namespace gctp { namespace graphic {

	namespace dx {
#ifdef GCTP_LITE
		typedef IDirect3DDevice8 IDirect3DDevice;
		typedef IDirect3DSurface8 IDirect3DSurface;
		typedef IDirect3DTexture8 IDirect3DTexture;
		typedef IDirect3DBaseTexture8 IDirect3DBaseTexture;
		typedef IDirect3DVertexBuffer8 IDirect3DVertexBuffer;
		typedef IDirect3DIndexBuffer8 IDirect3DIndexBuffer;

		typedef D3DCAPS8 D3DCAPS;
		typedef D3DLIGHT8 D3DLIGHT;
		typedef D3DVIEWPORT8 D3DVIEWPORT;
		typedef D3DADAPTER_IDENTIFIER8 D3DADAPTER_IDENTIFIER;
		typedef D3DMATERIAL8 D3DMATERIAL;

		TYPEDEF_DXCOMPTREX(IDirect3D8, IDirect3DPtr);
		TYPEDEF_DXCOMPTREX(IDirect3DDevice8, IDirect3DDevicePtr);
		TYPEDEF_DXCOMPTREX(IDirect3DSwapChain8, IDirect3DSwapChainPtr);
		
		TYPEDEF_DXCOMPTREX(IDirect3DBaseTexture8, IDirect3DBaseTexturePtr);
		TYPEDEF_DXCOMPTREX(IDirect3DTexture8, IDirect3DTexturePtr);
		TYPEDEF_DXCOMPTREX(IDirect3DVertexBuffer8, IDirect3DVertexBufferPtr);
		TYPEDEF_DXCOMPTREX(IDirect3DIndexBuffer8, IDirect3DIndexBufferPtr);
		TYPEDEF_DXCOMPTREX(IDirect3DSurface8, IDirect3DSurfacePtr);
#else
		typedef IDirect3DDevice9 IDirect3DDevice;
		typedef IDirect3DSurface9 IDirect3DSurface;
		typedef IDirect3DTexture9 IDirect3DTexture;
		typedef IDirect3DBaseTexture9 IDirect3DBaseTexture;
		typedef IDirect3DStateBlock9 IDirect3DStateBlock;
		typedef IDirect3DVertexBuffer9 IDirect3DVertexBuffer;
		typedef IDirect3DIndexBuffer9 IDirect3DIndexBuffer;

		typedef D3DCAPS9 D3DCAPS;
		typedef D3DLIGHT9 D3DLIGHT;
		typedef D3DVIEWPORT9 D3DVIEWPORT;
		typedef D3DADAPTER_IDENTIFIER9 D3DADAPTER_IDENTIFIER;
		typedef D3DMATERIAL9 D3DMATERIAL;
		typedef D3DVERTEXELEMENT9 D3DVERTEXELEMENT;

		TYPEDEF_DXCOMPTREX(IDirect3D9, IDirect3DPtr);
		TYPEDEF_DXCOMPTREX(IDirect3DDevice9, IDirect3DDevicePtr);
		TYPEDEF_DXCOMPTREX(IDirect3DSwapChain9, IDirect3DSwapChainPtr);
		TYPEDEF_DXCOMPTREX(IDirect3DStateBlock9, IDirect3DStateBlockPtr);

		TYPEDEF_DXCOMPTREX(IDirect3DBaseTexture9, IDirect3DBaseTexturePtr);
		TYPEDEF_DXCOMPTREX(IDirect3DTexture9, IDirect3DTexturePtr);
		TYPEDEF_DXCOMPTREX(IDirect3DVertexBuffer9, IDirect3DVertexBufferPtr);
		TYPEDEF_DXCOMPTREX(IDirect3DIndexBuffer9, IDirect3DIndexBufferPtr);
		TYPEDEF_DXCOMPTREX(IDirect3DSurface9, IDirect3DSurfacePtr);

		TYPEDEF_DXCOMPTREX(IDirect3DVertexShader9, IDirect3DVertexShaderPtr);
#endif
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