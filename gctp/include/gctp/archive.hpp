#ifndef _GCTP_ARCHIVE_HPP_
#define _GCTP_ARCHIVE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult�A�[�J�C�u�t�@�C���N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/05 15:59:05
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/iobstream.hpp>
#include <gctp/buffer.hpp>
#include <gctp/tcstr.hpp>
#include <gctp/file.hpp>
#include <map>
#include <list>

namespace gctp {

	/** �A�[�J�C�u���̃t�@�C�����
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/01/20 7:09:06
	 * $Id: archive.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	struct ArchiveEntry {
		time_t	time;
		bool	ziped;
		long	size;
		long	pos;
	};

	/** �A�[�J�C�u�t�@�C���N���X
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 * $Id: archive.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class Archive : protected File
	{
	public:
		/// �A�[�J�C�u���̃t�@�C�����̍�����\���N���X
		typedef std::map<TCStr, ArchiveEntry> Index;
		/// Index��iterator
		typedef Index::iterator IndexItr;

		/// �f�t�H���g�R���X�g���N�^
		Archive() : index_size_(0) {}
		/// �A�[�J�C�u�t�@�C�������w�肵�ĊJ��
		explicit Archive(const _TCHAR *fn);
		/// �A�[�J�C�u�t�@�C�������w�肵�ĊJ��
		void open(const _TCHAR *fn);
		/// �J���Ă��邩�H
		bool is_open() {return File::is_open();}
		/// ����
		void close() {File::close();}
		/// �A�[�J�C�u�t�@�C���̒���
		int length() {return File::length();}
		/// �G���g���擾
		ArchiveEntry *get(const _TCHAR *fn);
		/// �G���g���擪�ɃV�[�N
		Archive &seek(const ArchiveEntry *entry);
		/// �G���g���擪�ɃV�[�N
		Archive &seek(const _TCHAR *fn);
		/// �����̃t�@�C����ʃt�@�C���ɒ��o
		Archive &extract(const _TCHAR *fn, const _TCHAR *entryname);
		/// �����̃t�@�C����ʃt�@�C���ɒ��o
		Archive &extract(const _TCHAR *fn, const ArchiveEntry *entry);
		/// ���ׂĂ̓����t�@�C����ʃt�@�C���ɒ��o
		Archive &extract();
		/// �ǂݍ���
		Archive &read(void *dst, const _TCHAR *fn);
		/// �ǂݍ���
		Archive &read(void *dst, const ArchiveEntry *entry);
		/// �ǂݍ���
		Archive &read(void *dst, const int size);
		/// �����t�@�C����Buffer�ŕԂ�
		BufferPtr load(const _TCHAR *fn);
		/// �����t�@�C����Buffer�ŕԂ�
		BufferPtr load(const ArchiveEntry *entry);
		/// �w��T�C�Y�ǂݍ����Buffer�ŕԂ�
		BufferPtr load(const int size);
		/// �������R���\�[���ɏo��
		void printIndex(std::basic_ostream<_TCHAR> &os);
		
	protected:
		Index index_; // �ǂݏo���̂Ƃ��̍���
		bool readIndex();
		int index_size_; // Index index_�̃T�C�Y�ł͂Ȃ��A�I�[�v���������_�ł̃t�@�C����̃C���f�b�N�X�f�[�^�̃T�C�Y�B
	};

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
		enum {INDEX_PAGE_SIZE = 1024}; // �C���f�b�N�X���͍Œ�1k����A1k�P�ʂł��������Ȃ�
		class EntryAttr : public ArchiveEntry {
		public:
			enum Attr {ALREADY, NEW, UPDATE, REMOVE};
			/// �G���g���̃p�X
			TCStr name;
			/// ����
			Attr attr;

			/// �f�t�H���g�R���X�g���N�^
			EntryAttr() : attr(ALREADY) {}
			/// �R���X�g���N�^
			EntryAttr(const _TCHAR *name, ArchiveEntry &entry, Attr attr = ALREADY) : ArchiveEntry(entry), name(name), attr(attr) {}
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
		ArchiveEditable() : alignment_(4), index_page_size_(4) {}
		/// �R���X�g���N�^
		explicit ArchiveEditable(const _TCHAR *fn);
		/// �A���C�����g���w��
		void setAlign(int n) { alignment_ = index_page_size_ = n; }
		/// �J��
		void open(const _TCHAR *fn);
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
		void printList(std::basic_ostream<_TCHAR> &os);
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
				if(i->name == wfn) break;
			}
			return i;
		}
		int alignment_;
		int align(int n)
		{ return ((n + alignment_ - 1)/alignment_)*alignment_; }
		int indexalign(int n)
		{ return ((n + index_page_size_ - 1)/index_page_size_)*index_page_size_; }
	private:
		int index_page_size_;
	};

} // namespace gctp

#endif //_GCTP_ARCHIVE_HPP_