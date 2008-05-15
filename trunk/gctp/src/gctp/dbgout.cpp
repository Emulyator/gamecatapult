/** @file
 * GameCatapult �f�o�b�O�o�͎x���}�N���E�֐�
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:09:55
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/dbgout.hpp>
#include <gctp/dbgoutbuf.hpp>
#include <mbctype.h>

using namespace std;

namespace gctp {

	static debuggeroutbuf<_TCHAR> _dbgout_buf;
	basic_ostream<_TCHAR> dbgout(&_dbgout_buf);		// �f�o�b�K�A�E�g�v�b�g�X�g���[��

	void insertDbgoutSyncStream(std::basic_streambuf<_TCHAR> *sync_buf)
	{
		_dbgout_buf.insertSyncBuf(sync_buf);
	}
	void eraseDbgoutSyncStream(std::basic_streambuf<_TCHAR> *sync_buf)
	{
		_dbgout_buf.eraseSyncBuf(sync_buf);
	}

	//basic_ofstream<_TCHAR> logfile(_T("log.txt"));		// ���O�t�@�C���X�g���[��
	basic_ofstream<_TCHAR> logfile;

};	// namespace gctp
