#ifndef _GCTP_FILE_HPP_
#define _GCTP_FILE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ファイルクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/05 15:59:05
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/iobstream.hpp>
#include <gctp/buffer.hpp>
#include <gctp/tcstr.hpp>
#include <tchar.h>

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
		struct Temporary {};
		enum Mode {NEW, READ, WRITE};
		FileBuf() : fd_(-1) {}
		explicit FileBuf(Temporary) : fd_(-1) { open(); }
		explicit FileBuf(const _TCHAR *fname, Mode mode = READ) : fd_(-1)
		{ open(fname, mode); }
		virtual ~FileBuf() { sync(); close(); }
		bool is_open() const
		{ return (fd_!=-1)?true:false; }
		Mode mode()
		{ return mode_; }
		void open(const _TCHAR *fname, Mode mode = READ);
		void open();
		void close();

		int fd() const { return fd_; }

	protected:
		virtual pos_type seekoff(off_type, std::ios_base::seekdir, std::ios_base::openmode = std::ios_base::in | std::ios_base::out);
		virtual pos_type seekpos(pos_type, std::ios_base::openmode = std::ios_base::in | std::ios_base::out);

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
		struct Temporary {};
		enum {DEFAULT_MOVE_BUF_SIZE = 1024*1024};
		enum Mode {NEW, READ, WRITE};
		enum {BEG, CUR, END};
		File() : iobstream(&filebuf_), move_buf_size_(DEFAULT_MOVE_BUF_SIZE) {}
		explicit File(Temporary) : iobstream(&filebuf_), move_buf_size_(DEFAULT_MOVE_BUF_SIZE) { open(); }
		explicit File(const _TCHAR *fname, Mode mode = READ) : iobstream(&filebuf_), move_buf_size_(DEFAULT_MOVE_BUF_SIZE)
		{ open(fname, mode); }
		virtual ~File() {flush(); close();}
		bool is_open() const
		{return filebuf_.is_open();}
		Mode mode()
		{return (Mode)filebuf_.mode();}
		void open(const _TCHAR *fname, Mode mode = READ)
		{ filebuf_.open(fname, (FileBuf::Mode)mode); }
		void open()
		{ filebuf_.open(); }
		void close()
		{ filebuf_.close(); }

		/// ファイル長
		int length() const;

		/// サイズ変更
		File &truncate(int newsize);
		/// サイズ変更（相対指定）
		File &relativeTruncate(int addsize);
		/// obstreamに指定位置から指定サイズ分書き込み
		File &extract(obstream &dst, long pos, long size);
		/// ostreamに指定位置から指定サイズ分書き込み
		File &extract(std::ostream &dst, long pos, long size);
		/// 指定位置を読み込んでBufferで返す
		BufferPtr load(long pos = -1, long size = -1);
		/// 作業バッファサイズ設定
		void setWorkBufferSize(int size)
		{
			move_buf_size_ = size;
		}
		/// ファイルバッファ
		FileBuf *filebuf() { return &filebuf_; }

	private:
		FileBuf filebuf_;

		const File &operator=(const File &src);
		File(const File &src);
		int move_buf_size_;
# ifdef __ee__
		CStr filename_;
# endif
	};

} // namespace gctp

#endif //_GCTP_FILE_HPP_
