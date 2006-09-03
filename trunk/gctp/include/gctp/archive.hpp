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
#include <gctp/cstr.hpp>
#include <map>
#include <list>

namespace gctp {
	
	/** アーカイブ用ファイルバッファクラス
	 *
	 * STLPortにはファイル記述子、またはファイルポインタを取得する非標準のメンバ関数があるが、非標準なので(VC付属のSTLには無い)、
	 * 自前で用意する。
	 * truncateの実装のためにファイル記述子の取得が出来ないと困る。
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

	/** アーカイブ用ファイルストリームクラス
	 *
	 * まだmove_buf_size_のsetter、getterを用意していない。スレッドセーフにするには、
	 * moveblock、<< File 中にロックしなきゃいけないから。
	 *
	 * fstreamなどと比べて文字列の扱いが違う。読み取り時は改行までではなくヌル文字まで、
	 * 書き込み時はヌル文字も書き込む(fstreamはヌル文字は書かない)。
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

		/// 読み込み
		File &read(void *d, int n);
		/// 書き込み
		File &write(const void *s, int n);
		/// ファイル長
		int length() const;
		/// 終端に達したか？
		bool eof() const;

		/// シーク
		File &seek(std::streamoff off, std::ios::seek_dir dir = std::ios::beg);
		/// ポインタ位置問い合わせ
		std::streamoff tell() const;

		/// バッファフラッシュ
		File &flush();
		/// サイズ変更
		File &truncate(int newsize);
		/// サイズ変更（相対指定）
		File &relativeTruncate(int addsize);
		/// obstreamに指定位置から指定サイズ分書き込み
		File &extract(obstream &dst, long pos, long size);
		/// ostreamに指定位置から指定サイズ分書き込み
		File &extract(std::ostream &dst, long pos, long size);
		/// 指定位置を読み込んでBufferで返す
		Buffer load(long pos = -1, long size = -1);
		/// ブロック移動
		File &moveBlock(int dst_pos, int src_pos, int size);
		/// 空白挿入
		File &insert(int ins_pos, int size);
		/// 削除
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
		typedef std::map<CStr, ArchiveEntry> Index;
		/// Indexのiterator
		typedef Index::iterator IndexItr;

		/// デフォルトコンストラクタ
		Archive() : index_size_(0) {}
		/// アーカイブファイル名を指定して開く
		explicit Archive(const char *fn);
		/// アーカイブファイル名を指定して開く
		void open(const char *fn);
		/// 開いているか？
		bool is_open() {return File::is_open();}
		/// 閉じる
		void close() {File::close();}
		/// アーカイブファイルの長さ
		int length() {return File::length();}
		/// エントリ取得
		ArchiveEntry *get(const char *fn);
		/// エントリ先頭にシーク
		Archive &seek(const ArchiveEntry *entry);
		/// エントリ先頭にシーク
		Archive &seek(const char *fn);
		/// 内部のファイルを別ファイルに抽出
		Archive &extract(const char *fn, const char *entryname);
		/// 内部のファイルを別ファイルに抽出
		Archive &extract(const char *fn, const ArchiveEntry *entry);
		/// すべての内部ファイルを別ファイルに抽出
		Archive &extract();
		/// 読み込み
		Archive &read(void *dst, const char *fn);
		/// 読み込み
		Archive &read(void *dst, const ArchiveEntry *entry);
		/// 読み込み
		Archive &read(void *dst, const int size);
		/// 内部ファイルをBufferで返す
		Buffer load(const char *fn);
		/// 内部ファイルをBufferで返す
		Buffer load(const ArchiveEntry *entry);
		/// 指定サイズ読み込んでBufferで返す
		Buffer load(const int size);
		/// 索引をコンソールに出力
		void printIndex(std::ostream &os);
		
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
			CStr name;
			/// 属性
			Attr attr;

			/// デフォルトコンストラクタ
			EntryAttr() : attr(ALREADY) {}
			/// コンストラクタ
			EntryAttr(const char *name, ArchiveEntry &entry, Attr attr = ALREADY) : ArchiveEntry(entry), name(name), attr(attr) {}
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
		explicit ArchiveEditable(const char *fn);
		/// アライメントを指定
		void setAlign(int n) { alignment_ = index_page_size_ = n; }
		/// 開く
		void open(const char *fn);
		/// エントリ追加
		void add(const char *fn);
		/// 更新日付を無視し、強制再構築
		void forceUpdate(const char *fn);
		/// 再構築
		void update();
		/// エントリ削除
		void remove(const char *fn);
		/// 確定
		void commit();
		/// 編集中リストを出力
		void printList(std::ostream &os);
	protected:
		List list_; // 書き込みのときのリスト
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