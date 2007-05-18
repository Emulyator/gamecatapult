#ifndef _GCTP_BUFFWD_HPP_
#define _GCTP_BUFFWD_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult バッファクラスの先行宣言
 *
 *
 * BufferPtr,BufferHndl,を宣言する
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:28:35
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointer.hpp>

namespace gctp {

	class Buffer;

	/// Bufferへのポインター（シノニム）
	typedef Pointer<Buffer> BufferPtr;
	/// Bufferへのハンドル（シノニム）
	typedef Handle<Buffer> BufferHndl;

} // namespace gctp

#endif //_GCTP_BUFFWD_HPP_