#ifndef _GUTIL_ARGMAP_HPP_
#define _GUTIL_ARGMAP_HPP_
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
	ArgMap arg;
	arg.addHaveValKey("output");
	arg.addHaveValKey("include");
	arg.defAlias('o', "output");
	arg.defAlias('i', "include");
	arg.parse(argc, argv);
		foo -o output_path -i include_path input_file1 "input file2"
		#=>	"output" => {"output_path"}
			"include" => {"include_path"}
			"" => {"input_file1", "input file2"}
	@endcode
	 * '"'の解釈はOSの方でやってくれる。\n
	 *	: 読み込んだ引数を表示するサンプル
	@code
	ArgMap arg;
	arg.parse(argc, argv);
	for(ArgMap::Itr i = arg.begin(); i != arg.end(); i++) {
		cout << "KEY:" << i->first << " VAL:";
		for(ArgMap::ValListItr j = i->second.begin(); j != i->second.end(); j++) {
			cout << *j << " ";
		}
		cout << endl;
	}
	@endcode
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/09 20:43:33
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class ArgMap : public std::map<std::string, std::vector<std::string> >
	{
	public:
		typedef iterator Itr;
		typedef const_iterator ConItr;
		typedef std::auto_ptr<ArgMap> Ptr;
		typedef std::vector<std::string> ValList;
		typedef std::vector<std::string>::iterator ValListItr;
		typedef std::vector<std::string>::const_iterator ValListConItr;
		bool hasKey(const std::string &key) const;
		void add(const std::string &key, const std::string &val);
		void addHasValKey(const std::string &key);
		void defAlias(const char opt, const std::string &key);
		bool parse(int argc, char *argv[]);
		bool parse(const char *cmdline);
		bool parse(std::istream &in);
		void clear(void);
	private:
		typedef std::map<std::string, ValList> Super;
		typedef std::vector<std::string> HasValKeyList;
		HasValKeyList have_val_key_;
		std::map<char, std::string> alias_;
		bool doseKeyHasVal(const std::string &key) const;
		bool hasAlias(const char opt) const;
		bool checkOpt(const char *arg, std::string &cur_key);
		static std::istream &tokenize(std::istream &lhs, std::string &rhs);
	};

} // namespace gctp

#endif //_GUTIL_ARGMAP_HPP_