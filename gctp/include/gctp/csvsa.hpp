#ifndef _CSVSA_HPP_
#define _CSVSA_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file csv.hpp
 * CSVSA(Comma Sepalated Value Structure Array)解析
 *
 * @author osamu takasugi
 * @date 2003年5月21日
 * $Id: csvsa.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 */
#include <gctp/csv.hpp>
#include <strstream>

namespace gctp {

	class Rop;

	/** CSVSAスキーマの１属性値
	 *
	 * @author osamu takasugi
	 * @date 2003年5月21日
	 * $Id: csvsa.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
	 */
	struct CSVSAType {
		enum TypeID {
			empty,	// 空要素
			num,	// 符号の無し整数
			unum,	// 符号の有り整数
			fnum,	// 浮動少数
			sarray,	// 文字列の配列
			sptr,	// 文字列のポインタ
			ptr		// ポインタ
		};
		TypeID type;
		std::size_t size;
		CSVSAType() {}
		explicit CSVSAType(const std::string &col) {
			parse(col);
		}

		void parse(const std::string &col);
		void output(std::strstream &out, std::string col, size_t chunksize, std::strstream &strbuf, Rop &rop);
	};

	template<class E, class T>
	std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, CSVSAType const & rhs)
	{
		switch(rhs.type) {
		case CSVSAType::empty:
			os << "empty";
			break;
		case CSVSAType::num:
			os << "size:" << rhs.size;
			break;
		case CSVSAType::unum:
			os << "unsigned size:" << rhs.size;
			break;
		case CSVSAType::sptr:
			os << "char *";
			break;
		case CSVSAType::sarray:
			os << "char[" << rhs.size << "]";
			break;
		case CSVSAType::fnum:
			if(rhs.size == 4) os << "float";
			else if(rhs.size == 8) os << "double";
			else os << "fnum?:size:" << rhs.size;
			break;
		case CSVSAType::ptr:
			os << "pointer";
			break;
		}
		return os;
	}

	/** CSVSAスキーマ
	 *
	 * @author osamu takasugi
	 * @date 2003年5月21日
	 * $Id: csvsa.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
	 */
	class CSVSASchema : public std::vector<CSVSAType> {
	public:
		CSVSASchema() {}
		CSVSASchema(const CSVRow &src)
		{
			parse(src);
		}

		std::size_t bytesize() const
		{
			std::size_t ret = 0;
			for(const_iterator i = begin(); i != end(); ++i) {
				if(i->type != CSVSAType::sarray) {
					switch(i->size) {
					case 2: while(ret%2) ret++; break;
					case 4: while(ret%4) ret++; break;
					case 8: while(ret%8) ret++; break;
					}
				}
				ret += i->size;
			}
			return (ret+3)/4*4; // 4バイト境界に
		}

		void parse(const CSVRow &src)
		{
			std::string::size_type pos;
			if((pos = src[0].find("def:")) != std::string::npos) {
				name_ = std::string(src[0].c_str()+pos+4);
			}
			CSVRow::const_iterator i = src.begin();
			for(++i; i != src.end(); ++i) {
				push_back(CSVSAType(*i));
			}
		}

		std::string name_;
	};

	template<class E, class T>
	std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, CSVSASchema const & rhs)
	{
		for(unsigned int i = 0; i < rhs.size(); i++) {
			os << ((i==0)?"(":" (") << i << "):";
			os << rhs.at(i);
		}
		return os;
	}

	/** CSVSA表記の１定義分を保持
	 *
	 * @author osamu takasugi
	 * @date 2003年5月21日
	 * $Id: csvsa.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
	 */
	class CSVSA : public CSV {
	public:
		CSVSA(const CSVRow &schema) : schema_(schema) {}
		
		void parse(const CSVRow &csvl)
		{
			push_back(csvl);
		}
		
		void registerSymbol(size_t ofs);
		void translateSymbol();
		void output(std::strstream &out, size_t chunksize, std::strstream &strbuf, Rop &rop);
		
		std::size_t bytesize() const
		{
			return schema_.bytesize() * size();
		}

		CSVSASchema schema_;
	};

	typedef unsigned long uint32_t;
	// なんだこれ？？

	/** CSVSA表記の１チャンクを保持
	 *
	 * @author osamu takasugi
	 * @date 2003年5月21日
	 * $Id: csvsa.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
	 */
	class CSVSAChunk : public std::vector<CSVSA> {
	public:
		CSVSAChunk() : fourcc_('CSVS') {}
		CSVSAChunk(const std::string &name) { set4CC(name); }

		void set4CC(std::string name) {
			std::string::size_type pos;
			if((pos = name.find("start:")) != std::string::npos) {
				name = name.substr(pos+6, 4);
			}
			else name = name.substr(0, 4);
			while(name.size()%4) name += '\0'; // ４文字なかったらパディング
			fourcc_ = ((name[0]&0xFF)<<24) | ((name[1]&0xFF)<<16) | ((name[2]&0xFF)<<8) | (name[3]&0xFF);
		}
		std::string str4CC()
		{
			char c[5] = {
				static_cast<char>((fourcc_>>24)&0xFF),
				static_cast<char>((fourcc_>>16)&0xFF),
				static_cast<char>((fourcc_>>8)&0xFF),
				static_cast<char>((fourcc_)&0xFF),
				'\0'
			};
			return c;
		}

		void parse(const CSVRow &csvl);
		void flush(std::ostream &out);

		static void parse(std::istream &in, std::ostream &out);
		
		uint32_t fourcc_;
	};

}

#endif //_CSVSA_HPP_
