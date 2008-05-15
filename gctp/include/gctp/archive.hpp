#ifndef _GCTP_ARCHIVE_HPP_
#define _GCTP_ARCHIVE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapultアーカイブファイルクラス
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

	/** アーカイブ内のファイル情報
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

	/** アーカイブファイルクラス
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
		/// フラグ
		enum Flags {
			FLAG_CRYPTED = 1,
		};

		/// デフォルトコンストラクタ
		ArchiveReader() : flags_(0), filter_(0), crypt_(0) {}
		/// アーカイブファイル名を指定して開く
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

		/// アーカイブファイル名を指定して開く
		void open(const _TCHAR *fn) { File::open(fn); }
		/// 開いているか？
		bool isOpen() { return File::is_open(); }
		/// アーカイブを閉じる
		void close() { File::close(); }
		/// アーカイブファイルの長さ
		int length() { return File::length(); }
		/// エントリ先頭にシーク
		ArchiveReader &seek(const ArchiveEntry *entry);
		/// 内部のファイルを別ファイルに抽出
		ArchiveReader &extract(const _TCHAR *fn, const ArchiveEntry *entry);
		/// 読み込み
		ArchiveReader &read(void *dst, const ArchiveEntry *entry);
		/// 読み込み
		ArchiveReader &read(void *dst, const int size);
		/// 内部ファイルをBufferで返す
		BufferPtr load(const ArchiveEntry *entry);
		/// 指定サイズ読み込んでBufferで返す
		BufferPtr load(const int size);
		/// 作業バッファサイズ設定
		void setWorkBufferSize(int size)
		{
			File::setWorkBufferSize(size);
		}
		/// 暗号化キーをセット
		void setKey(const char *key);
		/// 暗号情報をコピー
		void setKey(const Archive &src);
		/// フラグ @see Flags
		int flags() { return flags_; }

	protected:
		int flags_;
		cbccryptfilter *filter_;
		Crypt *crypt_;
		Crypt::DWord *iv_;
		static int align(int n, int alignment) { return ((n+alignment-1)/alignment)*alignment; }
		void preseek(const ArchiveEntry &entry);
	};

	/** アーカイブファイルクラス
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
		/// アーカイブ内のファイル情報の索引を表すクラス
		typedef std::set<ArchiveEntry> Index;
		/// Indexのiterator
		typedef Index::iterator IndexItr;

		/// デフォルトコンストラクタ
		Archive() : index_size_(0) {}
		/// アーカイブファイル名を指定して開く
		explicit Archive(const _TCHAR *fn, const char *key = 0) : ArchiveReader(fn, key), index_size_(0)
		{
			if(!readIndex()) close();
		}

		/// アーカイブファイル名を指定して開く
		void open(const _TCHAR *fn);
		/// エントリ取得
		ArchiveEntry *get(const _TCHAR *fn);
		/// エントリ先頭にシーク
		Archive &seek(const _TCHAR *fn);
		/// 内部のファイルを別ファイルに抽出
		Archive &extract(const _TCHAR *fn, const _TCHAR *entryname);
		/// すべての内部ファイルを別ファイルに抽出
		Archive &extract();
		/// 読み込み
		Archive &read(void *dst, const _TCHAR *fn);
		/// 内部ファイルをBufferで返す
		BufferPtr load(const _TCHAR *fn);
		/// 索引をコンソールに出力
		void printIndex(std::ostream &os);

		/// エントリ先頭にシーク
		Archive &seek(const ArchiveEntry *entry)
		{
			ArchiveReader::seek(entry);
			return *this;
		}
		/// 内部のファイルを別ファイルに抽出
		Archive &extract(const _TCHAR *fn, const ArchiveEntry *entry)
		{
			ArchiveReader::extract(fn, entry);
			return *this;
		}
		/// 読み込み
		Archive &read(void *dst, const ArchiveEntry *entry)
		{
			ArchiveReader::read(dst, entry);
			return *this;
		}
		/// 読み込み
		Archive &read(void *dst, const int size)
		{
			ArchiveReader::read(dst, size);
			return *this;
		}

	protected:
		Index index_; // 読み出しのときの索引
		bool readIndex();
		int index_size_; // Index index_のサイズではなく、オープンした時点でのファイル上のインデックスデータのサイズ。
		Crypt::DWord index_iv_;
	};

} // namespace gctp

#endif //_GCTP_ARCHIVE_HPP_