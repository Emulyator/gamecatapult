/** @file
 * GameCatapultアーカイブファイルクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/12/30
 * $Id: archive.cpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/fileserver.hpp>
#include <gctp/archive.hpp>
#include <gctp/zipfile.hpp>
#include <gctp/turi.hpp>
#include <process.h> //  for _beginthread, _endthread
#include <sstream>

using namespace std;

namespace gctp {

	class FileServer::Volume : public Object {
	public:
		Volume(const _TCHAR *volume_name) : name_(volume_name), priority_(0) {}
		virtual int find(const _TCHAR *fname) = 0;
		virtual int read(const _TCHAR *fname, Buffer &buffer) = 0;

		static bool compare(const Pointer<Volume> &lhs, const Pointer<Volume> &rhs)
		{
			if(lhs && rhs) return lhs->priority_ > rhs->priority_;
			return lhs > rhs;
		}

		TCStr name_;
		int priority_;
	};

	namespace {

		// ネイティブファイルシステム
		class NativeFS : public FileServer::Volume {
		public:
			static bool isExist(const _TCHAR *volume_name)
			{
				DWORD attr = ::GetFileAttributes(volume_name);
				if(attr != -1 && attr | FILE_ATTRIBUTE_DIRECTORY) {
					return true;
				}
				return false;
			}
			NativeFS(const _TCHAR *volume_name) : FileServer::Volume(volume_name)
			{
			}
			virtual int find(const _TCHAR *fname)
			{
				std::basic_stringstream<_TCHAR> str;
				str << name_.c_str() << "/" << fname;
				gctp::File f(str.str().c_str());
				if(f.is_open()) {
					return f.length();
				}
				return -1;
			}
			virtual int read(const _TCHAR *fname, Buffer &buffer)
			{
				std::basic_stringstream<_TCHAR> str;
				str << name_.c_str() << "/" << fname;
				gctp::File f(str.str().c_str());
				if(f.is_open()) {
					f.read(buffer.buf(), f.length());
					return f.length();
				}
				return -1;
			}
		};

		// gar
		class Gar : public FileServer::Volume {
		public:
			static bool isExist(const _TCHAR *volume_name)
			{
				DWORD attr = ::GetFileAttributes(volume_name);
				if(attr != -1 && !(attr | FILE_ATTRIBUTE_DIRECTORY)) {
					return true;
				}
				std::basic_stringstream<_TCHAR> str;
				str << volume_name << ".gar";
				attr = ::GetFileAttributes(str.str().c_str());
				if(attr != -1 && !(attr | FILE_ATTRIBUTE_DIRECTORY)) {
					return true;
				}
				return false;
			}
			Gar(const _TCHAR *volume_name) : FileServer::Volume(volume_name)
			{
				gar_.open(volume_name);
				if(!gar_.is_open()) {
					std::basic_stringstream<_TCHAR> str;
					str << volume_name << ".gar";
					gar_.open(str.str().c_str());
				}
			}
			virtual int find(const _TCHAR *fname)
			{
				ArchiveEntry *entry = gar_.get(fname);
				if(entry) return entry->size;
				return -1;
			}
			virtual int read(const _TCHAR *fname, Buffer &buffer)
			{
				ArchiveEntry *entry = gar_.get(fname);
				if(entry) {
					gar_.read(buffer.buf(), entry);
					return entry->size;
				}
				return -1;
			}
		private:
			Archive gar_;
		};

#ifdef GCTP_USE_ZLIB
		// zip
		class Zip : public FileServer::Volume {
		public:
			static bool isExist(const _TCHAR *volume_name)
			{
				DWORD attr = ::GetFileAttributes(volume_name);
				if(attr != -1 && !(attr | FILE_ATTRIBUTE_DIRECTORY)) {
					return true;
				}
				std::basic_stringstream<_TCHAR> str;
				str << volume_name << ".zip";
				attr = ::GetFileAttributes(str.str().c_str());
				if(attr != -1 && !(attr | FILE_ATTRIBUTE_DIRECTORY)) {
					return true;
				}
				return false;
			}
			Zip(const _TCHAR *volume_name) : FileServer::Volume(volume_name)
			{
				if(!zip_.open(volume_name)) {
					std::basic_stringstream<_TCHAR> str;
					str << volume_name << ".zip";
					zip_.open(str.str().c_str());
				}
			}
			virtual int find(const _TCHAR *fname)
			{
				if(zip_.seekFile(fname)) {
					return zip_.getFileSize();
				}
				return -1;
			}
			virtual int read(const _TCHAR *fname, Buffer &buffer)
			{
				if(zip_.seekFile(fname)) {
					zip_.openFile();
					int ret = zip_.readFile(buffer.buf(), zip_.getFileSize());
					zip_.closeFile();
					return ret;
				}
				return -1;
			}
		private:
			ZipFile zip_;
		};
#endif // GCTP_USE_ZLIB

	}

	class FileServer::Thread {
	public:
		Thread(FileServer *fs) : thread_(0), id_(0)
		{
			thread_ = (HANDLE)_beginthreadex( NULL, 0, &threadfunc, fs, 0, (unsigned int *)&id_ );
		}
		~Thread()
		{
			::PostThreadMessage( id_, WM_QUIT, 0, 0 );
			::WaitForSingleObject( thread_, INFINITE );
			::CloseHandle( thread_ );
		}
	private:
		static unsigned int __stdcall threadfunc( void* arg )
		{
			FileServer *fs = (FileServer *)arg;
			fs->service();
			_endthreadex( 0 );
			return 0;
		}
		HANDLE thread_;
		DWORD  id_;
	};

	FileServer::FileServer() : thread_(0)
	{
		//mount(_T("."), NATIVE);
	}

	FileServer::~FileServer()
	{
		if(thread_) delete thread_;
	}

	bool FileServer::mount(const _TCHAR *path, ArchiveType type)
	{
		for(PointerList<Volume>::iterator i = volume_list_.begin(); i != volume_list_.end(); i++) {
			if(*i && (*i)->name_ == path) return false;
		}
		if(type == AUTO) {
			if(NativeFS::isExist(path)) {
				type = NATIVE;
			}
			else if(Gar::isExist(path)) {
				type = GAR;
			}
#ifdef GCTP_USE_ZLIB
			else if(Zip::isExist(path)) {
				type = ZIP;
			}
#endif // GCTP_USE_ZLIB
		}
		switch(type) {
		case NATIVE:
			volume_list_.push_back(new NativeFS(path));
			break;
		case GAR:
			volume_list_.push_back(new Gar(path));
			break;
#ifdef GCTP_USE_ZLIB
		case ZIP:
			volume_list_.push_back(new Zip(path));
			break;
#endif // GCTP_USE_ZLIB
		default:
			return false;
		}
		stable_sort(volume_list_.begin(), volume_list_.end(), Volume::compare);
		return false;
	}
	
	bool FileServer::unmount(const _TCHAR *path)
	{
		PointerList<Volume>::iterator i;
		for(i = volume_list_.begin(); i != volume_list_.end(); i++) {
			if(*i && (*i)->name_ == path) break;
		}
		if(i != volume_list_.end()) {
			volume_list_.erase(i);
			return true;
		}
		return false;
	}
	
	void FileServer::setPriority(const _TCHAR *archive_name, int priority)
	{
		for(PointerList<Volume>::iterator i = volume_list_.begin(); i != volume_list_.end(); i++) {
			if(*i && (*i)->name_ == archive_name) {
				(*i)->priority_ = priority;
				return;
			}
		}
		stable_sort(volume_list_.begin(), volume_list_.end(), Volume::compare);
	}
	
	int FileServer::getPriority(const _TCHAR *archive_name)
	{
		for(PointerList<Volume>::iterator i = volume_list_.begin(); i != volume_list_.end(); i++) {
			if(*i && (*i)->name_ == archive_name) {
				return (*i)->priority_;
			}
		}
		return 0;
	}

	BufferPtr FileServer::getFile(const _TCHAR *name)
	{
		for(PointerList<Volume>::iterator i = volume_list_.begin(); i != volume_list_.end(); i++) {
			if(*i) {
				int s = (*i)->find(name);
				if(s >= 0) {
					BufferPtr ret = new Buffer(s);
					if((*i)->read(name, *ret) >= 0) {
						return ret;
					}
				}
			}
		}
		GCTP_TRACE(_T("要求されたファイル'")<<name<<_T("'はマウントされているボリュームの中に見つかりませんでした。"));
		return BufferPtr();
	}
	
	AsyncBufferPtr FileServer::getFileAsync(const _TCHAR *name)
	{
		if(!thread_) thread_ = new Thread(this);
		GCTP_TRACE(_T("要求されたファイル'")<<name<<_T("'はマウントされているボリュームの中に見つかりませんでした。"));
		return AsyncBufferPtr();
	}

	void FileServer::service()
	{
	}
	
	bool FileServer::busy()
	{
		return true;
	}

	FileServer &FileServer::getInstance()
	{
		static FileServer instance;
		return instance;
	}

} // namespace gctp
