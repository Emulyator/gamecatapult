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
#include <gctp/buffwd.hpp> // for BufferPtr
#include <tchar.h>

namespace gctp {

	/** ���\�[�X�����烊�\�[�X���\�z����֐��|�C���^
	 *
	 * �񓯊��p�ɁAbuffer��NULL��������
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:17:49
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	typedef Ptr (*RealizeMethod)(BufferPtr buffer);

	/** ���\�[�X�����烊�\�[�X���\�z����֐��|�C���^
	 *
	 * �񓯊��p�ɁAbuffer��NULL���������̃I�u�W�F�N�g�i�f�t�H���g�R���X�g���N�^���Ă΂ꂽ�����j���A
	 * self��NULL�ȊO���n���ꂽ�炻��ɑ΂���SetUp���Ăяo���悤�ɂȂ��Ă��Ȃ���΂����Ȃ��B
	 * �����ǂ݂̏ꍇ��self==NULL,buffer!=NULL�ŌĂяo�����B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:17:49
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	//typedef Ptr (*RealizeMethod)(Ptr self, BufferPtr buffer);

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
		~Extension();
		bool guard_for_unlink() { return true; }
		static RealizeMethod get(const _TCHAR *extension);
	private:
		const _TCHAR *ext_;
	};

	template<class T, bool (T::*SetUpMethod)(BufferPtr)>
	Ptr realize(BufferPtr buffer)
	{
		Pointer<T> ret = new T;
		if(ret) {
			if((ret.get()->*SetUpMethod)(buffer)) {
				return ret;
			}
		}
		return Ptr();
	}

/*	template<class T, bool (T::*SetUpMethod)(BufferPtr)>
	Ptr realize(Ptr self, BufferPtr buffer)
	{
		if(!self) self = new T;
		if(self && buffer) {
			if((self.get()->*SetUpMethod)(buffer)) {
				return self;
			}
			return Ptr();
		}
		return self;
	}*/

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
#define GCTP_REGISTER_REALIZER(_E, _C)							\
	static Ptr _C##_E##_realize(BufferPtr buffer) {				\
		return gctp::realize<_C, &_C::setUp>(buffer);			\
	}															\
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
#define GCTP_REGISTER_REALIZER2(_E, _C, _M)						\
	static Ptr _C##_E##_realize(BufferPtr buffer) {				\
		return gctp::realize<_C, _M>(buffer);					\
	}															\
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
#define GCTP_REGISTER_REALIZER_EX(_S, _E, _C)				\
	static Ptr _C##_E##_realize(BufferPtr buffer) {			\
		return gctp::realize<_C, &_C::setUp>(buffer);		\
	}														\
	static Extension _C##_E##_realizer(_S, _C##_E##_realize)

/** �ėp���A���C�U��`�}�N��
 *
 * �Z�b�g�A�b�v���\�b�h���w��ł���
 *
 * �g���q������ƁArealize���\�b�h�̖��O��ʂɂ���i���ʂ̎���������ꍇ�j
 *
 * �R���e�L�X�g�c�a�ɓo�^����̂ł���΁Aconst char *name���󂯎��setUp�����o�֐���
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
#define GCTP_REGISTER_REALIZER2_EX(_S, _E, _C, _M)			\
	static Ptr _C##_E##_realize(BufferPtr buffer) {			\
		return gctp::realize<_C, _M>(buffer);				\
	}														\
	static Extension _C##_E##_realizer(_S, _C##_E##_realize)

#endif //_GCTP_EXTENSION_HPP_
