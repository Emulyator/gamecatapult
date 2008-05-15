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
#include <gctp/cryptfilter.hpp>
#include <set>

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
		TCStr   id;
		time_t	time;
		bool	ziped;
		long	size;
		long	pos;

		ArchiveEntry() {}
		explicit ArchiveEntry(const _TCHAR *fn) : id(fn) {}
		bool operator<(const ArchiveEntry &rhs) const
		{
			return id < rhs.id;
		}
		bool operator==(const ArchiveEntry &rhs) const
		{
			return id == rhs.id;
		}
	};

	template<class E, class T> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, const ArchiveEntry &ae)
	{
		char ts[26];
		ctime_s(ts, 26, &ae.time);
		ts[24] = '\0';
		return os<<ae.id.c_str()<<" "<<ts<<"\tsize "<<ae.size<<", offset "<<ae.pos;
	}

	class Archive;

	/** �A�[�J�C�u�t�@�C���N���X
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 * $Id: archive.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class ArchiveReader : protected File
	{
	public:
		/// �t���O
		enum Flags {
			FLAG_CRYPTED = 1,
		};

		/// �f�t�H���g�R���X�g���N�^
		ArchiveReader() : flags_(0), filter_(0), crypt_(0) {}
		/// �A�[�J�C�u�t�@�C�������w�肵�ĊJ��
		explicit ArchiveReader(const _TCHAR *fn, const char *key = 0) : flags_(0), filter_(0), crypt_(0), iv_(0)
		{
			setKey(key);
			open(fn);
		}
		~ArchiveReader()
		{
			if(filter_) {
				rdbuf(filebuf());
				delete filter_;
				delete crypt_;
				delete iv_;
			}
		}

		/// �A�[�J�C�u�t�@�C�������w�肵�ĊJ��
		void open(const _TCHAR *fn) { File::open(fn); }
		/// �J���Ă��邩�H
		bool isOpen() { return File::is_open(); }
		/// �A�[�J�C�u�����
		void close() { File::close(); }
		/// �A�[�J�C�u�t�@�C���̒���
		int length() { return File::length(); }
		/// �G���g���擪�ɃV�[�N
		ArchiveReader &seek(const ArchiveEntry *entry);
		/// �����̃t�@�C����ʃt�@�C���ɒ��o
		ArchiveReader &extract(const _TCHAR *fn, const ArchiveEntry *entry);
		/// �ǂݍ���
		ArchiveReader &read(void *dst, const ArchiveEntry *entry);
		/// �ǂݍ���
		ArchiveReader &read(void *dst, const int size);
		/// �����t�@�C����Buffer�ŕԂ�
		BufferPtr load(const ArchiveEntry *entry);
		/// �w��T�C�Y�ǂݍ����Buffer�ŕԂ�
		BufferPtr load(const int size);
		/// ��ƃo�b�t�@�T�C�Y�ݒ�
		void setWorkBufferSize(int size)
		{
			File::setWorkBufferSize(size);
		}
		/// �Í����L�[���Z�b�g
		void setKey(const char *key);
		/// �Í������R�s�[
		void setKey(const Archive &src);
		/// �t���O @see Flags
		int flags() { return flags_; }

	protected:
		int flags_;
		cbccryptfilter *filter_;
		Crypt *crypt_;
		Crypt::DWord *iv_;
		static int align(int n, int alignment) { return ((n+alignment-1)/alignment)*alignment; }
		void preseek(const ArchiveEntry &entry);
	};

	/** �A�[�J�C�u�t�@�C���N���X
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 * $Id: archive.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class Archive : public ArchiveReader
	{
	public:
		/// �A�[�J�C�u���̃t�@�C�����̍�����\���N���X
		typedef std::set<ArchiveEntry> Index;
		/// Index��iterator
		typedef Index::iterator IndexItr;

		/// �f�t�H���g�R���X�g���N�^
		Archive() : index_size_(0) {}
		/// �A�[�J�C�u�t�@�C�������w�肵�ĊJ��
		explicit Archive(const _TCHAR *fn, const char *key = 0) : ArchiveReader(fn, key), index_size_(0)
		{
			if(!readIndex()) close();
		}

		/// �A�[�J�C�u�t�@�C�������w�肵�ĊJ��
		void open(const _TCHAR *fn);
		/// �G���g���擾
		ArchiveEntry *get(const _TCHAR *fn);
		/// �G���g���擪�ɃV�[�N
		Archive &seek(const _TCHAR *fn);
		/// �����̃t�@�C����ʃt�@�C���ɒ��o
		Archive &extract(const _TCHAR *fn, const _TCHAR *entryname);
		/// ���ׂĂ̓����t�@�C����ʃt�@�C���ɒ��o
		Archive &extract();
		/// �ǂݍ���
		Archive &read(void *dst, const _TCHAR *fn);
		/// �����t�@�C����Buffer�ŕԂ�
		BufferPtr load(const _TCHAR *fn);
		/// �������R���\�[���ɏo��
		void printIndex(std::ostream &os);

		/// �G���g���擪�ɃV�[�N
		Archive &seek(const ArchiveEntry *entry)
		{
			ArchiveReader::seek(entry);
			return *this;
		}
		/// �����̃t�@�C����ʃt�@�C���ɒ��o
		Archive &extract(const _TCHAR *fn, const ArchiveEntry *entry)
		{
			ArchiveReader::extract(fn, entry);
			return *this;
		}
		/// �ǂݍ���
		Archive &read(void *dst, const ArchiveEntry *entry)
		{
			ArchiveReader::read(dst, entry);
			return *this;
		}
		/// �ǂݍ���
		Archive &read(void *dst, const int size)
		{
			ArchiveReader::read(dst, size);
			return *this;
		}

	protected:
		Index index_; // �ǂݏo���̂Ƃ��̍���
		bool readIndex();
		int index_size_; // Index index_�̃T�C�Y�ł͂Ȃ��A�I�[�v���������_�ł̃t�@�C����̃C���f�b�N�X�f�[�^�̃T�C�Y�B
		Crypt::DWord index_iv_;
	};

} // namespace gctp

#endif //_GCTP_ARCHIVE_HPP_