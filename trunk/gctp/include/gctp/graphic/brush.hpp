#ifndef _GCTP_GRAPHIC_BRUSH_HPP_
#define _GCTP_GRAPHIC_BRUSH_HPP_
/** @file
 * GameCatapult �V�F�[�_�[�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:43:53
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/class.hpp>

namespace gctp { namespace graphic {

	TYPEDEF_DXCOMPTR(ID3DXEffect);

	/// �V�F�[�_�[���\�[�X�N���X
	class Brush : public Rsrc {
	public:
		HRslt setUp(const _TCHAR *fname);

		HRslt restore();
		void cleanUp();

		ID3DXEffect *operator->() { return ptr_; }
		const ID3DXEffect *operator->() const { return ptr_; }
		operator ID3DXEffect *() { return ptr_; }
		operator const ID3DXEffect *() const { return ptr_; }
		ID3DXEffect *get() const { return ptr_; }

		/// �K�p�J�n
		HRslt begin(uint &passnum) const;
		/// �K�p�I��
		HRslt end() const;
		/// �w��̃p�X��K�p�J�n
		HRslt beginPass(uint passno) const;
		/// �w��̃p�X��K�p�I��
		HRslt endPass() const;

	GCTP_DECLARE_CLASS

	protected:
		ID3DXEffectPtr ptr_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_BRUSH_HPP_