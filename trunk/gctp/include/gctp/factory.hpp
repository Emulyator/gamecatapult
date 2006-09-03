#ifndef _GCTP_FACTORY_HPP_
#define _GCTP_FACTORY_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �t�@�N�g���[���\�b�h�f�[�^�x�[�X�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 23:00:42
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/typeinfo.hpp>

namespace gctp {

	class Object;
	class Ptr;

	/// �t�@�N�g���[���\�b�h�̌^
	typedef Object *(*FactoryMethod)();

	/** GameCatapult�t�@�N�g���[�o�^�N���X
	 *
	 * ���̃N���X�̃C���X�^���X�́A�ÓI�I�u�W�F�N�g�Ƃ��āA�t�@�N�g���[���\�b�h�̎����o�^�����邽�߂̂��́B
	 *
	 * gctp::Factory�̋@�\�𗘗p����ɂ́AObject�h���ł��邱�ƁA����Ƀf�t�H���g�R���X�g���N�^
	 * �������ƁA�������B\n
	 * GCTP_IMPLEMENT_FACTORY�̎������Q�l��FactoryMethod�����O�ō��΁A�K�������f�t�H���g�R���X�g���N�^
	 * �����K�v�͖�������ǂ��B
	 * @see GCTP_DECLARE_FACTORY
	 * @see GCTP_IMPLEMENT_FACTORY
	 * @see GCTP_USE_FACTORY
	 * @see Class
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/09 23:00:53
	 *
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Factory {
	public:
		Factory(const GCTP_TYPEINFO &typeinfo, const FactoryMethod f);
		static Ptr create(const GCTP_TYPEINFO &typeinfo);
		static Ptr create(const char *classname);
	};

} // namespace gctp

/** �t�@�N�g���[���\�b�h��ǉ����邽�߂̃}�N��
 *
 * @code
 *  class Foo : public Object {
 *     Foo();
 *     GCTP_DECLARE_FACTORY; //<-�������N���X��`���ɏ���
 *  };
 *
 * in impleremt file.
 *
 * GCTP_IMPLEMENT_FACTORY(Foo, Object); //<-������cpp�t�@�C�����ɏ���
 * @endcode
 *
 * �A�N�Z�X�w��q���܂܂�Ă��邱�Ƃɒ��ӁB���̃}�N���̒����private�ɂȂ�B
 */
#define GCTP_DECLARE_FACTORY										\
	public:															\
		static gctp::Factory &useFactory() { return factory__; }	\
		static gctp::Object *create();								\
	private:														\
		static gctp::Factory factory__;

/** GCTP_DECLARE_FACTORY�}�N���ɑΉ���������}�N��
 *
 * C++��̃N���X���Ƃ͕ʂ̃N���X�����������ꍇ�͂�����g�p����
 *
 * @code
 *  class Foo : public Object {
 *     Foo();
 *     GCTP_DECLARE_FACTORY; //<-�������N���X��`���ɏ���
 *  };
 *
 * in impleremt file.
 *
 * GCTP_IMPLEMENT_FACTORY(Foo); //<-������cpp�t�@�C�����ɏ���
 * @endcode
 */
#define GCTP_IMPLEMENT_FACTORY(_classname)												\
	gctp::Object *_classname::create() {												\
		gctp::Object *ret = new(_classname::allocate(sizeof(_classname))) _classname();	\
		if(ret) ret->setDeleter(_classname::deleter());									\
		return ret;																		\
	}																					\
	gctp::Factory _classname::factory__(GCTP_TYPEID(_classname), _classname::create);

/** GameCatapult�̃I�u�W�F�N�g���g�p�\�ɂ���
 *
 * Factory���錾����Ă��Ă��A���̃N���X�����ڎg�p����Ă��Ȃ��iPointer�̃e���v���[�g�����ł̂�
 * �g���Ă���ꍇ�Ȃǁj�ƃ����J�ɂ����Factory�̃I�u�W�F�N�g���폜�����B
 * �����h�����߂ɂ�����A�v���P�[�V�����{�̂̂ǂ����ɏ����Ă����B
 */
#define GCTP_USE_FACTORY(_classname)	{ _classname::useFactory(); }

#endif //_GCTP_FACTORY_HPP_