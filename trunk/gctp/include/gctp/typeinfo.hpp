#ifndef _GCTP_TYPEINFO_HPP_
#define _GCTP_TYPEINFO_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���s���^���N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/09 23:00:42
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/config.hpp>
#ifdef GCTP_USE_STD_RTTI
#include <typeinfo>
#define GCTP_TYPEINFO		std::type_info
#define GCTP_DYNCAST		dynamic_cast
#define GCTP_TYPEID(x)		typeid(x)
#define GCTP_THISTYPEID(x)	typeid(x)
#define GCTP_DEFINE_TYPEINFO(_classname, _baseclassname)
#define GCTP_DECLARE_TYPEINFO
#define GCTP_IMPLEMENT_TYPEINFO(_classname, _baseclassname)
#else
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

namespace gctp {

	/** GameCatapult���s���^���N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 13:27:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class TypeInfo {
	public:
		TypeInfo(const char* name, const TypeInfo *_super) : name_(name), super_(_super) {}

		int operator==(const TypeInfo& rhs) const
		{
			return this == &rhs;
		}
		int operator!=(const TypeInfo& rhs) const
		{
			return this != &rhs;
		}

		/// �L���X�g�\���H
		int before(const TypeInfo& upperclass) const // ����ŗǂ��̂��Ȃ�
		{
			if(&upperclass == this) return 1;
			if(super_) return super_->before(upperclass);
			return 0;
		}

		const char* name() const { return name_; }
		const char* raw_name() const { return name_; }

		/// �X�[�p�[�N���X�̌^����Ԃ�
		const TypeInfo *getBase() const { return super_; }

	private:
		const TypeInfo *super_;
		const char *name_;
	};

	class Object;
	/// gctp::TypeInfo��p�����_�C�i�~�b�N�L���X�g
	template<class _T>
	_T dynamicCast(Object *p)
	{
		BOOST_STATIC_ASSERT(boost::is_pointer<_T>::value); // �|�C���^�[�^�ȊO�Ή����Ȃ�
		if(p && p->typeOfThis().before(boost::remove_pointer<_T>::type::typeinfo())) return reinterpret_cast<_T>(p);
		return 0;
	}

	/// gctp::TypeInfo��p�����_�C�i�~�b�N�L���X�g(const��)
	template<class _T>
	_T dynamicCast(const Object *p)
	{
		BOOST_STATIC_ASSERT(boost::is_pointer<_T>::value); // �|�C���^�[�^�ȊO�Ή����Ȃ�
		if(p && p->typeOfThis().before(boost::remove_pointer<_T>::type::typeinfo())) return reinterpret_cast<_T>(p);
		return 0;
	}

}

#define GCTP_TYPEINFO		gctp::TypeInfo
#define GCTP_DYNCAST		gctp::dynamicCast
#define GCTP_TYPEID(T)		(T::typeinfo())
#define GCTP_THISTYPEID(x)	((x).typeOfThis())

/** ���s���^�����`����}�N��
 *
 * @code
 *  class Foo : public Object {
 *  public:
 *     Foo();
 *     GCTP_DEFINE_TYPEINFO(Foo, Object) //<-�������N���X��`���ɏ���
 *  };
 *
 *  class Bar : public Foo {
 *  public:
 *     Bar();
 *     GCTP_DEFINE_TYPEINFO(Bar, Foo) //<-�������N���X��`���ɏ���
 *  };
 * // ���̏ꍇBar��Foo,Pointee�Ƀ_�E���L���X�g�\
 * @endcode
 *
 * �A�N�Z�X�w��q���܂܂�Ă��邱�Ƃɒ��ӁB���̃}�N���̒����public�ɂȂ�B
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/01/05 13:30:37
 * $Id: typeinfo.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_DEFINE_TYPEINFO(_classname, _baseclassname)								\
	public:																				\
		static gctp::TypeInfo &typeinfo() {												\
			static gctp::TypeInfo typeinfo(#_classname, &_baseclassname::typeinfo());	\
			return typeinfo;															\
		}																				\
		virtual const gctp::TypeInfo &typeOfThis() const { return typeinfo(); }

/** ���s���^������������}�N��
 *
 * @code
 *  class Foo : public Object {
 *  public:
 *     Foo();
 *     GCTP_DEFINE_TYPEINFO //<-�������N���X��`���ɏ���
 *  };
 *
 *  class Bar : public Foo {
 *  public:
 *     Bar();
 *     GCTP_DEFINE_TYPEINFO //<-�������N���X��`���ɏ���
 *  };
 * : for impelemtation file
 *
 * GCTP_IMPLEMET_TYPEINFO(Foo, Object); //<-������cpp�t�@�C���ɏ���
 * GCTP_IMPLEMET_TYPEINFO(Bar, Foo); //<-������cpp�t�@�C���ɏ���
 *
 * // ���̏ꍇBar��Foo,Pointee�Ƀ_�E���L���X�g�\
 * @endcode
 *
 * �A�N�Z�X�w��q���܂܂�Ă��邱�Ƃɒ��ӁB���̃}�N���̒����public�ɂȂ�B
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/01/05 13:30:37
 * $Id: typeinfo.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_DECLARE_TYPEINFO								\
	public:													\
		virtual const gctp::TypeInfo &typeOfThis() const;	\
		static gctp::TypeInfo &typeinfo();

/** ���s���^����錾����}�N��
 *
 *  class Foo : public Object {
 *  public:
 *     Foo();
 *     GCTP_DEFINE_TYPEINFO //<-�������N���X��`���ɏ���
 *  };
 *
 *  class Bar : public Foo {
 *  public:
 *     Bar();
 *     GCTP_DEFINE_TYPEINFO //<-�������N���X��`���ɏ���
 *  };
 * : for impelemtation file
 *
 * GCTP_IMPLEMET_TYPEINFO(Foo, Object); //<-������cpp�t�@�C���ɏ���
 * GCTP_IMPLEMET_TYPEINFO(Bar, Foo); //<-������cpp�t�@�C���ɏ���
 *
 * // ���̏ꍇBar��Foo,Pointee�Ƀ_�E���L���X�g�\
 * @endcode
 *
 * �A�N�Z�X�w��q���܂܂�Ă��邱�Ƃɒ��ӁB���̃}�N���̒����public�ɂȂ�B
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/01/05 13:30:37
 * $Id: typeinfo.hpp,v 1.3 2005/02/21 02:20:28 takasugi Exp $
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_IMPLEMENT_TYPEINFO(_classname, _baseclassname)							\
	gctp::TypeInfo &_classname::typeinfo() {										\
		static gctp::TypeInfo typeinfo(#_classname, &_baseclassname::typeinfo());	\
		return typeinfo;															\
	}																				\
	const gctp::TypeInfo &_classname::typeOfThis() const { return typeinfo(); }

#endif

#endif //_GCTP_TYPEINFO_HPP_