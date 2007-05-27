/** @file
 * GameCatapult オブジェクトスーパークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:29:04
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/class.hpp>
#include <gctp/object.hpp>
#include <gctp/pointer.hpp>
#include <gctp/property.hpp>
#include <gctp/mutex.hpp>

using namespace std;

namespace gctp {
	
	namespace {

		Class object_class("gctp", "Object", GCTP_TYPEID(Object));

	}

	void AbstractDeleter::destroy(class Object *p)
	{
		p->~Object();
	}
	
	void Object::expire() const
	{
		if(stub_) {
			if(stub_->addRef()) {
				stub_->p_ = 0;
				if(stub_->decRef()) delete stub_;
				else if(stub_->decRef()) delete stub_;
			}
			stub_ = 0;
		}
	}

	Object::~Object()
	{
		{
			Object::AutoLock al(mutex_);
			expire();
		}
		if(mutex_) delete mutex_;
		// Objectの削除後には、Stubのsynchronizeを解除する手段は無い…
	}
	
	void Object::synchronize(bool yes)
	{
		if(yes && !mutex_) {
			mutex_ = new Mutex;
		}
		if(!yes && mutex_) {
			delete mutex_; mutex_ = 0;
		}
		if(stub_) stub_->synchronize(yes);
	}

#ifdef GCTP_USE_PROPERTY
	Property Object::property(const char *property_name)
	{
		const Property *pp;
# ifdef GCTP_USE_DYNAMIC_PROPERTY
		for(DynamicPropertyMap *dpmap = dynamic_property_; dpmap; dpmap = dpmap->child_) {
			pp = dpmap->get(property_name);
			if(pp) return *pp;
		}
# endif
		for(const GCTP_TYPEINFO *typeinfo = &GCTP_TYPEID(*this); typeinfo; typeinfo = Class::getBase(*typeinfo)) {
			PropertyMap *pmap = PropertyMap::getMap(*typeinfo);
			if(pmap) {
				pp = pmap->get(property_name);
				if(pp) {
					Property ret(*pp);
					ret.data_ = (void *)(((std::ptrdiff_t)pp->data_)+((std::ptrdiff_t)this));
					return ret;
				}
			}
		}
		return Property();
	}

	bool Object::method(const char *method_name)
	{
		const Property *pp;
# ifdef GCTP_USE_DYNAMIC_PROPERTY
		for(DynamicPropertyMap *dpmap = dynamic_property_; dpmap; dpmap = dpmap->child_) {
			pp = dpmap->get(method_name);
			if(pp) return pp->do_(this);
		}
# endif
		for(const GCTP_TYPEINFO *typeinfo = &GCTP_TYPEID(*this); typeinfo; typeinfo = Class::getBase(*typeinfo)) {
			PropertyMap *pmap = PropertyMap::getMap(*typeinfo);
			if(pmap) {
				pp = pmap->get(method_name);
				if(pp) return pp->do_(this);
			}
		}
		return false;
	}

# ifdef GCTP_USE_DYNAMIC_PROPERTY
	void Object::setDynamicProperty(DynamicPropertyMap *dpmap)
	{
		if(dpmap) {
			if(dynamic_property_) dpmap->child_ = dynamic_property_;
			dynamic_property_ = dpmap;
		}
		else if(dynamic_property_) dynamic_property_ = dynamic_property_->child_;
	}
# endif
#endif

} // namespace gctp
