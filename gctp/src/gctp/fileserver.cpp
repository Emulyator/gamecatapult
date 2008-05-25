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
#include <gctp/app.hpp>
#include <gctp/fileserver.hpp>
#include <gctp/archive.hpp>
#include <gctp/zipfile.hpp>
#include <gctp/turi.hpp>
#include <process.h> //  for _beginthread, _endthread
#include <sstream>
#include <queue>

using namespace std;

namespace gctp {
	
	AbstractFile::~AbstractFile()
	{
	}

	class FileServer::Volume : public Object {
	public:
		Volume(const _TCHAR *volume_name) : name_(volume_name), priority_(0) {}
		virtual int find(const _TCHAR *fname) = 0;
		virtual int read(const _TCHAR *fname, Buffer &buffer) = 0;
		virtual AbstractFilePtr createAbstractFile(const _TCHAR *fname) = 0;

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
				if(attr != -1 && (attr & FILE_ATTRIBUTE_DIRECTORY)) {
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

			class NativeFSFile : public AbstractFile {
			public:
				virtual int size() const
				{
					return file_.length();
				}
				virtual int seek(int pos)
				{
					file_.seekg(pos);
					return file_.tellg();
				}
				virtual int read(void *p, size_t s)
				{
					std::streamoff n = file_.tellg();
					file_.read(p, (int)s);
					return file_.tellg()-n;
				}
				File file_;
			};
			virtual AbstractFilePtr createAbstractFile(const _TCHAR *fname)
			{
				Pointer<NativeFSFile> ret = new NativeFSFile;
				std::basic_stringstream<_TCHAR> str;
				str << name_.c_str() << "/" << fname;
				ret->file_.open(str.str().c_str());
				if(!ret->file_.is_open()) ret = 0;
				return ret;
			}
		};

		// gcar
		class Gcar : public FileServer::Volume {
		public:
			static bool isExistEx(const _TCHAR *volume_name, std::basic_string<_TCHAR> &filename, std::basic_string<_TCHAR> &currentdir)
			{
				TURI volume_uri(volume_name);
				std::basic_string<_TCHAR> ext = volume_uri.extension();
				/*if(ext.empty()) {
					std::basic_stringstream<_TCHAR> str;
					str << volume_name << ".gcar";
					DWORD attr = ::GetFileAttributes(str.str().c_str());
					if(attr != -1 && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
						filename = str.str();
						return true;
					}
				}
				else if(ext == _T("gcar")) {*/
					DWORD attr = ::GetFileAttributes(volume_name);
					if(attr != -1 && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
						filename = volume_name;
						return true;
					}
				//}
				std::basic_string<_TCHAR> path = volume_uri.path();
				if(!path.empty()) {
					if(currentdir.empty()) currentdir = TURI(volume_name).leaf() + _T("/");
					else currentdir = TURI(volume_name).leaf() + _T("/") + currentdir;
					return isExistEx(path.c_str(), filename, currentdir);
				}
				return false;
			}
			static bool isExist(const _TCHAR *volume_name)
			{
				std::basic_string<_TCHAR> filename;
				std::basic_string<_TCHAR> currentdir;
				return isExistEx(volume_name, filename, currentdir);
			}
			Gcar(const _TCHAR *volume_name, const char *key) : FileServer::Volume(volume_name)
			{
				if(isExistEx(volume_name, filename_, currentdir_)) {
					//PRNN("gcar volume_name "<<volume_name);
					//PRNN("gcar filename "<<filename_);
					//PRNN("gcar currentdir "<<currentdir_);
					if(key) gcar_.setKey(key);
					gcar_.open(filename_.c_str());
					assert(gcar_.isOpen());
				}
			}
			virtual int find(const _TCHAR *fname)
			{
				std::basic_string<_TCHAR> filename = currentdir_+fname;
				ArchiveEntry *entry = gcar_.get(filename.c_str());
				if(entry) return entry->size;
				return -1;
			}
			virtual int read(const _TCHAR *fname, Buffer &buffer)
			{
				std::basic_string<_TCHAR> filename = currentdir_+fname;
				ArchiveEntry *entry = gcar_.get(filename.c_str());
				if(entry) {
					gcar_.read(buffer.buf(), entry);
					return entry->size;
				}
				return -1;
			}

			class GcarFile : public AbstractFile {
			public:
				virtual int size() const
				{
					return entry_.size;
				}
				virtual int seek(int pos)
				{
					((File &)file_).seekg(std::min<std::streamoff>(entry_.pos+entry_.size, entry_.pos + pos));
					return ((File &)file_).tellg();
				}
				virtual int read(void *p, size_t s)
				{
					std::streamoff n = ((File &)file_).tellg();
					std::streamoff end = (std::streamoff)(entry_.pos+entry_.size);
					s = std::min<size_t>(std::max<std::streamoff>(end-n,0),s);
					if(s > 0) {
						((File &)file_).read(p, (int)s);
					}
					return (int)s;
				}
				ArchiveEntry entry_;
				ArchiveReader file_;
			};
			virtual AbstractFilePtr createAbstractFile(const _TCHAR *fname)
			{
				std::basic_string<_TCHAR> filename = currentdir_+fname;
				ArchiveEntry *entry = gcar_.get(filename.c_str());
				if(!entry) return 0;
				Pointer<GcarFile> ret = new GcarFile;
				ret->file_.open(filename_.c_str());
				if(!ret->file_.isOpen()) ret = 0;
				else {
					ret->entry_ = *entry;
					ret->file_.setKey(gcar_);
					ret->file_.seek(entry);
				}
				return ret;
			}
		private:
			Archive gcar_;
			std::basic_string<_TCHAR> filename_;
			std::basic_string<_TCHAR> currentdir_;
		};

#ifdef GCTP_USE_ZLIB
		// zip
		class Zip : public FileServer::Volume {
		public:
			static bool isExistEx(const _TCHAR *volume_name, std::basic_string<_TCHAR> &filename, std::basic_string<_TCHAR> &currentdir)
			{
				TURI volume_uri(volume_name);
				std::basic_string<_TCHAR> ext = volume_uri.extension();
				/*if(ext.empty()) {
					std::basic_stringstream<_TCHAR> str;
					str << volume_name << ".zip";
					DWORD attr = ::GetFileAttributes(str.str().c_str());
					if(attr != -1 && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
						filename = str.str();
						return true;
					}
				}
				else if(ext == _T("zip")) {*/
					DWORD attr = ::GetFileAttributes(volume_name);
					if(attr != -1 && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
						filename = volume_name;
						return true;
					}
				//}
				std::basic_string<_TCHAR> path = volume_uri.path();
				if(!path.empty()) {
					if(currentdir.empty()) currentdir = TURI(volume_name).leaf() + _T("/");
					else currentdir = TURI(volume_name).leaf() + _T("/") + currentdir;
					return isExistEx(path.c_str(), filename, currentdir);
				}
				return false;
			}
			static bool isExist(const _TCHAR *volume_name)
			{
				std::basic_string<_TCHAR> filename;
				std::basic_string<_TCHAR> currentdir;
				return isExistEx(volume_name, filename, currentdir);
			}
			Zip(const _TCHAR *volume_name) : FileServer::Volume(volume_name)
			{
				if(isExistEx(volume_name, filename_, currentdir_)) {
					//PRNN("zip volume_name "<<volume_name);
					//PRNN("zip filename "<<filename_);
					//PRNN("zip currentdir "<<currentdir_);
					zip_.open(filename_.c_str());
				}
			}
			virtual int find(const _TCHAR *fname)
			{
				std::basic_string<_TCHAR> filename = currentdir_+fname;
				if(zip_.seekFile(filename.c_str())) {
					return zip_.getFileSize();
				}
				return -1;
			}
			virtual int read(const _TCHAR *fname, Buffer &buffer)
			{
				std::basic_string<_TCHAR> filename = currentdir_+fname;
				if(zip_.seekFile(filename.c_str())) {
					zip_.openFile();
					int ret = zip_.readFile(buffer.buf(), zip_.getFileSize());
					zip_.closeFile();
					return ret;
				}
				return -1;
			}

			class ZipFileI : public AbstractFile {
			public:
				virtual ~ZipFileI()
				{
					zip_.closeFile();
				}
				virtual int size() const
				{
					return zip_.getFileSize();
				}
				virtual int seek(int pos)
				{
					if(pos < position_) {
						zip_.closeFile();
						zip_.seekFile(filename_.c_str());
						zip_.openFile();
						position_ = 0;
					}
					while(pos > position_) {
						static const int bufsize = 1024*4;
						static char buf[bufsize];
						read(buf, std::min(bufsize, pos-position_));
					}
					return position_;
				}
				virtual int read(void *p, size_t s)
				{
					int ret = zip_.readFile(p, s);
					position_ += ret;
					return ret;
				}

				bool open(const _TCHAR *arcname, const _TCHAR *fname)
				{
					if(!zip_.open(arcname)) return false;
					if(!zip_.seekFile(fname)) return false;
					filename_ = fname;
					zip_.openFile();
					position_ = 0;
					return true;
				}
				ZipFile zip_;
				int position_;
				std::basic_string<_TCHAR> filename_;
			};
			virtual AbstractFilePtr createAbstractFile(const _TCHAR *fname)
			{
				std::basic_string<_TCHAR> filename = currentdir_+fname;
				if(!zip_.seekFile(filename.c_str())) return 0;
				Pointer<ZipFileI> ret = new ZipFileI;
				if(!ret->open(filename_.c_str(), filename.c_str())) return 0;
				return ret;
			}
		private:
			ZipFile zip_;
			std::basic_string<_TCHAR> filename_;
			std::basic_string<_TCHAR> currentdir_;
		};
#endif // GCTP_USE_ZLIB

	}

	class FileServer::Thread {
	public:
		struct Request {
			TCStr name;
			Handle<Volume> volume;
			AsyncBufferPtr buffer;
		};
		struct Read {
			TCStr name;
			AsyncBufferHndl buffer;
		};
		std::queue<Request> requests_;
		std::queue<Read> read_;

		Mutex monitor_;
		Mutex read_list_monitor_;
		Thread(FileServer *fs) : thread_(0), id_(0)
		{
			fs->synchronize(true);
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
			MSG msg;
			while(!(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE) != 0 && msg.message == WM_QUIT)) {
				fs->serviceRequest();
			}
			return 0;
		}
		HANDLE thread_;
		DWORD  id_;
	};

	FileServer::FileServer() : thread_(0), update_slot(Slot::MAX_PRIORITY)
	{
		update_slot.bind(this);
		//mount(_T("."), NATIVE);
	}

	FileServer::~FileServer()
	{
		if(thread_) delete thread_;
	}

	bool FileServer::mount(const _TCHAR *path, ArchiveType type)
	{
		return mount(path, 0, type);
	}

	bool FileServer::mount(const _TCHAR *path, const char *key, ArchiveType type)
	{
		for(PointerList<Volume>::iterator i = volume_list_.begin(); i != volume_list_.end(); i++) {
			if(*i && (*i)->name_ == path) return true;
		}
		if(type == AUTO) {
			if(NativeFS::isExist(path)) {
				type = NATIVE;
			}
			else if(Gcar::isExist(path)) {
				type = GCAR;
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
			PRNN(_T("アーカイブ'")<<path<<_T("'をネイティブFSとしてマウント。"));
			break;
		case GCAR:
			volume_list_.push_back(new Gcar(path, key));
			PRNN(_T("アーカイブ'")<<path<<_T("'をGameCatapultアーカイブとしてマウント。"));
			break;
#ifdef GCTP_USE_ZLIB
		case ZIP:
			volume_list_.push_back(new Zip(path));
			PRNN(_T("アーカイブ'")<<path<<_T("'をZIPアーカイブとしてマウント。"));
			break;
#endif // GCTP_USE_ZLIB
		default:
			GCTP_TRACE(_T("アーカイブ'")<<path<<_T("'のマウントが要求されましたが、失敗しました。"));
			return false;
		}
		stable_sort(volume_list_.begin(), volume_list_.end(), Volume::compare);
		return true;
	}
	
	bool FileServer::unmount(const _TCHAR *path)
	{
		PointerList<Volume>::iterator i;
		for(i = volume_list_.begin(); i != volume_list_.end(); ++i) {
			if(*i && (*i)->name_ == path) break;
		}
		if(i != volume_list_.end()) {
			PRNN(_T("アーカイブ'")<<(*i)->name_<<_T("'をアンマウント。"));
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
		startAsync();
		for(PointerList<Volume>::iterator i = volume_list_.begin(); i != volume_list_.end(); i++) {
			if(*i) {
				int s = (*i)->find(name);
				if(s >= 0) {
					AsyncBufferPtr ret = new AsyncBuffer(s);
					if(ret) {
						Thread::Request request;
						request.name = name;
						request.volume = (*i);
						request.buffer = ret;
						ScopedLock sl(thread_->monitor_);
						thread_->requests_.push(request);
					}
					return ret;
				}
			}
		}
		GCTP_TRACE(_T("要求されたファイル'")<<name<<_T("'はマウントされているボリュームの中に見つかりませんでした。"));
		return AsyncBufferPtr();
	}
	
	AbstractFilePtr FileServer::getFileInterface(const _TCHAR *name)
	{
		for(PointerList<Volume>::iterator i = volume_list_.begin(); i != volume_list_.end(); i++) {
			if(*i) {
				int s = (*i)->find(name);
				if(s >= 0) {
					return (*i)->createAbstractFile(name);
				}
			}
		}
		GCTP_TRACE(_T("要求されたファイル'")<<name<<_T("'はマウントされているボリュームの中に見つかりませんでした。"));
		return AbstractFilePtr();
	}

	bool FileServer::service(float delta)
	{
		if(thread_) {
			ScopedLock sl(thread_->read_list_monitor_);
			while(!thread_->read_.empty()) {
				AsyncBufferPtr buffer = thread_->read_.front().buffer.lock();
				if(buffer) buffer->ready_signal(thread_->read_.front().name.c_str(), buffer);
				thread_->read_.pop();
			}
		}
		return true;
	}

	void FileServer::serviceRequest()
	{
		Thread::Request req;
		{
			ScopedLock sl(thread_->monitor_);
			if(!thread_->requests_.empty()) {
				req = thread_->requests_.front();
				thread_->requests_.pop();
			}
		}
		if(req.volume) {
			Pointer<Volume> volume = req.volume.lock();
			if(volume) {
				if(volume->read(req.name.c_str(), *req.buffer) >= 0) {
					req.buffer->is_ready_ = true;
					{
						ScopedLock sl(thread_->read_list_monitor_);
						Thread::Read read;
						read.name = req.name;
						read.buffer = req.buffer;
						thread_->read_.push(read);
					}
				}
			}
		}
		else {
			::Sleep(0);
		}
	}
	
	void FileServer::startAsync()
	{
		if(!thread_) thread_ = new Thread(this);
		app().update_signal.connectOnce(update_slot);
	}
	
	bool FileServer::busy()
	{
		return false;
	}

	bool FileServer::done()
	{
		if(thread_) {
			ScopedLock sl(thread_->monitor_);
			return thread_->requests_.empty() && thread_->read_.empty();
		}
		return true;
	}

	FileServer &FileServer::getInstance()
	{
		static FileServer instance;
		return instance;
	}

} // namespace gctp
