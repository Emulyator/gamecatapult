#ifndef _GUTIL_CSV_HPP_
#define _GUTIL_CSV_HPP_
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

template<class E, class T>
std::basic_string<E, T> csv_escape(std::basic_string<E, T> &str)
{
	if(str.find('"') != std::basic_string<E, T>::npos
		|| str.find(',') != std::basic_string<E, T>::npos
		|| str.find('\n') != std::basic_string<E, T>::npos) {
		std::basic_string<E, T> ret("\"");
		ret += str + "\"";
		for(std::basic_string<E, T>::size_type pos = 0; (pos = str.find('"', pos)) != std::basic_string<E, T>::npos;) {
			ret.insert(pos, "\"");
		}
		return ret;
	}
	return str;
}

template<class E0, class T0, class E1, class T1>
std::basic_istream<E0, T0> &csv_tokenize(std::basic_istream<E0, T0> &lhs, std::basic_string<E1, T1> &rhs)
{
	bool in_quote = false, empty = true;
	rhs.clear();
	while(!lhs.eof()) {
		int ch;
		ch = lhs.get();
#ifdef _MBCS
		if(isleadbyte(ch)) ch = ch<<8 | lhs.get();
#endif
		if(ch == ',' && !in_quote) break; // ""の中でなければ','で終了
		else if((ch == ' ' || ch == '　') && !in_quote && empty) continue; // ""の中でなければ頭の空白は読み飛ばす
		else if(ch == '"') {
			ch = lhs.get();
			if(ch != '"') {
				if(in_quote) in_quote = false;
				else in_quote = true;
				lhs.unget(); // VC付属のSTLはungetバグってる…
				//lhs.putback(ch);
				continue;
			}
		}
		else if(ch == '\n') {
			if(!in_quote) {
				if(empty) rhs = '\n';
				else lhs.unget()/*lhs.putback(ch)*/;
				break;
			}
		}
		else if(ch == EOF) { // "\n"を行終了とみなすので、EOFも改行扱い
			if(empty) rhs = '\n';
			else lhs.unget()/*lhs.putback(ch)*/;
			break;
		}
		empty = false;
#ifdef _MBCS
		if(ch > 255) (rhs += (char)(ch>>8)) += (char)ch;
		else rhs += (char)ch;
#else
		rhs += (char)ch;
#endif
	}
	return lhs;
}

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

	template<class T0, class E0>
	std::basic_istream<E0, T0> &putBack(std::basic_istream<E0, T0> &in)
	{
		for(reverse_iterator i = rbegin(); i != rend(); ++i) {
			for(reverse_iterator j = i->rbegin(); j != i->rend(); ++j) {
				in.putback(*j); 
			}
		}
	}

	template<class T0, class E0>
	std::basic_ostream<T0, E0> &p(std::basic_ostream<T0, E0> &out)
	{
		out << "[";
		for(iterator i = begin(); i != end(); ++i) {
			if(i != begin()) out << ",";
			out << "\"" << *i << "\"";
		}
		out << "]";
		return out;
	}
};

template<class E0, class T0, class E1, class T1>
std::basic_ostream<E0 ,T0> &operator<<(std::basic_ostream<E0 ,T0> &lhs, CSVRowBase<E1, T1> &rhs)
{
	if(rhs.size() > 0) {
		for(CSVRowBase<E1, T1>::iterator i = rhs.begin(); i != rhs.end(); ++i) {
			if(i != rhs.begin()) lhs << ",";
			lhs << csv_escape(*i);
		}
		lhs << std::endl;
	}
	return lhs;
}

template<class E0, class T0, class E1, class T1>
std::basic_istream<E0, T0> &operator>>(std::basic_istream<E0, T0> &lhs, CSVRowBase<E1, T1> &rhs)
{
	std::basic_string<E1, T1> str;
	while(!lhs.eof()) {
		csv_tokenize(lhs, str);
		if(str == "\n") break;
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
	template<class E0, class T0>
	std::basic_ostream<E0, T0> &p(std::basic_ostream<E0, T0> &out)
	{
		out << "{" << std::endl;
		for(iterator i = begin(); i != end(); ++i) {
			if(i != begin()) out << "," << std::endl;
			out << "\t";
			i->p(out);
		}
		out << "}" << std::endl;
		return out;
	}
};

template<class E0, class T0, class E1, class T1>
std::basic_ostream<E0, T0> &operator<<(std::basic_ostream<E0, T0> &lhs, CSVBase<E1, T1> &rhs)
{
	for(CSVBase<E1, T1>::iterator i = rhs.begin(); i != rhs.end(); ++i) {
		lhs << *i;
	}
	return lhs;
}

template<class E0, class T0, class E1, class T1>
std::basic_istream<E0, T0> &operator>>(std::basic_istream<E0, T0> &lhs, CSVBase<E1, T1> &rhs)
{
	while(!lhs.eof()) {
		rhs.push_back(CSVRowBase<E1, T1>());
		lhs >> rhs.back();
		if(rhs.back().empty()) rhs.pop_back();
	}
	return lhs;
}

typedef CSVBase<char, std::char_traits<char> > CSV;
typedef CSVBase<wchar_t, std::char_traits<wchar_t> > WCSV;

#endif //_GUTIL_CSV_HPP_