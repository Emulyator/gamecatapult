/** @file
 * GameCatapult グラフィックリソース基底クラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: rsrc.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/dx/stateblock.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace graphic {
	
	GCTP_IMPLEMENT_CLASS_NS(gctp, Rsrc, Object);

	Rsrc::Rsrc()
	{
		if(!device().isOpen()) {
			GCTP_TRACE_MSGBOX("graphic::Deviceがセットアップされていないか、\n"
				"カレントデバイスが設定されていない状態でデバイス管理リソースが製作されています。\n"
				"静的オブジェクトとしてデバイス管理リソースを持ちたい場合は、\n"
				"ポインターにするなどして、デバイスのオープン後に製作されるようにしてください。");
		}
		else device().registerRsrc(Handle<Rsrc>(this));
	}

	void Rsrc::cleanUp()
	{
	}

	HRslt Rsrc::restore()
	{
		return S_OK;
	}

}} // namespace gctp
