/** @file
 * GameCatapult リソースリアライザ登録クラス
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
		// RealizerMapのシングルトンを返す
		RealizerMap &realizers()
		{
			static RealizerMap _realizers_;
			return _realizers_;
		}
	}

	/** 拡張子毎のリアライザーを登録
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 12:04:48
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Extension::Extension(const _TCHAR *extension, const RealizeMethod f) : ext_(extension)
	{
		realizers().put(extension, f);
	}

	/** 登録を抹消
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 12:04:59
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Extension::~Extension()
	{
		realizers().erase(ext_);
	}

	/** リアライザを取得
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
