#ifndef _GCTP_DXCOMPTRS_HPP_
#define _GCTP_DXCOMPTRS_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file
 * ���W���[�����܂������Ďg�p�����ėp�I��COM�|�C���^�[�͂����ɂ܂Ƃ߂Đ錾
 *
 * �Ƃ����Ă�ID3DXBufferPtr����
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/21 15:53:39
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/com_ptr.hpp>

namespace gctp {

	TYPEDEF_DXCOMPTR(ID3DXBuffer);

} // namespace gctp

#endif //_GCTP_DXCOMPTRS_HPP_