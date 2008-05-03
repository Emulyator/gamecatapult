#ifndef _GCTP_ZIPFILE_HPP_
#define _GCTP_ZIPFILE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * zipファイルバッファ・ストリームフィルタークラスヘッダファイル
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 23:00:42
 */
#ifdef GCTP_USE_ZLIB
#include <gctp/buffer.hpp>

namespace gctp {

	class ZipFileImpl;
	/// 読み込みのみ
	class ZipFile {
	public:
		ZipFile();
		~ZipFile();
		
		bool open(const _TCHAR *zipname);
		void close();
		bool seekFile(const _TCHAR *fname);
		int getFileSize() const;
		int openFile();
		void closeFile();
		int readFile(void *b, size_t s);
		Buffer getFileBlock(const _TCHAR *fname);
		
	private:
		ZipFileImpl *impl_;
	};

}
#endif // GCTP_USE_ZLIB

#endif // _GCTP_ZIPFILE_HPP_
