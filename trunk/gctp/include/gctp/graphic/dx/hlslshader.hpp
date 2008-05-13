#ifndef _GCTP_GRAPHIC_DX_HLSLSHADER_HPP_
#define _GCTP_GRAPHIC_DX_HLSLSHADER_HPP_
/** @file
 * GameCatapult シェーダークラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:43:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/buffwd.hpp> // for BufferPtr
#include <gctp/graphic.hpp>
#include <gctp/class.hpp>
#include <gctp/graphic/shader.hpp>

namespace gctp { namespace graphic { namespace dx {

	TYPEDEF_DXCOMPTR(ID3DXEffect);

	/// シェーダーリソースクラス
	class HLSLShader : public Shader {
	public:
		virtual HRslt setUp(const _TCHAR *fname);
		HRslt setUp(BufferPtr buffer);

		virtual HRslt restore();
		virtual void cleanUp();

		ID3DXEffect *operator->() { return ptr_; }
		const ID3DXEffect *operator->() const { return ptr_; }
		operator ID3DXEffect *() { return ptr_; }
		operator const ID3DXEffect *() const { return ptr_; }
		ID3DXEffect *get() const { return ptr_; }

		/// 適用開始
		virtual HRslt begin() const;
		/// 適用終了
		virtual HRslt end() const;
		/// 指定のパスを適用開始
		virtual HRslt beginPass(uint passno) const;
		/// 指定のパスを適用終了
		virtual HRslt endPass() const;
		/// パス数を返す（begin~endの間のみ有効）
		virtual uint passnum() const { return passnum_; }

	GCTP_DECLARE_CLASS

	protected:
		mutable uint passnum_;
		ID3DXEffectPtr ptr_;
	};

}}} //namespace gctp

#endif // _GCTP_GRAPHIC_DX_HLSLSHADER_HPP_