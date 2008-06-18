#ifndef _GCTP_FILESERVER_HPP_
#define _GCTP_FILESERVER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapultファイルサーバークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/05 15:59:05
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointerlist.hpp>
#include <gctp/signal.hpp>
#include <gctp/buffer.hpp>
#include <gctp/tcstr.hpp>

namespace gctp {

	class AsyncBuffer;
	typedef Pointer<AsyncBuffer> AsyncBufferPtr;
	typedef Handle<AsyncBuffer> AsyncBufferHndl;

	/// 抽象ファイルインターフェース
	class AbstractFile : public Object {
	public:
		virtual ~AbstractFile();
		virtual int size() const = 0;
		virtual int seek(int pos) = 0;
		virtual int read(void *s, size_t size) = 0;
	};
	typedef Pointer<AbstractFile> AbstractFilePtr;

	/** アーカイブのマウント、非同期読み込みサービスを提供するクラス
	 *
	 * ネイティブファイルシステム、gar、zipをマウントできる
	 *
@code
	AyncBufferPtr file;
	if(!fileserver().busy()) file = fileserver().getFileAsync(fname); // リクエスト追加
	if(file.isReady()) {
		// リアライザ呼び出し
		f(file);
	}
@endcode
	 * @author SAM (T&GG, Org.) <sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 *
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class FileServer : public Object {
	public:
		/// アーカイブの種別を示す定数
		enum ArchiveType {
			AUTO, ///< 自動認識
			NATIVE, ///< ネイティブのファイルシステム（単にフォルダを指定するだけ）
			GCAR, ///< gcarアーカイブ
			ZIP, ///< zipアーカイブ
		};

		/// アーカイブをマウントする
		bool mount(const _TCHAR *path, ArchiveType type = AUTO);
		/// (暗号化)アーカイブをマウントする
		bool mount(const _TCHAR *path, const char *key, ArchiveType type = AUTO);
		/// アーカイブをアンマウントする
		bool unmount(const _TCHAR *path);
		/// 検索優先度の設定
		void setPriority(const _TCHAR *archive_name, int priority);
		/// 検索優先度の取得
		int getPriority(const _TCHAR *archive_name);

		/// ファイルを同期読み込みしたファイル全体の内容を返す
		BufferPtr getFile(const _TCHAR *name);
		/// ファイルを非同期読み込みしたファイル全体の内容を返す
		AsyncBufferPtr getFileAsync(const _TCHAR *name);
		/// ファイル読み込みインターフェースを返す
		AbstractFilePtr getFileInterface(const _TCHAR *name);

		/// 非同期読み込みサービス開始
		void startAsync();
		/// 非同期読み込みサービス終了
		void endAsync();
		/// 非同期読み込みを受け入れ不可能か？
		bool busy();
		/// 非同期読み込みリクエストがすべて処理されたか？
		bool done();

		bool service(float delta);
		MemberSlot1<FileServer, float, &FileServer::service> update_slot;

		static FileServer &getInstance();

		class Volume;
		~FileServer();

	private:
		FileServer();
		FileServer(FileServer &); // not implement
		PointerList<Volume> volume_list_;
		class Thread; // 外に出してもいいかな…
		friend Thread;
		void serviceRequest();
		Thread *thread_;
	};

	/// FileServerを取得
	inline FileServer &fileserver()
	{
		return FileServer::getInstance();
	}

	/** 非同期読み込みの内容を提供するバッファクラス
	 *
	 * getFileAsyncは読み込み先のメモリだけを確保し、まだ内容の埋まっていない
	 * AsyncBufferを即座に返す。
	 * 
	 * これを受け取った側は、isReadyメンバを呼び出すことで、読み込みが完了したことを知ることが出来る。
	 *
	 * ユーザーはいつでもバッファにアクセスできるが、isReadyがtrueを返すまでは完全な内容では無い。
	 *
	 * FileServerだけが、readyフラグをたてることが出来る。
	 * @author SAM (T&GG, Org.) <sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2002/12/31 16:49:24
	 *
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class AsyncBuffer : public Buffer {
	public:
		bool isReady()
		{
			if(is_ready_) synchronize(false);
			return is_ready_;
		}
		void connect(const Slot2<const _TCHAR *, BufferPtr> &slot)
		{
			ready_signal.connect(slot);
		}
		void disconnect(const Slot2<const _TCHAR *, BufferPtr> &slot)
		{
			ready_signal.disconnect(slot);
		}
	protected:
		AsyncBuffer() : is_ready_(false)
		{
			synchronize(true);
		}
		AsyncBuffer(std::size_t size) : Buffer(size), is_ready_(false)
		{
			synchronize(true);
		}
		friend FileServer;
		MonoSignal2<const _TCHAR *, BufferPtr> ready_signal;
		bool is_ready_;
	};

} // namespace gctp

#endif //_GCTP_FILESERVER_HPP_