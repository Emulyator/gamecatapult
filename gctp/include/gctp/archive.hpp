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
#include <gctp/crypt.hpp>
#include <map>
#include <list>

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
		time_t	time;
		bool	ziped;
		long	size;
		long	pos;
	};

	/** アーカイブファイルクラス
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
		enum Flags {
			FLAG_CRYPTED = 1,
		};

		/// アーカイブ内のファイル情報の索引を表すクラス
		typedef std::map<TCStr, ArchiveEntry> Index;
		/// Indexのiterator
		typedef Index::iterator IndexItr;

		/// デフォルトコンストラクタ
		Archive() : index_size_(0), flags_(0), crypt_(0) {}
		/// アーカイブファイル名を指定して開く
		explicit Archive(const _TCHAR *fn) : index_size_(0), flags_(0), crypt_(0)
		{
			open(fn);
		}
		~Archive();

		/// アーカイブファイル名を指定して開く
		void open(const _TCHAR *fn);
		/// 開いているか？
		bool is_open() { return File::is_open(); }
		/// 閉じる
		void close() { File::close(); }
		/// アーカイブファイルの長さ
		int length() { return File::length(); }
		/// エントリ取得
		ArchiveEntry *get(const _TCHAR *fn);
		/// エントリ先頭にシーク
		Archive &seek(const ArchiveEntry *entry);
		/// エントリ先頭にシーク
		Archive &seek(const _TCHAR *fn);
		/// 内部のファイルを別ファイルに抽出
		Archive &extract(const _TCHAR *fn, const _TCHAR *entryname);
		/// 内部のファイルを別ファイルに抽出
		Archive &extract(const _TCHAR *fn, const ArchiveEntry *entry);
		/// すべての内部ファイルを別ファイルに抽出
		Archive &extract();
		/// 読み込み
		Archive &read(void *dst, const _TCHAR *fn);
		/// 読み込み
		Archive &read(void *dst, const ArchiveEntry *entry);
		/// 読み込み
		Archive &read(void *dst, const int size);
		/// 内部ファイルをBufferで返す
		BufferPtr load(const _TCHAR *fn);
		/// 内部ファイルをBufferで返す
		BufferPtr load(const ArchiveEntry *entry);
		/// 指定サイズ読み込んでBufferで返す
		BufferPtr load(const int size);
		/// 索引をコンソールに出力
		void printIndex(std::ostream &os);
		//void printIndex(std::basic_ostream<_TCHAR> &os);
		/// 作業バッファサイズ設定
		void setWorkBufferSize(int size)
		{
			File::setWorkBufferSize(size);
		}
		/// 暗号化キーをセット
		void setKey(const char *key);

	protected:
		Index index_; // 読み出しのときの索引
		bool readIndex();
		int index_size_; // Index index_のサイズではなく、オープンした時点でのファイル上のインデックスデータのサイズ。
		int flags_;
		Crypt *crypt_;
		static int align(int n, int alignment) { return ((n+alignment-1)/alignment)*alignment; }
	};

} // namespace gctp

#endif //_GCTP_ARCHIVE_HPP_