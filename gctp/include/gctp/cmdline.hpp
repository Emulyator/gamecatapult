#ifndef _GUTIL_CMDLINE_HPP_
#define _GUTIL_CMDLINE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file cmdline.hpp
 * コマンドライン引数解析
 * 
 * @author osamu takasugi
 * @date 2003年5月21日
 * $Id: cmdline.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 */
#include <string>
#include <vector>

namespace gctp {
	/** コマンドライン引数解析
	*
	* Windowsの単一文字列式のコマンドラインを読み取って、
	* int argc, char *argv[] 方式に変換する。
	*
	* また、ファイルストリームをコマンドライン引数として解釈する機能もある。
	* 
	* @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	* @date 2004/01/18 23:06:52
	* Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	*/
	template<typename _CHAR>
	class CmdLineBase {
	public:
		CmdLineBase() : argv_(0) {}
		CmdLineBase(const _CHAR *cmdline) {
			parse(cmdline);
		}
		CmdLineBase(std::basic_istream<_CHAR> &in) {
			parse(in);
		}

		void push(const _CHAR *newarg)
		{
			vec_.push_back(std::basic_string<_CHAR>(newarg));
			sync();
		}

		void push(const std::basic_string<_CHAR> &newarg)
		{
			vec_.push_back(newarg);
			sync();
		}

		void parse(const _CHAR *cmdline)
		{
			std::basic_string<_CHAR> str;
			while((cmdline = tokenize(cmdline, str)) != NULL) {
				vec_.push_back(str);
			}
			sync();
		}

		void parse(std::basic_istream<_CHAR> &in)
		{
			std::basic_string<_CHAR> str;
			while(!tokenize(in, str).eof()) {
				vec_.push_back(str);
			}
			sync();
		}

		/// argcを返す
		int argc() const { return (int)arg_.size(); }

		/// argvを返す
		char **argv() { if(arg_.empty()) return 0; else return &arg_[0]; }
		/// argvを返す
		const char **argv() const { if(arg_.empty()) return 0; else return &arg_[0]; }

		/** 文字列からからコマンド引数の１トークンを切り出し
		*
		* @author osamu takasugi
		* @date 2003年5月21日
		* $Id: cmdline.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
		*/
		static const char *tokenize(const _CHAR *cmdline, std::basic_string<_CHAR> &rhs)
		{
			bool in_quote = false;
			rhs.clear();
			while(cmdline && *cmdline) {
				int ch;
				ch = *cmdline++;
#ifdef _MBCS
				if(isleadbyte(ch)) ch = ch<<8 | *cmdline++;
#endif
				// ""の中でなければ空白は区切り
				if((ch == ' ' || ch == '　' || ch == '\n') && !in_quote) {
					return cmdline;
				}
				else if(ch == '"') {
					if(in_quote) in_quote = false;
					else in_quote = true;
					continue;
				}
#ifdef _MBCS
				if(ch > 255) (rhs += (char)(ch>>8)) += (char)ch;
				else
#endif
					rhs += (char)ch;
			}
			return 0;
		}

		/** ストリームからコマンド引数の１トークンを切り出し
		*
		* @author osamu takasugi
		* @date 2003年5月21日
		* $Id: cmdline.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
		*/
		static std::istream &tokenize(std::basic_istream<_CHAR> &lhs, std::basic_string<_CHAR> &rhs)
		{
			bool in_quote = false;
			rhs.clear();
			while(!lhs.eof()) {
				int ch;
				ch = lhs.get();
#ifdef _MBCS
				if(isleadbyte(ch)) ch = ch<<8 | lhs.get();
#endif
				if((ch == ' ' || ch == '\n' || ch == '　') && !in_quote) break; // ""の中でなければ空白は区切り
				else if(ch == '"') {
					if(in_quote) in_quote = false;
					else in_quote = true;
					continue;
				}
#ifdef _MBCS
				if(ch > 255) (rhs += (char)(ch>>8)) += (char)ch;
				else
#endif
					rhs += (char)ch;
			}
			return lhs;
		}
	private:
		void sync()
		{
			arg_.resize(vec_.size());
			std::vector< char * >::iterator j = arg_.begin();
			for(std::vector< std::basic_string<_CHAR> >::iterator i = vec_.begin(); i != vec_.end(); ++i)
			{
				(*j++) = reinterpret_cast<char *>(const_cast<_CHAR *>((*i).c_str()));
			}
		}
		std::vector< char * > arg_;
		std::vector< std::basic_string<_CHAR> > vec_;
	};
	typedef CmdLineBase<char> CmdLine;
	typedef CmdLineBase<wchar_t> WCmdLine;
}

#endif //_GUTIL_CMDLINE_HPP_