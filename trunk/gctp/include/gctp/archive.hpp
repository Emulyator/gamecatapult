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
#include <gctp/cstr.hpp>
#include <map>
#include <list>

namespace gctp {
	
	/** �A�[�J�C�u�p�t�@�C���o�b�t�@�N���X
	 *
	 * STLPort�ɂ̓t�@�C���L�q�q�A�܂��̓t�@�C���|�C���^���擾�����W���̃����o�֐������邪�A��W���Ȃ̂�(VC�t����STL�ɂ͖���)�A
	 * ���O�ŗp�ӂ���B
	 * truncate�̎����̂��߂Ƀt�@�C���L�q�q�̎擾���o���Ȃ��ƍ���B
	 * @author SAM (T&GG, Org.) <sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 *
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class FileBuf : public std::streambuf {
	public:
		enum Mode {NEW, READ, WRITE};
		FileBuf(bool tmp = false) : fd_(-1) { if(tmp) open(); }
		explicit FileBuf(const char *fname, Mode mode = READ) : fd_(-1)
		{ open(fname, mode); }
		virtual ~FileBuf() { sync(); close(); }
		bool is_open() const
		{ return (fd_!=-1)?true:false; }
		Mode mode()
		{ return mode_; }
		void open(const char *fname, Mode mode = READ);
		void open();
		void close();

		int fd() const { return fd_; }

	protected:
		virtual std::streampos seekoff(std::streamoff off, std::ios::seek_dir dir, std::ios_base::openmode mode = std::ios::in | std::ios::out);
		virtual std::streampos seekpos(std::streampos pos, std::ios_base::openmode mode = std::ios::in | std::ios::out);

		virtual std::streamsize xsgetn(char_type *s, std::streamsize n);
		virtual std::streamsize xsputn(const char_type *s, std::streamsize n);

		virtual int_type overflow( int_type nChar = std::char_traits<char>::eof() );
		virtual int_type uflow();
		virtual int_type underflow();
		virtual int_type pbackfail( int_type nChar = std::char_traits<char>::eof() );
		virtual int_type sync();

	private:
		int fd_;
		Mode mode_;
	};

	/** �A�[�J�C�u�p�t�@�C���X�g���[���N���X
	 *
	 * �܂�move_buf_size_��setter�Agetter��p�ӂ��Ă��Ȃ��B�X���b�h�Z�[�t�ɂ���ɂ́A
	 * moveblock�A<< File ���Ƀ��b�N���Ȃ��Ⴂ���Ȃ�����B
	 *
	 * fstream�ȂǂƔ�ׂĕ�����̈������Ⴄ�B�ǂݎ�莞�͉��s�܂łł͂Ȃ��k�������܂ŁA
	 * �������ݎ��̓k����������������(fstream�̓k�������͏����Ȃ�)�B
	 * @author SAM (T&GG, Org.) <sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 *
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class File : public iobstream {
	public:
		enum {DEFAULT_MOVE_BUF_SIZE = 1024*1024};
		enum Mode {NEW, READ, WRITE};
		enum {BEG, CUR, END};
		File(bool tmp = false) : iobstream(&_M_buf), move_buf_size_(DEFAULT_MOVE_BUF_SIZE) { if(tmp) open(); }
		explicit File(const char *fname, Mode mode = READ) : iobstream(&_M_buf), move_buf_size_(DEFAULT_MOVE_BUF_SIZE)
		{ open(fname, mode); }
		virtual ~File() {flush(); close();}
		bool is_open() const
		{return _M_buf.is_open();}
		Mode mode()
		{return (Mode)_M_buf.mode();}
		void open(const char *fname, Mode mode = READ)
		{ _M_buf.open(fname, (FileBuf::Mode)mode); }
		void open()
		{ _M_buf.open(); }
		void close()
		{ _M_buf.close(); }

		/// �ǂݍ���
		File &read(void *d, int n);
		/// ��������
		File &write(const void *s, int n);
		/// �t�@�C����
		int length() const;
		/// �I�[�ɒB�������H
		bool eof() const;

		/// �V�[�N
		File &seek(std::streamoff off, std::ios::seek_dir dir = std::ios::beg);
		/// �|�C���^�ʒu�₢���킹
		std::streamoff tell() const;

		/// �o�b�t�@�t���b�V��
		File &flush();
		/// �T�C�Y�ύX
		File &truncate(int newsize);
		/// �T�C�Y�ύX�i���Ύw��j
		File &relativeTruncate(int addsize);
		/// obstream�Ɏw��ʒu����w��T�C�Y����������
		File &extract(obstream &dst, long pos, long size);
		/// ostream�Ɏw��ʒu����w��T�C�Y����������
		File &extract(std::ostream &dst, long pos, long size);
		/// �w��ʒu��ǂݍ����Buffer�ŕԂ�
		Buffer load(long pos = -1, long size = -1);
		/// �u���b�N�ړ�
		File &moveBlock(int dst_pos, int src_pos, int size);
		/// �󔒑}��
		File &insert(int ins_pos, int size);
		/// �폜
		File &remove(int rmv_pos, int size);

	private:
		FileBuf _M_buf;

		const File &operator=(const File &src);
		File(const File &src);
		int move_buf_size_;
# ifdef __ee__
		CStr filename_;
# endif
	};

	inline obstream &operator<<(obstream &lhs, File &rhs)
	{
		rhs.extract(lhs, 0, rhs.length());
		return lhs;
	}

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
		typedef std::map<CStr, ArchiveEntry> Index;
		/// Index��iterator
		typedef Index::iterator IndexItr;

		/// �f�t�H���g�R���X�g���N�^
		Archive() : index_size_(0) {}
		/// �A�[�J�C�u�t�@�C�������w�肵�ĊJ��
		explicit Archive(const char *fn);
		/// �A�[�J�C�u�t�@�C�������w�肵�ĊJ��
		void open(const char *fn);
		/// �J���Ă��邩�H
		bool is_open() {return File::is_open();}
		/// ����
		void close() {File::close();}
		/// �A�[�J�C�u�t�@�C���̒���
		int length() {return File::length();}
		/// �G���g���擾
		ArchiveEntry *get(const char *fn);
		/// �G���g���擪�ɃV�[�N
		Archive &seek(const ArchiveEntry *entry);
		/// �G���g���擪�ɃV�[�N
		Archive &seek(const char *fn);
		/// �����̃t�@�C����ʃt�@�C���ɒ��o
		Archive &extract(const char *fn, const char *entryname);
		/// �����̃t�@�C����ʃt�@�C���ɒ��o
		Archive &extract(const char *fn, const ArchiveEntry *entry);
		/// ���ׂĂ̓����t�@�C����ʃt�@�C���ɒ��o
		Archive &extract();
		/// �ǂݍ���
		Archive &read(void *dst, const char *fn);
		/// �ǂݍ���
		Archive &read(void *dst, const ArchiveEntry *entry);
		/// �ǂݍ���
		Archive &read(void *dst, const int size);
		/// �����t�@�C����Buffer�ŕԂ�
		Buffer load(const char *fn);
		/// �����t�@�C����Buffer�ŕԂ�
		Buffer load(const ArchiveEntry *entry);
		/// �w��T�C�Y�ǂݍ����Buffer�ŕԂ�
		Buffer load(const int size);
		/// �������R���\�[���ɏo��
		void printIndex(std::ostream &os);
		
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
			CStr name;
			/// ����
			Attr attr;

			/// �f�t�H���g�R���X�g���N�^
			EntryAttr() : attr(ALREADY) {}
			/// �R���X�g���N�^
			EntryAttr(const char *name, ArchiveEntry &entry, Attr attr = ALREADY) : ArchiveEntry(entry), name(name), attr(attr) {}
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
		explicit ArchiveEditable(const char *fn);
		/// �A���C�����g���w��
		void setAlign(int n) { alignment_ = index_page_size_ = n; }
		/// �J��
		void open(const char *fn);
		/// �G���g���ǉ�
		void add(const char *fn);
		/// �X�V���t�𖳎����A�����č\�z
		void forceUpdate(const char *fn);
		/// �č\�z
		void update();
		/// �G���g���폜
		void remove(const char *fn);
		/// �m��
		void commit();
		/// �ҏW�����X�g���o��
		void printList(std::ostream &os);
	protected:
		List list_; // �������݂̂Ƃ��̃��X�g
		void setUpList();
		long calcIndexLen();
		long commitEntrys(int index_size);
		void commitIndex(int index_size);
		ListItr find(const char *fn) {
			LStr wfn(fn);
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