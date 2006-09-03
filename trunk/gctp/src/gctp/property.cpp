/** @file
 * GameCatapult オブジェクトプロパティークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 20:27:08
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/property.hpp>
#ifdef GCTP_USE_PROPERTY
#include <gctp/class.hpp>
#include <gctp/object.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp {

	//////////////
	// Property
	//
	bool Property::do_(Object *obj) const
	{
		if(*type_ == typeid(ObjectMethod)) {
			ObjectMethod method;
			memcpy(&method, &data_, sizeof(method));
			return (obj->*method)();
		}
		if(*type_ == typeid(StaticMethod)) {
			StaticMethod method;
			memcpy(&method, &data_, sizeof(method));
			return method();
		}
		return false;
	}

	Property::Property() : type_(&typeid(void)), data_(0) {} // これ、標準RTTIじゃないと動かせないや

	Property::Property(const type_info &type, ObjectMethod method) : type_(&type)
	{
		memcpy(&data_, &method, sizeof(method));
	}

	Property::Property(const type_info &type, StaticMethod method) : type_(&type)
	{
		memcpy(&data_, &method, sizeof(method));
	}

	//////////////
	// PropertyMap
	//
	PropertyMap *PropertyMap::getMap(const type_info &typeinfo)
	{
		return const_cast<PropertyMap *>(maps().get(typeinfo));
	}

	void PropertyMap::registerMap(const type_info &typeinfo, PropertyMap *pmap)
	{
		maps().put(typeinfo, pmap);
	}

	PropertyMap::PropertyMapDB &PropertyMap::maps()
	{
		static PropertyMapDB _maps_;
		return _maps_;
	}

	void PropertyMap::register_(const char *key, const Property &property)
	{
		put(key, &property);
	}

	//////////////
	// DynamicPropertyMap
	//
	DynamicPropertyMap::DynamicPropertyMap() : child_(0) {}

	void DynamicPropertyMap::register_(const char *key, const Property &property)
	{
		Property *_property = const_cast<Property *>(get(key));
		if(_property) *_property = property;
		else {
			properties_.push_back(property);
			_property = &properties_.back();
		}
		put(key, _property);
	}

} // namespace gctp
#endif
