/** @file
 * GameCatapult シリアライザクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:29:04
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <sstream>
#include <gctp/serializer.hpp>
#include <gctp/class.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp {

	Serializer::Serializer(ibstream &stream) : istream_(&stream), ostream_(0)
	{
		uint32_t head;
		uint32_t magic;
		*istream_ >> head >> magic;
		if(head != HEAD || magic != MAGIC) istream_ = 0;
	}

	Serializer::Serializer(obstream &stream) : istream_(0), ostream_(&stream)
	{
		*ostream_ << (uint32_t)HEAD << (uint32_t)MAGIC; // ヘッダ
	}

	std::size_t Serializer::findPtr(Ptr ptr)
	{
		for(std::size_t i = 0; i < ptrs.size(); i++) {
			if(ptrs[i]==ptr) return i+1;
		}
		return 0;
	}
	
	void Serializer::checkVersion(int ver)
	{
		if(isLoading()) {
			int _ver;
			istream() >> _ver;
			if(ver != _ver) throw LoadFailExeption();
		}
		else if(isWriting()) {
			ostream() << ver;
		}
	}

	template<>
	Serializer &operator<<(Serializer &lhs, Object &rhs)
	{
		rhs.serialize(lhs);
		return lhs;
	}

	template<>
	Serializer &operator<<(Serializer &lhs, Ptr &rhs)
	{
		if(lhs.isLoading()) {
			size_t id;
			lhs.istream() >> id;
			if(id==0) {
				std::string classname;
				lhs.istream() >> classname;
				rhs = Factory::create(classname.c_str());
				lhs.ptrs.push_back(rhs);
				if(rhs) lhs << *rhs;
			}
			else if(id-1 < lhs.ptrs.size()) rhs = lhs.ptrs[id-1];
		}
		else if(lhs.isWriting()) {
			size_t id = lhs.findPtr(rhs);
			if(id==0) {
				lhs.ptrs.push_back(rhs);
				Class::Name name = Class::get(GCTP_THISTYPEID(*rhs.get()));
				if(!name.ns) name.ns = "?";
				if(!name.classname) name.classname = "?";
				{
					stringstream str;
					str << name.ns << "." << name.classname;
					lhs.ostream() << id << str.str();
				}
				lhs << *rhs;
			}
			else {
				lhs.ostream() << id;
			}
		}
		return lhs;
	}

	template<>
	Serializer &operator<<(Serializer &lhs, Hndl &rhs)
	{
		if(lhs.isLoading()) {
			size_t id;
			lhs.istream() >> id;
			if(id==0) {
				std::string classname;
				lhs.istream() >> classname;
				Ptr ptr = Factory::create(classname.c_str());
				lhs.ptrs.push_back(ptr);
				if(ptr) {
					lhs << *ptr;
					rhs = ptr;
				}
			}
			else {
				rhs = lhs.ptrs[id-1];
			}
		}
		else if(lhs.isWriting()) {
			size_t id = lhs.findPtr(rhs.lock());
			if(id==0) {
				Ptr ptr = rhs.lock();
				lhs.ptrs.push_back(ptr);
				Class::Name name = Class::get(GCTP_THISTYPEID(*rhs.get()));
				if(!name.ns) name.ns = "?";
				if(!name.classname) name.classname = "?";
				{
					stringstream str;
					str << name.ns << "." << name.classname;
					lhs.ostream() << id << str.str();
				}
				lhs << *ptr;
			}
			else {
				lhs.ostream() << id;
			}
		}
		return lhs;
	}

} // namespace gctp
