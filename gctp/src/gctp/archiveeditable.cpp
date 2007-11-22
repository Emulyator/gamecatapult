/** @file
 * GameCatapult�A�[�J�C�u�t�@�C���ҏW�N���X
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/12/30
 * $Id: archive.cpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/archiveeditable.hpp>
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

	ArchiveEditable::ArchiveEditable(const _TCHAR *fn) : alignment_(4)
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
				if(st.st_mode & _S_IFDIR) { // �f�B���N�g���Ȃ�
					TDIR *dir; tdirent *ent;
					dir = topendir(fn);
					while(dir) {
						ent = treaddir(dir);
						if(!ent) break;
						if(!_tcscmp(ent->d_name, _T(".")) && !_tcscmp(ent->d_name, _T(".."))) {
							basic_string<_TCHAR> path(fn); (path += _T("/")) += ent->d_name;
							add(path.c_str());
							PRN(path.c_str());
						}
					}
					tclosedir(dir);
				}
				else {
					// �Ō���ɒǉ�
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
		int ret = sizeof(int)*3; // �w�b�_�T�C�Y
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
		// �C���f�b�N�X���̃T�C�Y�ω��͂����ŃR�~�b�g
		// �ȍ~�AALREADY�t���O�̗����Ă���G���g���[��pos_��t���ւ��邾���ŁA���̂܂܂ŗǂ��͂��B
//		int diff_index_real_size = align(indexalign(index_size_)) - align(indexalign(index_size));
//		if(diff_index_real_size > 0) File::remove(0, diff_index_real_size);
//		if(diff_index_real_size < 0) File::insert(0, -diff_index_real_size);
		int ret = align(index_size);
		for(ListItr i = list_.begin(); i != list_.end(); i++) {
			switch(i->attr) {
			case EntryAttr::REMOVE:
				cout<<"REMOVE "<<CStr(i->name.c_str())<<endl;
				break;
			case EntryAttr::UPDATE:
				cout<<"UPDATE "<<CStr(i->name.c_str())<<endl;
				break;
			case EntryAttr::ALREADY:
				cout<<"ALREADY "<<CStr(i->name.c_str())<<endl;
				break;
			case EntryAttr::NEW:
				cout<<"NEW "<<CStr(i->name.c_str())<<endl;
				break;
			}
			switch(i->attr) {
			case EntryAttr::REMOVE:
				//i->pos = ret;
				//File::remove(i->pos, align(i->size));
				break;
			case EntryAttr::UPDATE:
				/*cout<<"UPDATE "<<CStr(i->name.c_str())<<endl;
				{
					int diff_size = align(index_[i->name].size) - align(i->size);
					i->pos = ret;
					ret += align(i->size);
					if(diff_size > 0) File::remove(i->pos,  diff_size);
					if(diff_size < 0) File::insert(i->pos, -diff_size);
					File::seek(i->pos);
					{
						File file(i->name.c_str());
						file.extract(*this, 0, file.length());
					}
					int align_size = align(i->size);
					for(int j = 0; j < align_size - i->size; j++) (*this) << '\0';
				}
				break;*/
			case EntryAttr::ALREADY:
			case EntryAttr::NEW:
				{
					i->pos = ret;
					ret += align(i->size);
					File::seek(i->pos);
					{
						File file(i->name.c_str());
						file.extract(*this, 0, file.length());
					}
					int align_size = align(i->size);
					for(int j = 0; j < align_size - i->size; j++) (*this) << '\0';
				}
				break;
			default:
				i->pos = ret;
				ret += align(i->size);
			}
		}
		File::clear();
		assert(ret > 0);
		return ret;
	}

	void ArchiveEditable::commitIndex(int index_size)
	{
#define _SWAP_(n)	(((n)&0xFF000000)>>24|((n)&0xFF0000)>>8|((n)&0xFF00)<<8|((n)&0xFF)<<24)
		index_size_ = index_size;
		File::seek(0);
		(*this) << (int)_SWAP_('GCAR') << index_size_ << flags_;
		for(ListItr i = list_.begin(); i != list_.end(); i++) {
			if(i->attr != EntryAttr::REMOVE) {
				(*this) << i->name.c_str() << i->time << i->size << i->pos;
				PRNN("write "<<i->name<<" "<<ctime(&i->time)<<"\tsize "<<i->size<<", pos "<<i->pos);
			}
		}
		File::clear();
#undef _SWAP_
	}

	void ArchiveEditable::commit()
	{
		assert(File::good());
		bool need_update = false;
		int old_file_size = length();
		int index_size = calcIndexLen();

		if(index_size != index_size_) need_update = true;
		else {
			for(ListItr i = list_.begin(); i != list_.end(); ++i) {
				if(i->attr != EntryAttr::ALREADY) {
					need_update = true;
					break;
				}
			}
		}

		if(need_update) {
			int cur_file_size = commitEntrys(index_size);
			assert(File::good());
			commitIndex(index_size);
			assert(File::good());
			// �t�@�C���T�C�Y�ύX
			if(old_file_size > cur_file_size) truncate(cur_file_size); // �k�񂾏ꍇ�͏k�߂�B�傫���Ȃ����ꍇ�͓��ɑ���͂���Ȃ�
			assert(File::good());
			File::flush();
			assert(File::good());
			index_.clear();
			list_.clear();
			if(readIndex()) setUpList();
			else close();
		}
		else {
			list_.clear();
		}
	}

	void ArchiveEditable::printList(std::ostream &os)
	{
		for(ListItr i = list_.begin(); i != list_.end(); ++i) {
			char *attr;
			switch(i->attr) {
			case EntryAttr::ALREADY: attr = "ALREADY"; break;
			case EntryAttr::NEW: attr = "NEW"; break;
			case EntryAttr::REMOVE: attr = "REMOVE"; break;
			case EntryAttr::UPDATE: attr = "UPDATE"; break;
			}
			os<<CStr(i->name.c_str())<<" "<<ctime(&i->time)<<"\tsize "<<i->size<<", pos "<<i->pos<<", attr "<<attr<<endl;
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