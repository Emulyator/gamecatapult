/** @file
 * GameCatapult 実行時型情報をキーとするマップの共通実装
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/typemap.hpp>

using namespace std;

namespace gctp {

	void TypeMapBase::put(const GCTP_TYPEINFO &key, const void *val)
	{
		(*this)[&key] = const_cast<void *>(val);
	}

	void *TypeMapBase::get(const GCTP_TYPEINFO &key) const
	{
		const_iterator i = find(&key);
		if(i != end()) return i->second;
		return 0;
	}

} // namespace gctp
