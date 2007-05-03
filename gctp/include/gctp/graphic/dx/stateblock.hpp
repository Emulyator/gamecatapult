#ifndef _GCTP_GRAPHIC_DX_STATEBLOCK_HPP_
#define _GCTP_GRAPHIC_DX_STATEBLOCK_HPP_
/** @file
 * GameCatapult DirectX Graphic�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: stateblock.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/types.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/pointer.hpp>
#include <gctp/graphic/rsrc.hpp>

namespace gctp { namespace graphic { namespace dx {

	/** DirectGraphic �X�e�[�g�u���b�N
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/25 21:40:09
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class StateBlock : public Object {
	public:
		StateBlock() {}
		StateBlock(IDirect3DDevicePtr dev, D3DSTATEBLOCKTYPE type = D3DSBT_ALL) { setUp(dev, type); }
		StateBlock(IDirect3DStateBlockPtr sb, D3DSTATEBLOCKTYPE type) { setUp(sb, type); }
		/// �X�e�[�g�u���b�N�̃n���h����Ԃ�
		IDirect3DStateBlockPtr get() const { return ptr_; }
		/// �X�e�[�g�u���b�N�̃n���h����Ԃ�
		IDirect3DStateBlockPtr &get() { return ptr_; }
		
		HRslt setUp(IDirect3DDevicePtr dev, D3DSTATEBLOCKTYPE type = D3DSBT_ALL);
		HRslt setUp(IDirect3DStateBlockPtr sb, D3DSTATEBLOCKTYPE type);
		void tearDown();
		HRslt apply() const;
		HRslt capture();
		D3DSTATEBLOCKTYPE type() { return type_; } 

		bool operator!() { return ptr_ && ptr_ != (IDirect3DStateBlock9 *)-1 ? false : true; }
		operator bool() { return ptr_ && ptr_ != (IDirect3DStateBlock9 *)-1 ? true : false; }

	private:
		D3DSTATEBLOCKTYPE type_;
		IDirect3DStateBlockPtr ptr_;
	};

	/** �X�e�[�g�u���b�N���\�[�X
	 *
	 * record���I�[�o�[���C�h���Ďg���B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 8:07:00
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class StateBlockRsrc : public Rsrc {
	public:
		/// ����
		HRslt setUp();
		/// ����(���\�[�X�p)
		HRslt setUp(const _TCHAR *name) { return setUp(); }
		// �K�p
		HRslt setCurrent() const;
		// ���ɖ߂�
		HRslt unSet() const;

		virtual void record() = 0;

		virtual HRslt restore();
		virtual void cleanUp();
		
	private:
		Pointer<StateBlock> sb_;
		mutable Pointer<StateBlock> sbb_;
	};

}}} // namespace gctp

#endif //_GCTP_GRAPHIC_DX_STATEBLOCK_HPP_