/** @file
 * GameCatapultアーカイブファイルクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/12/30
 * $Id: archive.cpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/archive.hpp>
#include <gctp/bbstream.hpp>
#include <assert.h>
#include <time.h>
#include <gctp/dbgout.hpp>
#include <gctp/cstr.hpp>
#include <_direct.h>
#include <iostream>
#ifdef __ee__
#include <sys/types.h>
#endif

using namespace std;

namespace gctp {

	Archive::~Archive()
	{
		if(crypt_) delete crypt_;
	}

	void Archive::open(const _TCHAR *fn)
	{
		File::open(fn);
		if(!readIndex()) close();
	}
	
	void Archive::setKey(const char *key)
	{
		if(crypt_) delete crypt_;
		crypt_ = new Crypt(key);
	}

	/** インデックス読み込み
	 *
	 * @return 空でなく、gcarファイルでもない場合、falseを返す。通常true
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/01/02 9:44:54
	 * $Id: archive.cpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	bool Archive::readIndex()
	{
#define _SWAP_(n)	(((n)&0xFF000000)>>24|((n)&0xFF0000)>>8|((n)&0xFF00)<<8|((n)&0xFF)<<24)
		if(!File::is_open()) return false;
		if(length()>0) {
			File::seek(0);
			int head;
			(*this) >> head;
			if(head == _SWAP_('GCAR')) {
				(*this) >> index_size_;
				(*this) >> flags_;
				ibstream *stream = 0;
				ibbstream in_memory;
				BufferPtr in_memory_index;
				if(flags_ & FLAG_CRYPTED) {
					if(!crypt_) return false;
					stream = &in_memory;
					in_memory_index = File::load(-1, align(index_size_, 8));
					//crypt_->reset();
					crypt_->setKey("");
					if(!crypt_->decode(in_memory_index.get(), align(index_size_, 8))) return false;
				}
				else {
					stream = this;
				}
				while(tell() < index_size_) {
					basic_string<_TCHAR> name; time_t time; int size, pos;
					(*stream) >> name >> time >> size >> pos;
					PRNN("read "<<name.c_str()<<" "<<ctime(&time)<<"\tsize "<<size<<", pos "<<pos);
					ArchiveEntry &ent = index_[name];
					ent.time = time;
					ent.size = size;
					ent.pos = pos;
				}
			}
			else return false;
		}
		return true;
#undef _SWAP_
	}

	ArchiveEntry *Archive::get(const _TCHAR *fn)
	{
		if(index_.end() != index_.find(fn)) {
			return &index_[fn];
		}
		return NULL;
	}

	Archive &Archive::seek(const ArchiveEntry *entry)
	{
		File::seek(entry->pos);
		return *this;
	}

	Archive &Archive::seek(const _TCHAR *fn)
	{
		IndexItr i = index_.find(fn);
		if(index_.end() != i) {
			File::seek(i->second.pos);
		}
		return *this;
	}

	Archive &Archive::extract(const _TCHAR *fn, const _TCHAR *entryname)
	{
		ArchiveEntry *entry = get(entryname);
		if(entry) File::extract(File(fn, File::WRITE), entry->pos, entry->size);
		return *this;
	}

	Archive &Archive::extract(const _TCHAR *fn, const ArchiveEntry *entry)
	{
		File::extract(File(fn, File::WRITE), entry->pos, entry->size);
		return *this;
	}

	Archive &Archive::extract()
	{
		for(IndexItr i = index_.begin(); i != index_.end(); i++) {
			File::extract(File(i->first.c_str(), File::WRITE), i->second.pos, i->second.size);
		}
		return *this;
	}

	Archive &Archive::read(void *dst, const _TCHAR *entryname)
	{
		ArchiveEntry *entry = get(entryname);
		File::seek(entry->pos);
		File::read(dst, entry->size);
		return *this;
	}

	Archive &Archive::read(void *dst, const ArchiveEntry *entry)
	{
		File::seek(entry->pos);
		File::read(dst, entry->size);
		return *this;
	}

	Archive &Archive::read(void *dst, const int size)
	{
		File::read(dst, size);
		return *this;
	}

	BufferPtr Archive::load(const _TCHAR *entryname)
	{
		ArchiveEntry *entry = get(entryname);
		if(entry) return load(entry);
		return BufferPtr();
	}

	BufferPtr Archive::load(const ArchiveEntry *entry)
	{
		return File::load(entry->pos, entry->size);
	}

	BufferPtr Archive::load(const int size)
	{
		return File::load(-1, size);
	}

	void Archive::printIndex(std::ostream &os)
	{
		for(IndexItr i = index_.begin(); i != index_.end(); ++i) {
			os<<CStr(i->first.c_str())<<" "<<ctime(&i->second.time)<<", size "<<i->second.size<<", offset "<<i->second.pos<<endl;
		}
	}

} // namespace gctp
