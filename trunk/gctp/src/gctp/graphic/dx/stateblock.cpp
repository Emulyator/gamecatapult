/**@file
 * GameCatapult DirectX ステートブロッククラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/25 21:48:07
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/graphic/dx/stateblock.hpp>
#include <gctp/graphic.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic { namespace dx {

	//////////////////////
	// StateBlock
	//
	HRslt StateBlock::setUp(IDirect3DDevicePtr dev, D3DSTATEBLOCKTYPE type)
	{
		type_ = type;
		return dev->CreateStateBlock(type, &ptr_);
	}

	HRslt StateBlock::setUp(IDirect3DStateBlockPtr sb, D3DSTATEBLOCKTYPE type)
	{
		type_ = type;
		ptr_ = sb;
		return S_OK;
	}

	void StateBlock::tearDown()
	{
		ptr_ = 0;
	}

	HRslt StateBlock::apply() const
	{
		if(!this || !ptr_ || ptr_ == (IDirect3DStateBlock *)-1) return E_FAIL;
		return ptr_->Apply();
	}

	HRslt StateBlock::capture()
	{
		if(!this || !ptr_ || ptr_ == (IDirect3DStateBlock *)-1) return CO_E_NOTINITIALIZED;
		return ptr_->Capture();
	}

	///////////////////////
	// StateBlockRsrc
	//
	HRslt StateBlockRsrc::setUp()
	{
		beginRecord();
		record();
		sb_ = endRecord();
		beginRecord();
		record();
		sbb_ = endRecord();
		return S_OK;
	}

	HRslt StateBlockRsrc::setCurrent() const
	{
		sbb_->capture();
		return sb_->apply();
	}

	HRslt StateBlockRsrc::unset() const
	{
		return sbb_->apply();
	}

	HRslt StateBlockRsrc::restore()
	{
		return setUp();
	}

	void StateBlockRsrc::cleanUp()
	{
		sb_ = 0;
		sbb_ = 0;
	}

}}} // namespace gctp
