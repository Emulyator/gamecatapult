#ifndef _GCTP_SCENE_GRAPHFILE_HPP_
#define _GCTP_SCENE_GRAPHFILE_HPP_
/** @file
 * GameCatapult �V�[���O���t�R���e���g�t�@�C���N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 21:29:23
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/buffwd.hpp> // for BufferPtr
#include <gctp/pointerlist.hpp>
#include <gctp/dxcomptrs.hpp>
#include <map>

namespace gctp { namespace scene {

	typedef Ptr (*XFileHandleMethod)(PtrList pvector, const char *extra_name);
	/** �w�t�@�C���n���h���[�o�^�N���X
	 *
	 * �e���v���[�g�h�c(GUID)���n���h���[�̂c�a���\������B
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:17:33
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class XFileHandler {
	public:
		XFileHandler(const GUID &template_id, const XFileHandleMethod f);
		~XFileHandler();

		static bool hasHandler(const GUID &template_id);
		static XFileHandleMethod get(const GUID &template_id);

	private:
		typedef std::map<GUID, XFileHandleMethod> HandlerMap;
		HandlerMap::iterator me_;
		static HandlerMap &handlers();
	};

	struct XFileReadingWork {
		ulong ticks_per_sec;
	};

	/** �V�[���O���t�t�@�C���ǂݍ���
	 *
	 * �t�@�C����ǂ݂���ŁA�����̃f�[�^�����\�[�X�Ƃ��ēo�^���Ă����B\n
	 * �o�^���ꂽ���\�[�X�͂��̃t�@�C�����\�[�X�ɂ���ď��L����A
	 * ���̃��\�[�X�̊J���ɂ���āA�ǂ݂��񂾃��\�[�X���J������B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 15:07:29
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class GraphFile : public Object, public PtrList {
	public:
		/// XFile�̓ǂ݂���
		bool setUpFromX(const _TCHAR *fn);
		/// XFile�̓ǂ݂���
		bool setUpFromX(BufferPtr buffer);
		
		PtrList asyncsolvers;
		
	GCTP_DECLARE_CLASS;
	};

}} //namespace gctp::scene

/** �w�t�@�C���ǂݍ��݃n���h���o�^�}�N��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#define GCTP_REGISTER_XFILE_HANDLER(_TID, _M)				\
	Ptr _TID##_handler(XData *cur) {						\
		Handle<_T> h = db()[name];							\
		if(h) return h.lock();								\
		Pointer<_T> ret = Object::create(GCTPTYPEID(_T));	\
		if(ret) {											\
			ret->setUp(name);								\
			db().insert(Hndl(ret.get()), name);				\
		}													\
		return ret;											\
	}														\
	XFileHandler _TID##_handler_reg(_TID, _TID##_handler)

#endif //_GCTP_SCENE_GRAPHFILE_HPP_