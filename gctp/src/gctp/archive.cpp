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

namespace {

	void setiv(gctp::Crypt::DWord &iv, const char *str)
	{
		iv.word0.word = iv.word1.word = 0;
		for(int i = 0; i < 256 && *str; i++, str++)
		{
			iv.word0.word += *str;
			iv.word1.word += *str * *str;
		}
	}

}

namespace gctp {

	void ArchiveReader::setKey(const char *key)
	{
		if(key) {
			if(!filter_) {
				crypt_ = new Crypt;
				iv_ = new Crypt::DWord;
				setiv(*iv_, key);
				filter_ = new cbccryptfilter(*crypt_, *iv_);
			}
			crypt_->setKey(key);
		}
		else if(filter_) {
			delete filter_;
			delete crypt_;
			delete iv_;
			filter_ = 0;
			crypt_ = 0;
			iv_ = 0;
		}
	}
	
	void ArchiveReader::setKey(const Archive &src)
	{
		if(src.filter_ && src.flags_ & FLAG_CRYPTED) {
			flags_ = src.flags_;
			crypt_ = new Crypt(*src.crypt_);
			iv_ = new Crypt::DWord(*src.iv_);
			filter_ = new cbccryptfilter(*crypt_, *iv_);
			filter_->open(rdbuf(filter_));
		}
		else if(filter_) {
			delete filter_;
			delete crypt_;
			delete iv_;
			filter_ = 0;
			crypt_ = 0;
			iv_ = 0;
		}
	}

	void ArchiveReader::preseek(const ArchiveEntry &entry)
	{
		if(filter_) {
			filter_->setBasePosition(entry.pos);
			setiv(*iv_, reinterpret_cast<const char *>(entry.id.c_str()));
		}
	}

	ArchiveReader &ArchiveReader::seek(const ArchiveEntry *entry)
	{
		preseek(*entry);
		File::seekg(entry->pos);
		return *this;
	}

	ArchiveReader &ArchiveReader::extract(const _TCHAR *fn, const ArchiveEntry *entry)
	{
		File ef(fn, File::WRITE);
		if(ef.is_open()) {
			preseek(*entry);
			File::extract(ef, entry->pos, entry->size);
		}
		else PRNN(_T("書き出しファイル:'")<<fn<<_T("'を開けませんでした。"));
		return *this;
	}

	ArchiveReader &ArchiveReader::read(void *dst, const ArchiveEntry *entry)
	{
		preseek(*entry);
		File::seekg(entry->pos);
		File::read(dst, entry->size);
		return *this;
	}

	ArchiveReader &ArchiveReader::read(void *dst, const int size)
	{
		File::read(dst, size);
		return *this;
	}

	BufferPtr ArchiveReader::load(const ArchiveEntry *entry)
	{
		preseek(*entry);
		return File::load(entry->pos, entry->size);
	}

	BufferPtr ArchiveReader::load(const int size)
	{
		return File::load(-1, size);
	}





	void Archive::open(const _TCHAR *fn)
	{
		File::open(fn);
		if(!readIndex()) close();
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
		if(!isOpen()) return false;
		if(length()>0) {
			rdbuf(filebuf());
			File::seekg(0);
			int head;
			(*this) >> head;
			if(head == _SWAP_('GCAR')) {
				(*this) >> index_size_;
				(*this) >> flags_;
				if(flags_ & FLAG_CRYPTED) {
					if(!filter_) return false;
					filter_->open(rdbuf(filter_));
				}
				while(tellg() < index_size_) {
					basic_string<_TCHAR> name; time_t time; int size, pos;
					(*this) >> name >> time >> size >> pos;
					ArchiveEntry ent(name.c_str());
					ent.time = time;
					ent.size = size;
					ent.pos = pos;
					index_.insert(ent);
					//PRNN("read "<<ent);
				}
			}
			else return false;
			if(flags_ & FLAG_CRYPTED) {
				if(!filter_) return false;
				filter_->close();
				rdbuf(filebuf());
				File::seekg(0);
				filter_->open(rdbuf(filter_));
			}
		}
		return true;
#undef _SWAP_
	}

	ArchiveEntry *Archive::get(const _TCHAR *fn)
	{
		IndexItr i = index_.find(ArchiveEntry(fn));
		if(index_.end() != i) return &(*i);
		return NULL;
	}

	Archive &Archive::seek(const _TCHAR *fn)
	{
		IndexItr i = index_.find(ArchiveEntry(fn));
		if(index_.end() != i) {
			preseek(*i);
			File::seekg(i->pos);
		}
		return *this;
	}

	Archive &Archive::extract(const _TCHAR *fn, const _TCHAR *entryname)
	{
		ArchiveEntry *entry = get(entryname);
		if(entry) {
			preseek(*entry);
			File::extract(File(fn, File::WRITE), entry->pos, entry->size);
		}
		return *this;
	}

	Archive &Archive::extract()
	{
		for(IndexItr i = index_.begin(); i != index_.end(); i++) {
			File ef(i->id.c_str(), File::WRITE);
			if(ef.is_open()) {
				preseek(*i);
				File::extract(ef, i->pos, i->size);
			}
			else PRNN(_T("書き出しファイル:'")<<i->id.c_str()<<_T("'を開けませんでした。"));
		}
		return *this;
	}

	Archive &Archive::read(void *dst, const _TCHAR *entryname)
	{
		ArchiveEntry *entry = get(entryname);
		preseek(*entry);
		File::seekg(entry->pos);
		File::read(dst, entry->size);
		return *this;
	}

	BufferPtr Archive::load(const _TCHAR *entryname)
	{
		ArchiveEntry *entry = get(entryname);
		if(entry) return ArchiveReader::load(entry);
		return BufferPtr();
	}

	void Archive::printIndex(std::ostream &os)
	{
		//for(IndexItr i = index_.begin(); i != index_.end(); ++i) os<<(*i)<<endl;
		// CStr/WCStr周りのバグで上手くいかない
		for(IndexItr i = index_.begin(); i != index_.end(); ++i) {
			char ts[26];
			ctime_s(ts, 26, &i->time);
			ts[24] = '\0';
			os<<CStr(i->id.c_str())<<" "<<ts<<"\tsize "<<i->size<<", offset "<<i->pos<<endl;
		}
	}

} // namespace gctp
