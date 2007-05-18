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
		/// アーカイブ内のファイル情報の索引を表すクラス
		typedef std::map<TCStr, ArchiveEntry> Index;
		/// Indexのiterator
		typedef Index::iterator IndexItr;

		/// デフォルトコンストラクタ
		Archive() : index_size_(0) {}
		/// アーカイブファイル名を指定して開く
		explicit Archive(const _TCHAR *fn);
		/// アーカイブファイル名を指定して開く
		void open(const _TCHAR *fn);
		/// 開いているか？
		bool is_open() {return File::is_open();}
		/// 閉じる
		void close() {File::close();}
		/// アーカイブファイルの長さ
		int length() {return File::length();}
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
		void printIndex(std::basic_ostream<_TCHAR> &os);
		
	protected:
		Index index_; // 読み出しのときの索引
		bool readIndex();
		int index_size_; // Index index_のサイズではなく、オープンした時点でのファイル上のインデックスデータのサイズ。
	};

	/** 編集用アーカイブファイルクラス
	 *
	 * add、removeなどで操作を行った後、commitで実際に反映される。
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 * $Id: archive.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	class ArchiveEditable : public Archive
	{
	public:
		enum {INDEX_PAGE_SIZE = 1024}; // インデックス部は最低1k消費し、1k単位でしか増えない
		class EntryAttr : public ArchiveEntry {
		public:
			enum Attr {ALREADY, NEW, UPDATE, REMOVE};
			/// エントリのパス
			TCStr name;
			/// 属性
			Attr attr;

			/// デフォルトコンストラクタ
			EntryAttr() : attr(ALREADY) {}
			/// コンストラクタ
			EntryAttr(const _TCHAR *name, ArchiveEntry &entry, Attr attr = ALREADY) : ArchiveEntry(entry), name(name), attr(attr) {}
			bool operator==(const EntryAttr &rhs) const { return pos==rhs.pos; }
			bool operator!=(const EntryAttr &rhs) const { return pos!=rhs.pos; }
			bool operator<(const EntryAttr &rhs) const { return pos<rhs.pos; }
			bool operator<=(const EntryAttr &rhs) const { return pos<=rhs.pos; }
			bool operator>(const EntryAttr &rhs) const { return pos>rhs.pos; }
			bool operator>=(const EntryAttr &rhs) const { return pos>=rhs.pos; }
		};
		/// 編集中エントリのリスト
		typedef std::list<EntryAttr> List;
		/// Listのiterator
		typedef List::iterator ListItr;

		/// デフォルトコンストラクタ
		ArchiveEditable() : alignment_(4), index_page_size_(4) {}
		/// コンストラクタ
		explicit ArchiveEditable(const _TCHAR *fn);
		/// アライメントを指定
		void setAlign(int n) { alignment_ = index_page_size_ = n; }
		/// 開く
		void open(const _TCHAR *fn);
		/// エントリ追加
		void add(const _TCHAR *fn);
		/// 更新日付を無視し、強制再構築
		void forceUpdate(const _TCHAR *fn);
		/// 再構築
		void update();
		/// エントリ削除
		void remove(const _TCHAR *fn);
		/// 確定
		void commit();
		/// 編集中リストを出力
		void printList(std::basic_ostream<_TCHAR> &os);
	protected:
		List list_; // 書き込みのときのリスト
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