#ifndef _GCTP_GRAPHIC_DX_VIEW_HPP_
#define _GCTP_GRAPHIC_DX_VIEW_HPP_
/**@file
 * GameCatapult DirectX 追加スワップチェーンクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 18:42:14
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <d3d9.h>
#include <gctp/def.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/graphic/rsrc.hpp>

namespace gctp { namespace graphic { namespace dx {
	
	/** DirectGraphic 追加スワップチェーンクラスヘッダファイル
	 *
	 * 追加チェーンは、フルスクリーンを取得できない…という設計に妥協。
	 *
	 * しかし、デバイス消失時はどうなるんだ？明示的なレストアが必要なのか？
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 21:40:09
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class View : public Rsrc {
	public:
		HRslt setUp(IDirect3DSwapChain9Ptr ptr);
		HRslt setUp(IDirect3DDevice9Ptr dev, uint i);
		
		// リソースとしてのお勤め
		virtual void cleanUp();
		virtual HRslt restore();

		/// カレントの描画対象にする
		HRslt setCurrent() const;
		/// 描画反映
		HRslt present() const;

	private:
		IDirect3DSwapChain9Ptr	ptr_;
		IDirect3DDevice9Ptr		device_;
		D3DPRESENT_PARAMETERS	pp_;
		uint					idx_;
	};

}}} // namespace gctp

#endif //_GCTP_GRAPHIC_DX_VIEW_HPP_