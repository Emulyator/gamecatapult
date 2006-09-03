#ifndef _GCTP_PROPERTY_HPP_
#define _GCTP_PROPERTY_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �I�u�W�F�N�g�v���p�e�B�[�N���X�w�b�_�t�@�C��
 *
 * �vRTTI
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 19:29:10
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/config.hpp>
#ifdef GCTP_USE_PROPERTY
#include <vector>
#include <typeinfo>
#include <gctp/stringmap.hpp>
#include <gctp/typemap.hpp>

namespace gctp {

	typedef bool (*StaticMethod)();
	class Object;
	typedef bool (Object::*ObjectMethod)();

	/** �v���p�e�B�[�N���X
	 *
	 * �v���p�e�B�[�̈ʒu�ƌ^���L������R���e�i
	 */
	class Property {
		const std::type_info	*type_;
		void					*data_;
#ifdef __MWERKS__
		void					*dummy_;
#endif

		template<typename ValueType> friend ValueType * property_cast(Property *);
		friend class Object;

		bool do_(class Object *obj) const;

	public:
		Property();
		Property(const std::type_info &type, void *data) : type_(&type), data_(data) {}
		Property(const std::type_info &type, ObjectMethod method);
		Property(const std::type_info &type, StaticMethod method);

		const type_info& type() const { return *type_; }
		
		template<typename _T>
		Property &operator=(const _T &rhs)
		{
			if(*type_ == typeid(rhs)) *reinterpret_cast<_T *>(data_) = rhs;
			return *this;
		}
#ifndef __MWERKS__
		template<>
#endif
		Property &operator=(const Property &rhs)
		{
			type_ = rhs.type_; data_ = rhs.data_;
			return *this;
		}
	};
	
	/** property_cast�p��O�N���X
	 */
    class bad_property_cast : public std::bad_cast
    {
    public:
        virtual const char * what() const throw()
        {
            return "gctp::bad_property_cast: "
                   "failed conversion using gctp::property_cast";
        }
    };

	/** Property������ۂ̒l�����o��
	 */
    template<typename ValueType>
    ValueType * property_cast(Property * operand)
    {
        return operand && operand->type() == GCTP_TYPEID(ValueType)
                    ? reinterpret_cast<ValueType *>(operand->data_)
                    : 0;
    }

	/** Property������ۂ̒l�����o��
	 */
    template<typename ValueType>
    const ValueType * property_cast(const Property * operand)
    {
        return property_cast<ValueType>(const_cast<Property *>(operand));
    }

	/** Property������ۂ̒l�����o��
	 */
    template<typename ValueType>
    ValueType property_cast(const Property & operand)
    {
        const ValueType * result = property_cast<ValueType>(&operand);
        if(!result)
            throw bad_property_cast();
        return *result;
    }

	/** �ÓI�v���p�e�B�[�}�b�v�N���X
	 *
	 * �����񂩂�v���p�e�B�[����������悤�ɂ���
	 *
	 * Property�ƃL�[������̎��������̃I�u�W�F�N�g��蒷���i�ÓI�I�u�W�F�N�g�A���e����������Ȃǁj
	 * �K�v������B�������s���ɕύX�ł��Ȃ��B
	 */
	class PropertyMap : public StaticStringMap<const Property *> {
		friend class Object;
		typedef TypeMap<const PropertyMap *> PropertyMapDB;
		static PropertyMapDB &maps();
	public:
		static PropertyMap *getMap(const std::type_info &typeinfo);
		static void registerMap(const std::type_info &typeinfo, PropertyMap *pmap);
		void register_(const char *key, const Property &property);
	};

	/** �ÓI�v���p�e�B�[�̎����o�^�p�N���X
	 *
	 */
	class PropertyRegister {
	public:
		PropertyRegister(const std::type_info &typeinfo, PropertyMap &pmap, void (register_func)(PropertyMap &pmap)) {
			PropertyMap::registerMap(typeinfo, &pmap);
			register_func(pmap);
		}
	};

	/** ���I�v���p�e�B�[�}�b�v�N���X
	 *
	 * �����񂩂�v���p�e�B�[����������悤�ɂ���
	 *
	 * ������͎��s���ɕύX�ł���\n
	 * �ł��V���A���C�Y���Ή�����Ӗ�������ˁB
	 */
	class DynamicPropertyMap : public StringMap<const Property *> {
		friend class Object;
	public:
		DynamicPropertyMap();
		void register_(const char *key, const Property &property);
		std::vector<Property> properties_;
		DynamicPropertyMap *child_;
	};

} // namespace gctp

/** �ÓI�v���p�e�B�[�}�b�v�𐶐�����}�N��
 *
 * @code
 *  class Foo : Object {
 *     Foo();
 *     GCTP_DECLARE_FACTORY;
 *     GCTP_DECLARE_PROPERTY;	//<-�������N���X��`���ɏ���
 *     int menber1;
 *     int menber2;
 *     Pointer<Bar> ptr_member;
 *  };
 *
 * in impleremt file.
 *
 * //������cpp�t�@�C�����ɏ���
 * GCTP_IMPLEMENT_PROPERTY_BEGIN(Foo);
 *    GCTP_PROPERTY(menber1);
 *    GCTP_PROPERTY(menber2);
 *    GCTP_PROPERTY(ptr_member);
 * GCTP_IMPLEMENT_PROPERTY_END;
 *
 * Foo foo; Object *obj = &foo;
 * Pointer<Bar> bar = property_cast< Pointer<Bar> >(obj->property("ptr_member"));
 * �ȂǂƂ��ăA�N�Z�X�ł���B
 * @endcode
 *
 * �A�N�Z�X�w��q���܂܂�Ă��邱�Ƃɒ��ӁB���̃}�N���̒����private�ɂȂ�B
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2003/07/06 5:05:09
 *
 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#define GCTP_DECLARE_PROPERTY									\
	private:													\
		static void property_regfunc(gctp::PropertyMap &map);	\
		static gctp::PropertyMap _pmap_;						\
		static gctp::PropertyRegister auto_regs_property;

#define GCTP_IMPLEMENT_PROPERTY_BEGIN(T)														\
	PropertyMap T::_pmap_;																		\
	gctp::PropertyRegister T::auto_regs_property(typeid(T), T::_pmap_, T::property_regfunc);	\
	void T::property_regfunc(PropertyMap &map)	{												\
		T *null_this = 0;

#define GCTP_NAMED_PROPERTY(name, member)	{											\
	static gctp::Property _property(typeid(null_this->member), &(null_this->member));	\
	map.register_(name, _property);														\
}

#define GCTP_PROPERTY(member)	GCTP_NAMED_PROPERTY(#member, member)

#define GCTP_NAMED_METHOD(T, name, member)	{											\
	static gctp::Property _property(typeid(ObjectMethod), (ObjectMethod)&T::member);	\
	map.register_(name, _property);														\
}																						\

#define GCTP_METHOD(T, member)	GCTP_NAMED_METHOD(T, #member, member)

#define GCTP_NAMED_STATIC_METHOD(T, name, member)	{									\
	static gctp::Property _property(typeid(StaticMethod), (StaticMethod)&T::member);	\
	map.register_(name, _property);														\
}																						\

#define GCTP_STATIC_METHOD(T, member)	GCTP_NAMED_STATIC_METHOD(T, #member, member)

#define GCTP_IMPLEMENT_PROPERTY_END	\
	}
#else

#define GCTP_DECLARE_PROPERTY
#define GCTP_IMPLEMENT_PROPERTY_BEGIN(T)
#define GCTP_NAMED_PROPERTY(name, member)
#define GCTP_PROPERTY(member)
#define GCTP_NAMED_METHOD(T, name, member)
#define GCTP_METHOD(T, member)
#define GCTP_NAMED_STATIC_METHOD(T, name, member)
#define GCTP_STATIC_METHOD(T, member)
#define GCTP_IMPLEMENT_PROPERTY_END

#endif

#endif //_GCTP_PROPERTY_HPP_