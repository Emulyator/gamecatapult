#ifndef _GCTP_GRAPHIC_DX_HLSLSHADER_HPP_
#define _GCTP_GRAPHIC_DX_HLSLSHADER_HPP_
/** @file
 * GameCatapult �V�F�[�_�[�N���X�w�b�_�t�@�C��
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

	/// �V�F�[�_�[���\�[�X�N���X
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

		/// �K�p�J�n
		virtual HRslt begin() const;
		/// �K�p�I��
		virtual HRslt end() const;
		/// �w��̃p�X��K�p�J�n
		virtual HRslt beginPass(uint passno) const;
		/// �w��̃p�X��K�p�I��
		virtual HRslt endPass() const;
		/// �p�X����Ԃ��ibegin~end�̊Ԃ̂ݗL���j
		virtual uint passnum() const { return passnum_; }

	GCTP_DECLARE_CLASS

	protected:
		mutable uint passnum_;
		ID3DXEffectPtr ptr_;
	};

}}} //namespace gctp

#endif // _GCTP_GRAPHIC_DX_HLSLSHADER_HPP_