#ifndef _GCTP_CSV_HPP_
#define _GCTP_CSV_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file
 * CSV解析ユーティリティー
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/31 3:22:39
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <iosfwd> // for i/ostream
#include <string>
#include <vector>
#include <cctype>

namespace gctp {

	template<class E>
	class CSVChar {};

	template<>
	class CSVChar<char> {
	public:
		static const char dquot = '"';
		static const char comma = ',';
		static const char tab = '\t';
		static const char newline = '\n';
		static const char lbracket = '[';
		static const char rbracket = ']';
		static const char lbrace = '{';
		static const char rbrace = '}';
		static const char endf = EOF;
		static const char *emptyline()
		{
			return "\n";
		}
		static bool isspace(char ch)
		{
			return ch == '　' || (std::isspace((int)ch) != 0);
		}
		template<class E1, class T1>
		static int get(std::basic_istream<E1, T1> &from)
		{
			int ret = from.get();
#ifdef _MBCS
			if(::isleadbyte(ret)) ret = ret<<8 | from.get();
#endif
			return ret;
		}
		template<class E1, class T1>
		static void put(std::basic_string<E1, T1> &to, int ch)
		{
#ifdef _MBCS
			if(ch > 255) (to += (char)(ch>>8)) += (char)ch;
			else to += (char)ch;
#else
			to += (char)ch;
#endif
		}
	};

	template<>
	class CSVChar<wchar_t> {
	public:
		static const wchar_t dquot = L'"';
		static const wchar_t comma = L',';
		static const wchar_t tab = L'\t';
		static const wchar_t newline = L'\n';
		static const wchar_t lbracket = L'[';
		static const wchar_t rbracket = L']';
		static const wchar_t lbrace = L'{';
		static const wchar_t rbrace = L'}';
		static const wchar_t endf = WEOF;
		static const wchar_t *emptyline()
		{
			return L"\n";
		}
		static bool isspace(wchar_t ch)
		{
			return ch == L'　' || (iswspace(ch) != 0);
		}
		template<class E1, class T1>
		static int get(std::basic_istream<E1, T1> &from)
		{
			return from.get();
		}
		template<class E1, class T1>
		static void put(std::basic_string<E1, T1> &to, int ch)
		{
			to += (wchar_t)ch;
		}
	};

	template<class E, class T>
	class CSVUtil {
	public:
		static std::basic_string<E, T> escape(std::basic_string<E, T> &str)
		{
			if(str.find(CSVChar<E>::dquot) != std::basic_string<E, T>::npos
				|| str.find(CSVChar<E>::comma) != std::basic_string<E, T>::npos
				|| str.find(CSVChar<E>::newline) != std::basic_string<E, T>::npos) {
				std::basic_string<E, T> ret;
				ret.push_back(CSVChar<E>::dquot);
				ret += str;
				ret.push_back(CSVChar<E>::dquot);
				for(std::basic_string<E, T>::size_type pos = 0; (pos = str.find(CSVChar<E>::dquot, pos)) != std::basic_string<E, T>::npos;) {
					ret.insert(pos, 1, CSVChar<E>::dquot);
				}
				return ret;
			}
			return str;
		}

		static std::basic_istream<E, T> &tokenize(std::basic_istream<E, T> &lhs, std::basic_string<E, T> &rhs)
		{
			bool in_quote = false, empty = true;
			rhs.clear();
			while(!lhs.eof()) {
				int ch = CSVChar<E>::get(lhs);
				if(ch == CSVChar<E>::comma && !in_quote) break; // ""の中でなければ','で終了
				else if(ch == CSVChar<E>::newline) {
					if(!in_quote) {
						if(empty) rhs = CSVChar<E>::newline;
						else lhs.unget();
						break;
					}
				}
				else if(ch == CSVChar<E>::endf) { // "\n"を行終了とみなすので、EOFも改行扱い
					if(empty) rhs = CSVChar<E>::newline;
					else lhs.unget();
					break;
				}
				else if(CSVChar<E>::isspace(ch) && !in_quote && empty) continue; // ""の中でなければ頭の空白は読み飛ばす
				else if(ch == CSVChar<E>::dquot) {
					ch = lhs.get();
					if(ch != CSVChar<E>::dquot) {
						if(in_quote) in_quote = false;
						else in_quote = true;
						lhs.unget();
						continue;
					}
				}
				empty = false;
				CSVChar<E>::put(rhs, ch);
			}
			return lhs;
		}
	};

	/** CSVの一行分を読み込んで文字列ベクタとして保持
	 *
	 * SJISにのみ対応。全角空白も空白として取り除いたりする。
	 * ロケールを日本語にしておかないと働かないので注意。
	 * ""中の改行にも対応。
	 * 
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/31 3:22:54
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class E, class T>
	class CSVRowBase : public std::vector< std::basic_string<E, T> > {
	public:
		bool isNull() const
		{
			if(empty()) return true;
			for(const_iterator i = begin(); i != end(); ++i) {
				if(!i->empty()) return false;
			}
			return true;
		}

		std::basic_istream<E, T> &putBack(std::basic_istream<E, T> &in)
		{
			for(reverse_iterator i = rbegin(); i != rend(); ++i) {
				for(reverse_iterator j = i->rbegin(); j != i->rend(); ++j) {
					in.putback(*j); 
				}
			}
		}

		std::basic_ostream<E, T> &p(std::basic_ostream<E, T> &out)
		{
			out.put(CSVChar<E>::lbracket);
			for(iterator i = begin(); i != end(); ++i) {
				if(i != begin()) out.put(CSVChar<E>::comma);
				out.put(CSVChar<E>::dquot);
				out << *i;
				out.put(CSVChar<E>::dquot);
			}
			out.put(CSVChar<E>::rbracket);
			return out;
		}
	};

	template<class E, class T>
	std::basic_ostream<E, T> &operator<<(std::basic_ostream<E, T> &lhs, CSVRowBase<E, T> &rhs)
	{
		if(rhs.size() > 0) {
			for(CSVRowBase<E, T>::iterator i = rhs.begin(); i != rhs.end(); ++i) {
				if(i != rhs.begin()) lhs.put(CSVChar<E>::comma);
				lhs << CSVUtil<E, T>::escape(*i);
			}
			lhs << std::endl;
		}
		return lhs;
	}

	template<class E, class T>
	std::basic_istream<E, T> &operator>>(std::basic_istream<E, T> &lhs, CSVRowBase<E, T> &rhs)
	{
		std::basic_string<E, T> str;
		while(!lhs.eof()) {
			CSVUtil<E, T>::tokenize(lhs, str);
			if(str == CSVChar<E>::emptyline()) break;
			rhs.push_back(str);
		}
		return lhs;
	}

	typedef CSVRowBase<char, std::char_traits<char> > CSVRow;
	typedef CSVRowBase<wchar_t, std::char_traits<wchar_t> > WCSVRow;

	/** ファイル全体を読み込んでCSVRawLineのベクタを形成
	 *
	 * SJISにのみ対応（といっても２バイト文字しかない、isleadbyteが働く、という前提なだけなので、EUCでも動くはず）。
	 *
	 * 全角空白も空白として取り除いたりする。
	 *
	 * ロケールを日本語にしておかないと働かない(isleadbyteが)ので注意。
	 * 
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/31 3:23:06
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class E, class T>
	class CSVBase : public std::vector< CSVRowBase<E, T> > {
	public:
		std::basic_ostream<E, T> &p(std::basic_ostream<E, T> &out)
		{
			out.put(CSVChar<E0>::lbrace);
			out << std::endl;
			for(iterator i = begin(); i != end(); ++i) {
				if(i != begin()) {
					out.put(CSVChar<E0>::comma);
					out << std::endl;
				}
				out.put(CSVChar<E0>::tab);
				i->p(out);
			}
			out.put(CSVChar<E0>::rbrace);
			out << std::endl;
			return out;
		}
	};

	template<class E, class T>
	std::basic_ostream<E, T> &operator<<(std::basic_ostream<E, T> &lhs, CSVBase<E, T> &rhs)
	{
		for(CSVBase<E, T>::iterator i = rhs.begin(); i != rhs.end(); ++i) {
			lhs << *i;
		}
		return lhs;
	}

	template<class E, class T>
	std::basic_istream<E, T> &operator>>(std::basic_istream<E, T> &lhs, CSVBase<E, T> &rhs)
	{
		while(!lhs.eof()) {
			rhs.push_back(CSVRowBase<E, T>());
			lhs >> rhs.back();
			if(rhs.back().empty()) rhs.pop_back();
		}
		return lhs;
	}

	typedef CSVBase<char, std::char_traits<char> > CSV;
	typedef CSVBase<wchar_t, std::char_traits<wchar_t> > WCSV;

}

#endif //_GCTP_CSV_HPP_
