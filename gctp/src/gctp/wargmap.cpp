/** @file
 *
 * 引数文字列の解析して、オプション=>値のマップを作る
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 20:44:40
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/wargmap.hpp>
#include <gctp/cmdline.hpp>
#include <string.h>
#include <iostream>
#include <vector>

using namespace std;

namespace gctp {

	bool WArgMap::hasKey(const wstring &key) const
	{
		return end() != find(key);
	}

	bool WArgMap::doseKeyHasVal(const wstring &key) const
	{
		for(HasValKeyList::const_iterator i = have_val_key_.begin(); i != have_val_key_.end(); i++) {
			if((*i) == key) return true;
		}
		return false;
	}

	bool WArgMap::hasAlias(const wchar_t opt) const
	{
		return alias_.end() != alias_.find(opt);
	}

	void WArgMap::add(const wstring &key, const wstring &val)
	{
		(*this)[key].push_back(val);
	}

	void WArgMap::addHasValKey(const wstring &key)
	{
		have_val_key_.push_back(key);
	}

	void WArgMap::defAlias(const wchar_t opt, const wstring &key)
	{
		alias_[opt]=key;
	}

	bool WArgMap::checkOpt(const wchar_t *arg, wstring &cur_key)
	{
		bool is_already_have_has_val_key = false;
		while(*arg) {
			wchar_t temp[] = {*arg, L'\0'};
			if(hasAlias(*arg) && doseKeyHasVal(alias_[*arg])) {
				if(is_already_have_has_val_key) return false; // 後に値が必要なものが２個以上一度に指定された
				is_already_have_has_val_key = true;
				cur_key = alias_[*arg];
			}
			else if(doseKeyHasVal(wstring(temp))) {
				if(is_already_have_has_val_key) return false; // 後に値が必要なものが２個以上一度に指定された
				is_already_have_has_val_key = true;
				cur_key = wstring(temp);
			}
			else if(hasAlias(*arg)) {
				(*this)[alias_[*arg]];
				cur_key = L"";
			}
			else {
				(*this)[wstring(temp)];
				cur_key = L"";
			}
			arg++;
		}
		return true;
	}

	void WArgMap::clear(void)
	{
		have_val_key_.clear();
		alias_.clear();
		Super::clear();
	}

	/** 引数を解析
	 *
	 * オプション=>値のマップに構築
	 *
	 * オプションに対する入力ではない引数は、ヌルストリングキーに入る。ArgMap[string("")]
	 *
	 * この中でプログラム本体のパスを読み飛ばすので、main関数の引数をそのまま渡してください。
	 *
	 * @return パーズエラーの場合false
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:48:01
	 * $Id: argmap.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	bool WArgMap::parse(int argc, wchar_t *argv[])
	{
		argc--; argv++;
		wstring cur_key;
		for(int i = 0; i < argc; i++) {
			if(argv[i][0] == L'-') {
				if(argv[i][1] == L'-') {
					if(argv[i][2] == L'-') {
						add(wstring(L""), wstring(argv[i]));
					}
					else {
						cur_key = argv[i]+2;
						if(!doseKeyHasVal(cur_key)) {
							(*this)[cur_key];
							cur_key = L"";
						}
					}
				}
				else {
					if(!checkOpt(argv[i]+1, cur_key)) return false;
				}
			}
			else {
				if(cur_key.length()) {
					add(cur_key, wstring(argv[i]));
					cur_key = L"";
				}
				else {
					add(wstring(L""), wstring(argv[i]));
				}
			}
		}
		return cur_key.length()==0;
	}

	/** 引数を解析
	 *
	 * オプション=>値のマップに構築
	 *
	 * 配列でない、単一の文字列から生成する。WinMainのCmdLine引数用。
	 *
	 * @return パーズエラーの場合false
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:48:01
	 * $Id: argmap.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	bool WArgMap::parse(const wchar_t *cmdline)
	{
		WCmdLine cmdl(cmdline);
		return parse(cmdl.argc(), cmdl.argv());
	}

	/** 引数を解析
	 *
	 * オプション=>値のマップに構築
	 *
	 * ファイル内容をコマンド引数として解釈。
	 *
	 * @return パーズエラーの場合false
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:48:01
	 * $Id: argmap.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	bool WArgMap::parse(wistream &in)
	{
		WCmdLine cmdl(in);
		return parse(cmdl.argc(), cmdl.argv());
	}

} // namespace gctp
