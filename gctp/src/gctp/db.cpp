/** @file
 * GameCatapult リソースデータベースクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: db.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/db.hpp>

using namespace std;

namespace gctp {

	/** 文字列インデックスとハンドルを関連付ける
	 *
	 * キー値は任意の文字列が使えるが、すべて大文字のものは、システムに予約されている。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:17:48
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool DB::insert(const char *key, const Hndl hndl)
	{
		if(index_.find(key) == index_.end()) {
			index_[key] = hndl;
			return true;
		}
		return false;
	}

	/** 文字列インデックスとハンドルを関連付ける
	 *
	 * 既存のエントリがあった場合、上書きする
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:17:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void DB::set(const char *key, Hndl hndl)
	{
		if(key) index_[key] = hndl;
	}

	/** 削除
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:17:42
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void DB::erase(const char *key)
	{
		index_.erase(key);
	}

	/** 検索
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:17:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Hndl DB::find(const char *key) const
	{
		if(key) {
			Index::const_iterator ret = index_.find(key);
			if(ret!=index_.end()) return ret->second;
		}
		return Hndl();
	}

	/** グローバルデータベース
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:17:35
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	DB &db()
	{
		static DB db_;
		return db_;
	}

} // namespace gctp
