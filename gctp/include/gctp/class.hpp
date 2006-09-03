#ifndef _GCTP_CLASS_HPP_
#define _GCTP_CLASS_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���^�N���X�N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 23:00:42
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/typeinfo.hpp>

namespace gctp {

	/** GameCatapult�^���f�[�^�x�[�X�N���X
	 *
	 * @see GCTP_DECLARE_CLASS
	 * @see GCTP_IMPLEMENT_CLASS
	 * @see GCTP_IMPLEMENT_CLASS_EX
	 * @see GCTP_IMPLEMENT_CLASS_NS
	 * @see GCTP_USE_CLASS
	 * @see GCTP_DECLARE_FACTORY
	 * @see GCTP_IMPLEMENT_FACTORY
	 * @see GCTP_USE_FACTORY
	 * @see Factory
	 *
	 * @todo UUID(GUID)���g���悤�ɂ���
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 13:27:01
	 *
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Class {
	public:
		Class(const char *classname, const GCTP_TYPEINFO &typeinfo);
		Class(const char *classname, const GCTP_TYPEINFO &typeinfo, const GCTP_TYPEINFO &basetypeinfo);
		Class(const char *ns, const char *classname, const GCTP_TYPEINFO &typeinfo);
		Class(const char *ns, const char *classname, const GCTP_TYPEINFO &typeinfo, const GCTP_TYPEINFO &basetypeinfo);

		static const GCTP_TYPEINFO *get(const char *ns, const char *classname);
		static const GCTP_TYPEINFO *get(const char *classname);
		
		struct Name { const char *ns, *classname; };
		static Name get(const GCTP_TYPEINFO &typeinfo);

		static const GCTP_TYPEINFO *getBase(const char *classname);
		static const GCTP_TYPEINFO *getBase(const GCTP_TYPEINFO &typeinfo);
	};

} // namespace gctp

#include <gctp/factory.hpp>

/** ���^�N���X��ǉ����邽�߂̃}�N��
 *
 * @code
 *  class Foo : public Object {
 *     Foo();
 *     GCTP_DECLARE_CLASS; //<-�������N���X��`���ɏ���
 *  };
 *
 * in impleremt file.
 *
 * GCTP_IMPLEMENT_CLASS(Foo, Object); //<-������cpp�t�@�C�����ɏ���
 * @endcode
 *
 * �A�N�Z�X�w��q���܂܂�Ă��邱�Ƃɒ��ӁB���̃}�N���̒����private�ɂȂ�B
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/01/05 13:30:37
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_DECLARE_CLASS									\
	GCTP_DECLARE_TYPEINFO									\
	GCTP_DECLARE_FACTORY									\
	public:													\
		static gctp::Class &useClass() { return class__; }	\
	private:												\
		static gctp::Class class__;

/** GCTP_DECLARE_CLASS�}�N���ɑΉ���������}�N��
 *
 * C++��̃N���X���Ƃ͕ʂ̃N���X�����������ꍇ�͂�����g�p����
 *
 * @code
 *  class Foo : public Object {
 *     Foo();
 * 
 *  GCTP_DECLARE_CLASS //<-�������N���X��`���ɏ���
 *  };
 *
 * in impleremt file.
 *
 * GCTP_IMPLEMENT_CLASS_EX("�t�H�H", Foo, Object); //<-������cpp�t�@�C�����ɏ���
 * @endcode
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/01/05 13:30:37
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_IMPLEMENT_CLASS_EX(_classnamestring, _classname, _baseclassname)	\
	GCTP_IMPLEMENT_TYPEINFO(_classname, _baseclassname)							\
	GCTP_IMPLEMENT_FACTORY(_classname)											\
	gctp::Class _classname::class__(_classnamestring, GCTP_TYPEID(_classname), GCTP_TYPEID(_baseclassname));

/** GCTP_DECLARE_CLASS�}�N���ɑΉ���������}�N��
 *
 * C++��̖��O�Ɠ���ł悢�ꍇ�́A��������g�p����
 *
 * @code
 *  class Foo : public Object {
 *     Foo();
 * 
 *  GCTP_DECLARE_CLASS // <-�������N���X��`���ɏ���
 *  };
 *
 * in impleremt file.
 *
 * GCTP_IMPLEMENT_CLASS(Foo, Object); // <-������cpp�t�@�C�����ɏ���
 * @endcode
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/01/05 13:30:37
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_IMPLEMENT_CLASS(_classname, _baseclassname)	\
	GCTP_IMPLEMENT_CLASS_EX(#_classname, _classname, _baseclassname)

/** GCTP_DECLARE_CLASS�}�N���ɑΉ���������}�N��
 *
 * ���O��Ԃ��w�肷��ꍇ������g��
 *
 * @code
 * namespase my {
 *   class Foo : public Object {
 *     Foo();
 *     GCTP_DECLARE_CLASS; // <-�������N���X��`���ɏ���
 *   };
 * }
 *
 * in impleremt file.
 *
 * GCTP_IMPLEMENT_CLASS_NS(my, Foo, Object); // <-������cpp�t�@�C�����ɏ���
 * @endcode
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/01/05 13:30:37
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_IMPLEMENT_CLASS_NS(_ns, _classname, _baseclassname)	\
	GCTP_IMPLEMENT_TYPEINFO(_classname, _baseclassname)				\
	GCTP_IMPLEMENT_FACTORY(_classname)								\
	gctp::Class _classname::class__(#_ns, #_classname, GCTP_TYPEID(_classname), GCTP_TYPEID(_baseclassname));

#define GCTP_IMPLEMENT_CLASS_NS2(_ns1, _ns2, _classname, _baseclassname)	\
	GCTP_IMPLEMENT_TYPEINFO(_classname, _baseclassname)						\
	GCTP_IMPLEMENT_FACTORY(_classname)										\
	gctp::Class _classname::class__(#_ns1"."#_ns2, #_classname, GCTP_TYPEID(_classname), GCTP_TYPEID(_baseclassname));

#define GCTP_IMPLEMENT_CLASS_NS3(_ns1, _ns2, _ns3, _classname, _baseclassname)	\
	GCTP_IMPLEMENT_TYPEINFO(_classname, _baseclassname)							\
	GCTP_IMPLEMENT_FACTORY(_classname)											\
	gctp::Class _classname::class__(#_ns1"."#_ns2"."#_ns3, #_classname, GCTP_TYPEID(_classname), GCTP_TYPEID(_baseclassname));

/** GameCatapult�̃I�u�W�F�N�g���g�p�\�ɂ���
 *
 * Class���錾����Ă��Ă��A���̃N���X�����ڎg�p����Ă��Ȃ��iPointer�̃e���v���[�g�����ł̂�
 * �g���Ă���ꍇ�Ȃǁj�ƃ����J�ɂ����Class�̃I�u�W�F�N�g���폜�����B
 * �����h�����߂ɂ�����A�v���P�[�V�����{�̂̂ǂ����ɏ����Ă����B
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/09/20 7:47:28
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_USE_CLASS(_classname)	{ _classname::useClass(); GCTP_USE_FACTORY(_classname); }

#endif //_GCTP_CLASS_HPP_