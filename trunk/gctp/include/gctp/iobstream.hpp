#ifndef _GCTP_IOBSTREAM_HPP_
#define _GCTP_IOBSTREAM_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult�o�C�i���X�g���[�����o�̓t�H�[�}�b�g
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 23:22:59
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/ibstream.hpp>
#include <gctp/obstream.hpp>

namespace gctp {

	/** �o�C�i���X�g���[�����o�̓t�H�[�}�b�g�N���X
	 *
	 * ostream�ȂǂƔ�ׂĕ�����̈������Ⴄ�B�ǂݎ�莞�͉��s�܂łł͂Ȃ��k�������܂ŁA
	 * �������ݎ��̓k����������������(ostream�̓k�������͏����Ȃ�)�B
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/04 23:23:21
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class iobstream : public ibstream, public obstream
	{
	public:
		explicit iobstream(std::streambuf *__buf) : ibstream(__buf), obstream(__buf) {}
		virtual ~iobstream() {}
	};

} // namespace gctp

#endif //_GCTP_IOBSTREAM_HPP_