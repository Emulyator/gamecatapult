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
#include <assert.h>
#include <time.h>
#include <gctp/dbgout.hpp>
#include <_direct.h>
#ifdef __ee__
#include <sys/types.h>
#endif

using namespace std;

namespace gctp {

	////////////////////////
	// Archive
	//
	Archive::Archive(const _TCHAR *fn) : index_size_(0)
	{
		open(fn);
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
		if(length()>0) {
			File::seek(0);
			int head, size;
			(*this) >> head;
			if(head == _SWAP_('GCAR')) {
				(*this) >> size;
				index_size_ = size;
				while(tell() < size) {
					basic_string<_TCHAR> name; /*time_t time;*/ int time, size, pos;
					(*this) >> name >> time >> size >> pos;
					PRNN("read " << name.c_str() << " " << ctime((time_t *)&time) <<"\tsize " << size << ", pos " << pos);
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
		File::extract(File(fn, File::WRITE), entry->pos, entry->size);
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

	void Archive::printIndex(std::basic_ostream<_TCHAR> &os)
	{
		for(IndexItr i = index_.begin(); i != index_.end(); ++i) {
			char *time = ctime(&i->second.time);
			time[strlen(time)-1] = '\0';
			os << i->first.c_str() << " " << time << ", size " << i->second.size << ", offset " << i->second.pos << endl;
		}
	}

	////////////////////////
	// ArchiveEditable
	//
	ArchiveEditable::ArchiveEditable(const _TCHAR *fn) : alignment_(4), index_page_size_(4)
	{
		open(fn);
	}

	void ArchiveEditable::open(const _TCHAR *fn)
	{
		File::open(fn, File::WRITE);
		if(readIndex()) setUpList();
		else close();
	}

	void ArchiveEditable::add(const _TCHAR *fn)
	{
		ListItr li = find(fn);
		if(li == list_.end()) {
			struct _stat st;
			if( 0 == _tstat(fn, &st) ) {
				if(st.st_mode & _S_IFDIR) { // ディレクトリなら
					TDIR *dir; tdirent *ent;
					dir = topendir(fn);
					while(dir) {
						ent = treaddir(dir);
						if(!ent) break;
						if(_tcscmp(ent->d_name, _T(".")) && _tcscmp(ent->d_name, _T(".."))) {
							basic_string<_TCHAR> path(fn); (path += _T("/")) += ent->d_name;
							add(path.c_str());
							PRN(path.c_str());
						}
					}
					tclosedir(dir);
				}
				else {
					// 最後尾に追加
					ArchiveEntry newe;
					newe.time = st.st_mtime;
					newe.size = st.st_size;
					if(list_.size() > 0) newe.pos = list_.back().pos+list_.back().size;
					else newe.pos = 0;
					list_.push_back(EntryAttr(fn, newe, EntryAttr::NEW));
				}
			}
		}
		else {
			struct _stat st;
			if( 0 == _tstat(fn, &st) && (li->time != st.st_mtime || li->size != st.st_size)) {
				li->attr = EntryAttr::UPDATE;
				li->time = st.st_mtime; li->size = st.st_size;
			}
			else if(li->attr == EntryAttr::REMOVE) li->attr = EntryAttr::ALREADY;
		}
	}

	void ArchiveEditable::forceUpdate(const _TCHAR *fn)
	{
		ListItr li = find(fn);
		if(li != list_.end() && li->attr != EntryAttr::NEW) {
			struct _stat st;
			if(0 == _tstat(fn, &st)) {
				li->attr = EntryAttr::UPDATE;
				li->time = st.st_mtime; li->size = st.st_size;
			}
		}
	}

	void ArchiveEditable::remove(const _TCHAR *fn)
	{
		ListItr li;
		if((li = find(fn))!=list_.end()) {
			if(li->attr == EntryAttr::NEW) list_.erase(li);
			else li->attr = EntryAttr::REMOVE;
		}
	}

	void ArchiveEditable::update()
	{
		for(ListItr i = list_.begin(); i != list_.end(); ++i) {
			struct _stat st;
			if( 0 == _tstat(i->name.c_str(), &st) && (i->time != st.st_mtime || i->size != st.st_size) && i->attr != EntryAttr::NEW) {
				i->attr = EntryAttr::UPDATE;
				i->time = st.st_mtime; i->size = st.st_size;
			}
		}
	}

	long ArchiveEditable::calcIndexLen()
	{
		int ret = 8; // ヘッダサイズ
		for(ListItr i = list_.begin(); i != list_.end(); i++) {
			if(i->attr != EntryAttr::REMOVE) {
				ret += static_cast<int>(i->name.size() + sizeof(_TCHAR) + sizeof(time_t) + sizeof(long) + sizeof(long));
			}
		}
		assert(ret > 0);
		return ret;
	}

	long ArchiveEditable::commitEntrys(int index_size)
	{
		// インデックス部のサイズ変化はここでコミット
		// 以降、ALREADYフラグの立っているエントリーはpos_を付け替えるだけで、そのままで良いはず。
		int diff_index_real_size = align(indexalign(index_size_)) - align(indexalign(index_size));
		if(diff_index_real_size > 0) File::remove(0, diff_index_real_size);
		if(diff_index_real_size < 0) File::insert(0, -diff_index_real_size);
		int ret = align(indexalign(index_size));
		for(ListItr i = list_.begin(); i != list_.end(); i++) {
			switch(i->attr) {
			case EntryAttr::REMOVE:
				i->pos = ret;
				File::remove(i->pos, align(i->size));
				break;
			case EntryAttr::UPDATE:
				{
					int diff_size = align(index_[i->name].size) - align(i->size);
					i->pos = ret;
					ret += align(i->size);
					if(diff_size > 0) File::remove(i->pos,  diff_size);
					if(diff_size < 0) File::insert(i->pos, -diff_size);
					File::seek(i->pos); (*this) << File(i->name.c_str());
					int align_size = align(i->size);
					for(int j = 0; j < align_size - i->size; j++) (*this) << '\0';
				}
				break;
			case EntryAttr::NEW:
				{
					i->pos = ret;
					ret += align(i->size);
					File::seek(i->pos); (*this) << File(i->name.c_str());
					int align_size = align(i->size);
					for(int j = 0; j < align_size - i->size; j++) (*this) << '\0';
				}
				break;
			default:
				i->pos = ret;
				ret += align(i->size);
			}
		}
		assert(ret > 0);
		return ret;
	}

	void ArchiveEditable::commitIndex(int index_size)
	{
		index_size_ = index_size;
		File::seek(0);
		write("GCAR", 4) << index_size_;
		for(ListItr i = list_.begin(); i != list_.end(); i++) {
			if(i->attr != EntryAttr::REMOVE) {
				(*this) << i->name.c_str() << (int)i->time << i->size << i->pos;
				PRNN("write "<<i->name<<" "<<ctime(&i->time)<<"\tsize "<<i->size<<", pos "<<i->pos);
			}
		}
	}

	void ArchiveEditable::commit()
	{
		int old_file_size = length();
		int index_size = calcIndexLen();
		int cur_file_size = commitEntrys(index_size);
		commitIndex(index_size);
		// ファイルサイズ変更
		if(old_file_size > cur_file_size) truncate(cur_file_size); // 縮んだ場合は縮める。大きくなった場合は特に操作はいらない
		index_.clear();
		list_.clear();
		if(readIndex()) setUpList();
		else close();
	}

	void ArchiveEditable::printList(std::basic_ostream<_TCHAR> &os)
	{
		for(ListItr i = list_.begin(); i != list_.end(); ++i) {
			char *attr;
			switch(i->attr) {
			case EntryAttr::ALREADY: attr = "ALREADY"; break;
			case EntryAttr::NEW: attr = "NEW"; break;
			case EntryAttr::REMOVE: attr = "REMOVE"; break;
			case EntryAttr::UPDATE: attr = "UPDATE"; break;
			}
			os << i->name.c_str()<<" "<<ctime(&i->time)<<"\tsize "<<i->size<<", pos "<<i->pos<<", attr "<<attr<<endl;
		}
	}

	void ArchiveEditable::setUpList()
	{
		for(IndexItr i = index_.begin(); i != index_.end(); ++i) {
			list_.push_back(EntryAttr(i->first.c_str(), i->second));
		}
		list_.sort();
	}

} // namespace gctp
