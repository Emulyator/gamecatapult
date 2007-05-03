/** @file
 * GameCatapult 文字列をキーとするマップの共通実装
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/wstringmap.hpp>

using namespace std;

namespace gctp {

	void WStaticStringMapBase::put(const wchar_t *key, const void *val)
	{
		(*this)[key] = const_cast<void *>(val);
	}

	void *WStaticStringMapBase::get(const wchar_t *key) const
	{
		const_iterator i = find(key);
		if(i != end()) return i->second;
		return 0;
	}

	void *&WStringMapBase::operator[](const WCStr key)
	{
		return static_cast<BaseType &>(*this)[key];
	}

	void *&WStringMapBase::operator[](const wchar_t *key)
	{
		iterator i = find(TempWCStr(key));
		if(i != end()) return i->second;
		else return static_cast<BaseType &>(*this)[key];
	}

	void WStringMapBase::put(const wchar_t *key, const void *val)
	{
		iterator i = find(TempWCStr(key));
		if(i != end()) i->second = const_cast<void *>(val);
		else static_cast<BaseType &>(*this)[key] = const_cast<void *>(val);
	}

	void WStringMapBase::put(const WCStr key, const void *val)
	{
		(*this)[key] = const_cast<void *>(val);
	}

	void *WStringMapBase::get(const wchar_t *key) const
	{
		const_iterator i = find(TempWCStr(key));
		if(i != end()) return i->second;
		return 0;
	}

	void *WStringMapBase::get(const WCStr key) const
	{
		const_iterator i = find(key);
		if(i != end()) return i->second;
		return 0;
	}

} // namespace gctp
