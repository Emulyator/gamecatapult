#include "common.h"
#include <gctp/rop.hpp>
#include <gctp/csvsa.hpp>
#include <gctp/dbgout.hpp>
#include <map>
#include <stack>
#include <boost/lexical_cast.hpp>
//#include <boost/regex.hpp>
#include <iostream>

#define _PRN(s)

using namespace std;

namespace {

	typedef std::map<std::string, int> CSVSASymbolTable;

	class CSVSASymbolHandle {
	public:
		CSVSASymbolHandle(CSVSASymbolTable &owner, CSVSASymbolTable::iterator it) : owner_(owner), it_(it) {}
		bool isValid() { return it_ != owner_.end(); }
		int val() { return it_->second; }
		const std::string &key() const { return it_->first; }
	protected:
		CSVSASymbolTable &owner_;
		CSVSASymbolTable::iterator it_;
	};

	class CSVSASymbolTableStack : public std::vector<CSVSASymbolTable> {
	public:
		CSVSASymbolTableStack()
		{
			newScope(); // グローバルスコープ用
		}

		void newScope()
		{
			push_back(CSVSASymbolTable());
		}

		void pop()
		{
			pop_back();
		}

		bool registerIt(const std::string &key, int val)
		{
			if(!find(key).isValid()) {
				back()[key] = val;
				return true;
			}
			return false;
		}

		CSVSASymbolHandle find(const std::string &key)
		{
			for(reverse_iterator i = rbegin(); i != rend(); ++i) {
				CSVSASymbolTable::iterator ret = i->find(key);
				if(ret != i->end()) return CSVSASymbolHandle(*i, ret);
			}
			return CSVSASymbolHandle(back(), back().end());
		}
	} symt;
} // anonymous namespace

void CSVSAType::parse(const std::string &col) {
	std::string w = col;
	std::string::size_type pos;
	if(std::string::npos != (pos = w.find("{"))) w.erase(0, pos);
	if(std::string::npos != (pos = w.find("}"))) w.erase(pos);
	if(std::string::npos != (pos = w.find("uchar"))) {
		type = unum; size = 1;
	}
	else if(std::string::npos != (pos = w.find("char*"))) {
		type = sptr; size = 4;
	}
	else if(std::string::npos != (pos = w.find("char["))) {
		type = sarray;
		char *endptr; size = strtol(w.c_str()+pos+5, &endptr, 0);
	}
	else if(std::string::npos != (pos = w.find("char"))) {
		type = num; size = 1;
	}
	else if(std::string::npos != (pos = w.find("ushort"))) {
		type = unum; size = 2;
	}
	else if(std::string::npos != (pos = w.find("short"))) {
		type = num; size = 2;
	}
	else if(std::string::npos != (pos = w.find("ulonglong"))) {
		type = unum; size = 8;
	}
	else if(std::string::npos != (pos = w.find("longlong"))) {
		type = num; size = 8;
	}
	else if(std::string::npos != (pos = w.find("ulong"))) {
		type = unum; size = 4;
	}
	else if(std::string::npos != (pos = w.find("long"))) {
		type = num; size = 4;
	}
	else if(std::string::npos != (pos = w.find("float"))) {
		type = fnum; size = 4;
	}
	else if(std::string::npos != (pos = w.find("double"))) {
		type = fnum; size = 8;
	}
	else if(std::string::npos != (pos = w.find("pointer"))) {
		type = ptr; size = 4;
	}
	else {
		type = empty; size = 0;
	}
}

void CSVSAType::output(std::strstream &out, std::string col, size_t chunksize, std::strstream &strbuf, Rop &rop)
{
	switch(type) {
	case empty:
		_PRN("empty:");
		break;

	case num: {
		long w; char *endptr;
		if(std::string::npos != col.find(".")) w = static_cast<long>(strtod(col.c_str(), &endptr));
		else w = strtol(col.c_str(), &endptr, 0);
		_PRN(size<<":"<<w);
		while(out.tellp()>0 && out.tellp()%size) out << ends; // それぞれのサイズ境界になかったらパディング
		out.write((char*)&w, static_cast<std::streamsize>(size));
			  }
		break;

	case unum: {
		unsigned long w; char *endptr;
		if(string::npos != col.find(".")) w = static_cast<unsigned long>(strtod(col.c_str(), &endptr));
		else w = strtoul(col.c_str(), &endptr, 0);
		_PRN(size<<":"<<w);
		while(out.tellp()>0 && out.tellp()%size) out << ends; // それぞれのサイズ境界になかったらパディング
		out.write((char*)&w, static_cast<std::streamsize>(size));
			   }
		break;

	case fnum: {
		char *endptr;
		float w = static_cast<float>(strtod(col.c_str(), &endptr));
		_PRN("float:"<<w);
		out.write((char*)&w, static_cast<std::streamsize>(size));
			   }
		break;

	case sarray:
		_PRN("colsize"<<col.size()<<":");
		if(col.size() < size-1) {
			out.write(col.c_str(), static_cast<std::streamsize>(col.size()));
			_PRN("str:"<<col);
			for(size_t i = col.size(); i < size; i++) {
				_PRN(".");
				out << '\0';
			}
		}
		else {
			_PRN("str:"<<col<<":"<<size);
			out.write(col.c_str(), static_cast<std::streamsize>(size-1)); out << '\0';
		}
		break;

	case sptr: {
		long w;
		_PRN("str ptr:"<<col);
		while(out.tellp()>0 && out.tellp()%4) out << ends; // ４バイト境界になかったらパディング
		if(!col.empty()) {
			w = long(chunksize + strbuf.tellp());
			strbuf << col << ends; // 文字列バッファに書き出し
			rop.add(out.tellp()); // Ropに登録
		}
		else w = 0;
		out.write((char*)&w, sizeof(w));
			   }
		break;

	case ptr: {
		unsigned long w; char *endptr;
		if(string::npos != col.find(".")) w = static_cast<unsigned long>(strtod(col.c_str(), &endptr));
		else w = strtoul(col.c_str(), &endptr, 0);
		if(w>0) rop.add(out.tellp()); // Ropに登録
		_PRN("ptr :"<<col);
		while(out.tellp()>0 && out.tellp()%size) out << ends; // それぞれのサイズ境界になかったらパディング
		out.write((char*)&w, sizeof(w));
			   }
		break;
	}
}

void CSVSA::registerSymbol(size_t ofs)
{
	symt.registerIt(schema_.name_, static_cast<int>(size()));
	int ni = 0;
	for(iterator i = begin(); i != end(); ++i, ni++) {
		CSVRow::iterator j = i->begin();
		if(!j->empty()) symt.registerIt(*j, static_cast<int>(ofs+schema_.bytesize()*ni));
	}
}

void CSVSA::translateSymbol()
{
	for(iterator i = begin(); i != end(); ++i) {
		CSVRow::iterator j = i->begin();
		for(++j; j != i->end(); ++j) {
			CSVSASymbolHandle sym = symt.find(*j);
			if(sym.isValid()) *j = boost::lexical_cast<string>(sym.val());
		}
	}
}

void CSVSA::output(std::strstream &out, size_t chunksize, std::strstream &strbuf, Rop &rop)
{
	for(iterator i = begin(); i != end(); ++i) {
		CSVRow::iterator col = i->begin();
		++col;
		for(CSVSASchema::iterator j = schema_.begin(); j != schema_.end(); ++j) {
			if(col != i->end()) {
				j->output(out, *col, chunksize, strbuf, rop);
				++col;
			}
			else {
				j->output(out, string(), chunksize, strbuf, rop);
			}
		}
	}
}

void CSVSAChunk::parse(const CSVRow &csvl)
{
	if(csvl[0].find("def:") != string::npos) {
		if(symt.size() < 2) symt.newScope();
		push_back(CSVSA(csvl));
	}
	else if(!empty()) {
		back().parse(csvl);
	}
/*	else {
		boost::reg_expression<char> regex = "#[ \t]*define[ \t]+([a-zA-Z][a-zA-Z0-9]*)[ \t]+.*([0-9]+)";
		boost::match_results<const char*> results;
		if(boost::regex_search(csvl[0].c_str(), results, regex)) {
			symt.registerIt(results.str(1), boost::lexical_cast<int>(results.str(2)));
		}
	}*/
}

void CSVSAChunk::flush(ostream &out)
{
	if(empty()) return;
	
	int ofs = 0;
	for(iterator i = begin(); i != end(); ++i) {
		i->registerSymbol(ofs);
		ofs += int(i->bytesize());
	}
	for(iterator i = begin(); i != end(); ++i) {
		i->translateSymbol();
	}
	strstream buf; strstream strbuf; Rop rop;
	for(iterator i = begin(); i != end(); ++i) {
		i->output(buf, ofs, strbuf, rop);
	}
	// チャンクヘッダ吐き出し
	_PRN("CHNK : "<<str4CC());
	out << str4CC();
	long size = (buf.pcount()+3)/4*4 + (strbuf.pcount()+3)/4*4; // ４バイトアライメント
	_PRN(" SIZE : "<<size<<endl);
	out.write((char *)&size, 4);
	out << buf.rdbuf(); // 実データ吐き出し
	while(out.tellp()>0 && out.tellp()%4) out << ends; // ４バイト境界になかったらパディング
	out << strbuf.rdbuf(); // 文字列データ吐き出し
	while(out.tellp()>0 && out.tellp()%4) out << ends; // ４バイト境界になかったらパディング
	if(rop.size()) out << rop; // RoP吐き出し
	while(out.tellp()>0 && out.tellp()%4) out << ends; // ４バイト境界になかったらパディング

	clear();
	fourcc_ = 'CSVS';
}

/** CSVで書かれたファイルをiffチャンク方式のバイナリファイルに変換する。
 *
 * pointerやstr*が使われた場合、普通のiffチャンクの直後にNinjaで使われた
 * pof0チャンクという物が作られる。
 *
 * 吐き出されるファイルの構成
 * HEAD 4 'start:xxxx'の構文で指定したもの  デフォルトでは'CSVS'
 * SIZE 4  チャンクのバイトサイズ
 *	    実際のデータ
 * HEAD 4  'ROPS'
 * SIZE 4
 *      ROPデータ
 */
void CSVSAChunk::parse(istream &in, ostream &out)
{
	CSVSAChunk chunk;
	while(!in.eof()) {
		CSVRow csvl;
		in >> csvl;
		if(!csvl.isNull()) {
			if(csvl[0].find("start:") != string::npos) {
				chunk.flush(out);
				if(symt.size() > 1) symt.pop();
				chunk.set4CC(csvl[0]);
				if(symt.size() < 2) symt.newScope();
			}
			else if(csvl[0].find("end:") != string::npos) {
				chunk.flush(out);
				if(symt.size() > 1) symt.pop();
			}
			else chunk.parse(csvl);
		}
	}
	chunk.flush(out);
}
