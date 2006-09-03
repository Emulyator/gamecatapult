/** @file
 * GameCatapult 文字列をキーとするマップの共通実装
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/stringmap.hpp>

using namespace std;

namespace gctp {

	void StaticStringMapBase::put(const char *key, const void *val)
	{
		(*this)[key] = const_cast<void *>(val);
	}

	void *StaticStringMapBase::get(const char *key) const
	{
		const_iterator i = find(key);
		if(i != end()) return i->second;
		return 0;
	}

	void *&StringMapBase::operator[](const CStr key)
	{
		return static_cast<BaseType &>(*this)[key];
	}

	void *&StringMapBase::operator[](const char *key)
	{
		iterator i = find(TempCStr(key));
		if(i != end()) return i->second;
		else return static_cast<BaseType &>(*this)[key];
	}

	void StringMapBase::put(const char *key, const void *val)
	{
		iterator i = find(TempCStr(key));
		if(i != end()) i->second = const_cast<void *>(val);
		else static_cast<BaseType &>(*this)[key] = const_cast<void *>(val);
	}

	void StringMapBase::put(const CStr key, const void *val)
	{
		(*this)[key] = const_cast<void *>(val);
	}

	void *StringMapBase::get(const char *key) const
	{
		const_iterator i = find(TempCStr(key));
		if(i != end()) return i->second;
		return 0;
	}

	void *StringMapBase::get(const CStr key) const
	{
		const_iterator i = find(key);
		if(i != end()) return i->second;
		return 0;
	}

} // namespace gctp
