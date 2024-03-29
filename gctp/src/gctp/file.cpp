/** @file
 * GameCatapult ファイルクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/12/30
 * $Id: archive.cpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/file.hpp>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <windows.h>
#include <assert.h>
#include <gctp/dbgout.hpp>
#ifdef __ee__
#include <sys/types.h>
#endif

// UNIX系だったら
/*
#define _open	::open
#define _close	::close
#define _O_RDWR O_RDWR
	って感じかなぁ
*/

using namespace std;

namespace gctp {

	////////////////////////
	// FileBuf
	//
	void FileBuf::open(const _TCHAR *fname, Mode mode)
	{
		close();
		if(mode == READ) {
			fd_ = _topen(fname, _O_RDONLY|_O_BINARY);
		}
		else if(mode == WRITE) {
			fd_ = _topen(fname, _O_RDWR|_O_CREAT|_O_BINARY, _S_IREAD|_S_IWRITE);
		}
		else if(mode == NEW) {
			fd_ = _topen(fname, _O_RDWR|_O_CREAT|_O_TRUNC|_O_BINARY, _S_IREAD|_S_IWRITE);
		}
		mode_ = mode;
	}

	void FileBuf::open()
	{
		close();
		mode_ = NEW;
		_TCHAR fname[_MAX_PATH];
		GetTempPath(_MAX_PATH, fname);
		GetTempFileName(fname, _T("tmp"), 0, fname);
		fd_ = _topen(fname, _O_RDWR|_O_CREAT|_O_TEMPORARY|_O_BINARY, _S_IREAD|_S_IWRITE);
	}

	void FileBuf::close()
	{
		if(fd_ != -1) _close(fd_); fd_ = -1;
	}

	FileBuf::pos_type FileBuf::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode)
	{
		int idir;
		if(dir == std::ios::cur) idir = SEEK_CUR;
		else if(dir == std::ios::end) idir = SEEK_END;
		else idir = SEEK_SET;
		return _lseek(fd_, off, idir);
	}

	FileBuf::pos_type FileBuf::seekpos(pos_type pos, std::ios_base::openmode mode)
	{
		return _lseek(fd_, pos, SEEK_SET);
	}

	std::streamsize FileBuf::xsgetn(char_type *s, std::streamsize n)
	{
		return _read(fd_, s, n);
	}

	std::streamsize FileBuf::xsputn(const char_type *s, std::streamsize n)
	{
		return _write(fd_, s, n);
	}

	FileBuf::int_type FileBuf::overflow( int_type nChar )
	{
		unsigned char c = 0xFF&nChar;
		return _write(fd_, &c, 1);
	}

	FileBuf::int_type FileBuf::uflow()
	{
		unsigned char c;
		if(_read(fd_, &c, 1) == 0) {
			return std::char_traits<char>::eof();
		}
		return (int_type)c;
	}

	FileBuf::int_type FileBuf::pbackfail( int_type nChar )
	{
		_lseek(fd_, -1, SEEK_CUR);
		return nChar;
	}

	FileBuf::int_type FileBuf::underflow()
	{
		return pbackfail( uflow() );
	}

	FileBuf::int_type FileBuf::sync()
	{
		if(fd_ == -1) return -1;
		if(mode_ == READ) return 0;
		return _commit(fd_);
	}









	////////////////////////
	// File
	//

	int File::length() const
	{
		return _filelength(filebuf_.fd());
	}

	File &File::truncate(int newsize)
	{
		flush();
		_chsize(filebuf_.fd(), newsize);
		return *this;
	}
	
	File &File::relativeTruncate(int addsize)
	{
		return truncate(length() + addsize);
	}

	File& File::extract(obstream &dst, long pos, long size)
	{
		const int max = move_buf_size_;
		seekg(pos);
		if(size > max) {
			int comp_len = 0;
			char *buf = new char[max];
			while(comp_len < size) {
				if((size - comp_len) >= max) {
					read(buf, max);
					dst.write(buf, max);
					comp_len += max;
				}
				else {
					read(buf, size - comp_len);
					dst.write(buf, size - comp_len);
					comp_len = size;
				}
			}
			delete [] buf;
		}
		else {
			char *buf = new char[size];
			read(buf, size);
			dst.write(buf, size);
			delete [] buf;
		}
		return *this;
	}

	File& File::extract(std::ostream &dst, long pos, long size)
	{
		const int max = move_buf_size_;
		seekg(pos);
		if(size > max) {
			int comp_len = 0;
			char *buf = new char[max];
			while(comp_len < size) {
				if((size - comp_len) >= max) {
					read(buf, max);
					dst.write(buf, max);
					comp_len += max;
				}
				else {
					read(buf, size - comp_len);
					dst.write(buf, size - comp_len);
					comp_len = size;
				}
			}
			delete [] buf;
		}
		else {
			char *buf = new char[size];
			read(buf, size);
			dst.write(buf, size);
			delete [] buf;
		}
		return *this;
	}
	
	/** 読み出してBufferで返す
	 *
	 * @param pos 読み出し開始位置。-1を指定すると、シークせずに現在のカーソルから開始。
	 * @param size 読み出しサイズ。-1を指定すると、ファイル終端まで。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/06/30 2:53:46
	 */
	BufferPtr File::load(long pos, long size)
	{
		if(size == -1) {
			size = length();
			if(pos != -1) size -= pos;
			else size -= tellg();
		}
		if(size > 0) {
			BufferPtr ret = new Buffer(size);
			if(pos != -1) seekg(pos);
			read(ret->buf(), size);
			return ret;
		}
		return BufferPtr();
	}

} // namespace gctp
