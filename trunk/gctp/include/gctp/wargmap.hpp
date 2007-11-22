#ifndef _GCTP_WARGMAP_HPP_
#define _GCTP_WARGMAP_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file argmap.hpp
 * ArgMapクラス
 * 引数読み取り・保持クラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/12/30
 * $Id: argmap.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <iosfwd>
#include <map>
#include <vector>
#include <string>

namespace gctp {

	/** コマンドライン引数クラス
	 *
	 * addHaveValKeyで値を持つオプションを指定した後、parseを実行。
	 * オプションをキーとしたmapが出来上がる。値はlist<string>。ArgMap::ValListという別名が
	 * つけられている。
	@code
	WArgMap arg;
	arg.addHaveValKey(L"output");
	arg.addHaveValKey(L"include");
	arg.defAlias(L'o', L"output");
	arg.defAlias(L'i', L"include");
	arg.parse(argc, argv);
		foo -o output_path -i include_path input_file1 "input file2"
		#=>	"output" => {"output_path"}
			"include" => {"include_path"}
			"" => {"input_file1", "input file2"}
	@endcode
	 * '"'の解釈はOSの方でやってくれる。\n
	 *	: 読み込んだ引数を表示するサンプル
	@code
	WArgMap arg;
	arg.parse(argc, argv);
	for(WArgMap::Itr i = arg.begin(); i != arg.end(); i++) {
		wcout << L"KEY:" << i->first << L" VAL:";
		for(ArgMap::ValListItr j = i->second.begin(); j != i->second.end(); j++) {
			wcout << *j << L" ";
		}
		wcout << endl;
	}
	@endcode
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/09 20:43:33
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class WArgMap : public std::map<std::wstring, std::vector<std::wstring> >
	{
	public:
		typedef iterator Itr;
		typedef const_iterator ConItr;
		typedef std::auto_ptr<WArgMap> Ptr;
		typedef std::vector<std::wstring> ValList;
		typedef std::vector<std::wstring>::iterator ValListItr;
		typedef std::vector<std::wstring>::const_iterator ValListConItr;
		bool hasKey(const std::wstring &key) const;
		void add(const std::wstring &key, const std::wstring &val);
		void addHasValKey(const std::wstring &key);
		void defAlias(const wchar_t opt, const std::wstring &key);
		bool parse(int argc, wchar_t *argv[]);
		bool parse(const wchar_t *cmdline);
		bool parse(std::wistream &in);
		void clear(void);
	private:
		typedef std::map<std::wstring, ValList> Super;
		typedef std::vector<std::wstring> HasValKeyList;
		HasValKeyList have_val_key_;
		std::map<wchar_t, std::wstring> alias_;
		bool doseKeyHasVal(const std::wstring &key) const;
		bool hasAlias(const wchar_t opt) const;
		bool checkOpt(const wchar_t *arg, std::wstring &cur_key);
		static std::wistream &tokenize(std::wistream &lhs, std::wstring &rhs);
	};

} // namespace gctp

#endif //_GCTP_WARGMAP_HPP_