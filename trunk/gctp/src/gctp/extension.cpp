/** @file
 * GameCatapult ���\�[�X���A���C�U�o�^�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/extension.hpp>
#include <gctp/tstringmap.hpp>
#include <gctp/turi.hpp>

using namespace std;

namespace gctp {

	///////////////////
	// Extension
	//

	namespace {
		typedef TStaticStringMap<RealizeMethod> RealizerMap;
		// RealizerMap�̃V���O���g����Ԃ�
		RealizerMap &realizers()
		{
			static RealizerMap _realizers_;
			return _realizers_;
		}
	}

	/** �g���q���̃��A���C�U�[��o�^
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 12:04:48
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Extension::Extension(const _TCHAR *extension, const RealizeMethod f) : ext_(extension)
	{
		realizers().put(extension, f);
	}

	/** �o�^�𖕏�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 12:04:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Extension::~Extension()
	{
		realizers().erase(ext_);
	}

	/** ���A���C�U���擾
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:37:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	RealizeMethod Extension::get(const _TCHAR *extension)
	{
		RealizeMethod ret = realizers().get(extension);
		if(!ret) {
			std::basic_string<_TCHAR> ext = TURI(extension).extension();
			while(ext != _T("")) {
				ret = realizers().get(ext.c_str());
				if(ret) break;
				ext = TURI(ext).extension();
			}
		}
		return ret;
	}

} // namespace gctp
