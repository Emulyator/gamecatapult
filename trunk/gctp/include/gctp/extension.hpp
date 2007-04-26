#ifndef _GCTP_EXTENSION_HPP_
#define _GCTP_EXTENSION_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���\�[�X���A���C�U�o�^�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointer.hpp>
//#ifndef GCTP_NOTDEF_BOOST_FUNC
//#include <boost/function.hpp>
//#endif
#include <tchar.h> // VC���肾�ȁc

namespace gctp {

// �ǂ������킯������̂Ƃ���ŁAboost::function1���肾��VC6�ł̓R���p�C���ł��Ȃ��̂Łc
//#ifndef GCTP_NOTDEF_BOOST_FUNC
	/** ���\�[�X�����烊�\�[�X���\�z����֐��|�C���^
	 *
	 * �i������Ǝv���Ƃ��낪�����Ċ֐��I�u�W�F�N�g�łȂ��|�C���^�Ɂj
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:17:49
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	typedef Ptr (*RealizeMethod)(const _TCHAR *);
	//typedef boost::function1<PointerBase, const _TCHAR *> RealizeMethod;

	/** ���\�[�X���A���C�U�o�^�N���X
	 *
	 * �g���q�����A���C�U�̂c�a���\������B
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:17:44
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Extension {
	public:
		Extension(const _TCHAR *extension, const RealizeMethod f);
		//Extension(const _TCHAR *extension, const RealizeMethod &f);
		~Extension();
		bool guard_for_unlink() { return true; }
		static RealizeMethod get(const _TCHAR *extension);
	private:
		const _TCHAR *ext_;
	};
//#endif

#if 0
	/** �ėp���A���C�U�e���v���[�g
	 *
	 * �O���[�o���c�a�ɓo�^����̂ł���΁Aconst char *name���󂯎��setUp�����o�֐���
	 * �p�ӂ��āAExtension txt_realizer("txt", Realizer<Text>()); �Ƃ��Ă��΂悢
	 *
	 * ���A���C�U�͓����̃��\�[�X�����݂����ꍇ�A�V�K�ɐ��삹�������̃I�u�W�F�N�g��Ԃ��B
	 *
	 * ���[�U�[���J�X�^���������̂��`����ɂ��Ă��A���̋K�������Ă��炤�K�v������B
	 * 
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:18:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	class Realizer {
	public:
		PointerBase operator()(const char *name) {
			Handle<_T> h = db()[name];
			if(h) return h.lock();
			Pointer<_T> ret = Object::create(GCTP_TYPEID(_T));
			if(ret) {
				ret->setUp(name);
				db().insert(Hndl(ret.get()), name);
			}
			return ret;
		}
	};
#endif

} // namespace gctp

/** �ėp���A���C�U��`�}�N��
 *
 * �O���[�o���c�a�ɓo�^����̂ł���΁Aconst char *name���󂯎��setUp�����o�֐���
 * �p�ӂ��āAGCTP_IMPLEMENT_REALIZER(txt, Text); �ȂǂƂ��Ă��΂悢
 *
 * ���A���C�U�͓����̃��\�[�X�����݂����ꍇ�A�V�K�ɐ��삹�������̃I�u�W�F�N�g��Ԃ��B
 *
 * ���[�U�[���J�X�^���������̂��`����ɂ��Ă��A���̋K�������Ă��炤�K�v������B
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#define GCTP_REGISTER_REALIZER(_E, _C)						\
	static Ptr _C##_E##_realize(const _TCHAR *name) {		\
		return createOnDB<_C>(name);						\
	}														\
	static Extension _C##_E##_realizer(_T(#_E), _C##_E##_realize)

/** �ėp���A���C�U��`�}�N��
 *
 * �Z�b�g�A�b�v���\�b�h���w��ł���
 *
 * �O���[�o���c�a�ɓo�^����̂ł���΁Aconst char *name���󂯎��setUp�����o�֐���
 * �p�ӂ��āAGCTP_IMPLEMENT_REALIZER(txt, Text); �ȂǂƂ��Ă��΂悢
 *
 * ���A���C�U�͓����̃��\�[�X�����݂����ꍇ�A�V�K�ɐ��삹�������̃I�u�W�F�N�g��Ԃ��B
 *
 * ���[�U�[���J�X�^���������̂��`����ɂ��Ă��A���̋K�������Ă��炤�K�v������B
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#define GCTP_REGISTER_REALIZER2(_E, _C, _M)					\
	static Ptr _C##_E##_realize(const _TCHAR *name) {		\
		return createOnDB<_C, _M>(name);					\
	}														\
	static Extension _C##_E##_realizer(_T(#_E), _C##_E##_realize)

/** �ėp���A���C�U��`�}�N��
 *
 * �g���q������ƁArealize���\�b�h�̖��O��ʂɂ���i���ʂ̎�������ꍇ�j
 *
 * �O���[�o���c�a�ɓo�^����̂ł���΁Aconst char *name���󂯎��setUp�����o�֐���
 * �p�ӂ��āAGCTP_IMPLEMENT_REALIZER("setup string", txt, Text); �ȂǂƂ��Ă��΂悢
 *
 * ���A���C�U�͓����̃��\�[�X�����݂����ꍇ�A�V�K�ɐ��삹�������̃I�u�W�F�N�g��Ԃ��B
 *
 * ���[�U�[���J�X�^���������̂��`����ɂ��Ă��A���̋K�������Ă��炤�K�v������B
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#define GCTP_REGISTER_REALIZER_EX(_S, _E, _T)				\
	static Ptr _T##_E##_realize(const _TCHAR *name) {		\
		return createOnDB<_T>(name);						\
	}														\
	static Extension _T##_E##_realizer(_S, _T##_E##_realize)

/** �ėp���A���C�U��`�}�N��
 *
 * �Z�b�g�A�b�v���\�b�h���w��ł���
 *
 * �g���q������ƁArealize���\�b�h�̖��O��ʂɂ���i���ʂ̎�������ꍇ�j
 *
 * �O���[�o���c�a�ɓo�^����̂ł���΁Aconst char *name���󂯎��setUp�����o�֐���
 * �p�ӂ��āAGCTP_IMPLEMENT_REALIZER("setup string", txt, Text); �ȂǂƂ��Ă��΂悢
 *
 * ���A���C�U�͓����̃��\�[�X�����݂����ꍇ�A�V�K�ɐ��삹�������̃I�u�W�F�N�g��Ԃ��B
 *
 * ���[�U�[���J�X�^���������̂��`����ɂ��Ă��A���̋K�������Ă��炤�K�v������B
 * 
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#define GCTP_REGISTER_REALIZER2_EX(_S, _E, _T, _M)			\
	static Ptr _T##_E##_realize(const _TCHAR *name) {		\
		return createOnDB<_T, _M>(name);					\
	}														\
	static Extension _T##_E##_realizer(_S, _T##_E##_realize)

#endif //_GCTP_EXTENSION_HPP_
