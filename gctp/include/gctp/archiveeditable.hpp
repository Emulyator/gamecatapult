#ifndef _GCTP_ARCHIVEEDITABLE_HPP_
#define _GCTP_ARCHIVEEDITABLE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult�A�[�J�C�u�t�@�C���ҏW�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/05 15:59:05
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/archive.hpp>
#include <list>

namespace gctp {

	/** �ҏW�p�A�[�J�C�u�t�@�C���N���X
	 *
	 * add�Aremove�Ȃǂő�����s������Acommit�Ŏ��ۂɔ��f�����B
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 * $Id: archive.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class ArchiveEditable : public Archive
	{
	public:
		class EntryAttr : public ArchiveEntry {
		public:
			enum Attr {ALREADY, NEW, UPDATE, REMOVE};
			/// ����
			Attr attr;
			/// ��փX�g���[��
			std::istream *strm;

			/// �f�t�H���g�R���X�g���N�^
			EntryAttr() : attr(ALREADY), strm(0) {}
			/// �R���X�g���N�^
			EntryAttr(ArchiveEntry &entry, std::istream *strm, Attr attr = ALREADY) : ArchiveEntry(entry), attr(attr), strm(strm) {}
			/// �R���X�g���N�^
			explicit EntryAttr(ArchiveEntry &entry, Attr attr = ALREADY) : ArchiveEntry(entry), attr(attr), strm(0) {}
			bool operator==(const EntryAttr &rhs) const { return pos==rhs.pos; }
			bool operator!=(const EntryAttr &rhs) const { return pos!=rhs.pos; }
			bool operator<(const EntryAttr &rhs) const { return pos<rhs.pos; }
			bool operator<=(const EntryAttr &rhs) const { return pos<=rhs.pos; }
			bool operator>(const EntryAttr &rhs) const { return pos>rhs.pos; }
			bool operator>=(const EntryAttr &rhs) const { return pos>=rhs.pos; }
		};
		/// �ҏW���G���g���̃��X�g
		typedef std::list<EntryAttr> List;
		/// List��iterator
		typedef List::iterator ListItr;

		/// �f�t�H���g�R���X�g���N�^
		explicit ArchiveEditable(bool crypt = false) : alignment_(crypt ? 8 : 4), crypt_(crypt) {}
		/// �R���X�g���N�^
		ArchiveEditable(const _TCHAR *fn, const char *key, bool crypt = false) : alignment_(crypt ? 8 : 4), crypt_(crypt)
		{
			setKey(key);
			open(fn);
		}
		/// �A���C�����g���w��(0�ɂ��낦���鉺�ʃr�b�g���Ŏw��)
		void setAlign(int bit)
		{
			alignment_ = 1<<bit;
			if(crypt_ && alignment_ < 8) alignment_ = 8;
		}
		/// �J��
		void open(const _TCHAR *fn);
		/// �V�K����
		void openAsNew(const _TCHAR *fn);
		/// �G���g���ǉ�
		void add(const _TCHAR *fn);
		/// �X�V���t�𖳎����A�����č\�z
		void forceUpdate(const _TCHAR *fn);
		/// �č\�z
		void update();
		/// �G���g���폜
		void remove(const _TCHAR *fn);
		/// �m��
		void commit();
		/// �ҏW�����X�g���o��
		void printList(std::ostream &os);
		//void printList(std::basic_ostream<_TCHAR> &os);

	protected:
		List list_; // �������݂̂Ƃ��̃��X�g
		void setUpList();
		long calcIndexLen();
		long commitEntrys(int index_size);
		void commitIndex(int index_size);
		ListItr find(const _TCHAR *fn) {
			TLStr wfn(fn);
			ListItr i;
			for(i = list_.begin(); i != list_.end(); ++i) {
				if(i->id == wfn) break;
			}
			return i;
		}
		int alignment_;
		bool crypt_;
		int align(int n) { return Archive::align(n, alignment_); }
	private:
		int index_page_size_;
	};

} // namespace gctp

#endif // _GCTP_ARCHIVEEDITABLE_HPP_
