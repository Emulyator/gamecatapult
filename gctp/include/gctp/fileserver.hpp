#ifndef _GCTP_FILESERVER_HPP_
#define _GCTP_FILESERVER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult�t�@�C���T�[�o�[�N���X
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

	/// ���ۃt�@�C���C���^�[�t�F�[�X
	class AbstractFile : public Object {
	public:
		virtual ~AbstractFile();
		virtual int size() const = 0;
		virtual int seek(int pos) = 0;
		virtual int read(void *s, size_t size) = 0;
	};
	typedef Pointer<AbstractFile> AbstractFilePtr;

	/** �A�[�J�C�u�̃}�E���g�A�񓯊��ǂݍ��݃T�[�r�X��񋟂���N���X
	 *
	 * �l�C�e�B�u�t�@�C���V�X�e���Agar�Azip���}�E���g�ł���
	 *
@code
	AyncBufferPtr file;
	if(!fileserver().busy()) file = fileserver().getFileAsync(fname); // ���N�G�X�g�ǉ�
	if(file.isReady()) {
		// ���A���C�U�Ăяo��
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
		/// �A�[�J�C�u�̎�ʂ������萔
		enum ArchiveType {
			AUTO, ///< �����F��
			NATIVE, ///< �l�C�e�B�u�̃t�@�C���V�X�e���i�P�Ƀt�H���_���w�肷�邾���j
			GCAR, ///< gcar�A�[�J�C�u
			ZIP, ///< zip�A�[�J�C�u
		};

		/// �A�[�J�C�u���}�E���g����
		bool mount(const _TCHAR *path, ArchiveType type = AUTO);
		/// (�Í���)�A�[�J�C�u���}�E���g����
		bool mount(const _TCHAR *path, const char *key, ArchiveType type = AUTO);
		/// �A�[�J�C�u���A���}�E���g����
		bool unmount(const _TCHAR *path);
		/// �����D��x�̐ݒ�
		void setPriority(const _TCHAR *archive_name, int priority);
		/// �����D��x�̎擾
		int getPriority(const _TCHAR *archive_name);

		/// �t�@�C���𓯊��ǂݍ��݂����t�@�C���S�̂̓��e��Ԃ�
		BufferPtr getFile(const _TCHAR *name);
		/// �t�@�C����񓯊��ǂݍ��݂����t�@�C���S�̂̓��e��Ԃ�
		AsyncBufferPtr getFileAsync(const _TCHAR *name);
		/// �t�@�C���ǂݍ��݃C���^�[�t�F�[�X��Ԃ�
		AbstractFilePtr getFileInterface(const _TCHAR *name);

		/// �񓯊��ǂݍ��݃T�[�r�X�J�n
		void startAsync();
		/// �񓯊��ǂݍ��݃T�[�r�X�I��
		void endAsync();
		/// �񓯊��ǂݍ��݂��󂯓���s�\���H
		bool busy();
		/// �񓯊��ǂݍ��݃��N�G�X�g�����ׂď������ꂽ���H
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
		class Thread; // �O�ɏo���Ă��������ȁc
		friend Thread;
		void serviceRequest();
		Thread *thread_;
	};

	/// FileServer���擾
	inline FileServer &fileserver()
	{
		return FileServer::getInstance();
	}

	/** �񓯊��ǂݍ��݂̓��e��񋟂���o�b�t�@�N���X
	 *
	 * getFileAsync�͓ǂݍ��ݐ�̃������������m�ۂ��A�܂����e�̖��܂��Ă��Ȃ�
	 * AsyncBuffer�𑦍��ɕԂ��B
	 * 
	 * ������󂯎�������́AisReady�����o���Ăяo�����ƂŁA�ǂݍ��݂������������Ƃ�m�邱�Ƃ��o����B
	 *
	 * ���[�U�[�͂��ł��o�b�t�@�ɃA�N�Z�X�ł��邪�AisReady��true��Ԃ��܂ł͊��S�ȓ��e�ł͖����B
	 *
	 * FileServer�������Aready�t���O�����Ă邱�Ƃ��o����B
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