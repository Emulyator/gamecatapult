/** @file
 * GameCatapult メタクラス情報クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:29:04
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/class.hpp>
#include <gctp/stringmap.hpp>
#ifdef GCTP_USE_STD_RTTI
#include <gctp/typemap.hpp>
#endif
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp {

	namespace {

		const char *global_ns__ = "";
#ifdef GCTP_USE_STD_RTTI
		// gctp独自の奴は、TypeInfoからベースクラスを辿れるので不要
		typedef TypeMap<const GCTP_TYPEINFO *> BaseClassMap;
		static BaseClassMap &basis()
		{
			static BaseClassMap basis;
			return basis;
		}
#endif
		typedef StaticStringMap<const GCTP_TYPEINFO *> ClassMap;
		typedef std::map<LStr, ClassMap> NameSpaceMap; // めんどい！ 後で考える！
		static NameSpaceMap &namespaces()
		{
			static NameSpaceMap namespaces;
			return namespaces;
		}

		void registerClass(const char *ns, const char *classname, const GCTP_TYPEINFO &typeinfo, const GCTP_TYPEINFO &basetypeinfo = GCTP_TYPEID(Object))
		{
			ClassMap &map = namespaces()[ns];
			map.put(classname, &typeinfo);
#ifdef GCTP_USE_STD_RTTI
			if(basetypeinfo != GCTP_TYPEID(Object)) basis().put(typeinfo, &basetypeinfo);
#endif
		}

	}

	Class::Class(const char *classname, const GCTP_TYPEINFO &typeinfo)
	{
		registerClass(global_ns__, classname, typeinfo);
	}
	
	Class::Class(const char *classname, const GCTP_TYPEINFO &typeinfo, const GCTP_TYPEINFO &basetypeinfo)
	{
		registerClass(global_ns__, classname, typeinfo, basetypeinfo);
	}

	Class::Class(const char *ns, const char *classname, const GCTP_TYPEINFO &typeinfo)
	{
		registerClass(ns, classname, typeinfo);
	}
	
	Class::Class(const char *ns, const char *classname, const GCTP_TYPEINFO &typeinfo, const GCTP_TYPEINFO &basetypeinfo)
	{
		registerClass(ns, classname, typeinfo, basetypeinfo);
	}

	const GCTP_TYPEINFO *Class::get(const char *ns, const char *classname)
	{
		NameSpaceMap::iterator map = namespaces().find(ns);
		if(map != namespaces().end()) return map->second.get(classname);
		return 0;
	}

	const GCTP_TYPEINFO *Class::get(const char *classname)
	{
		string str(classname);
		string::size_type i = str.rfind('.');
		if(i != string::npos) return get(str.substr(0, i).c_str(), str.substr(i+1).c_str());
		return get(global_ns__, classname);
	}
	
	Class::Name Class::get(const GCTP_TYPEINFO &typeinfo)
	{
		Class::Name ret = {0, 0};
		for(NameSpaceMap::const_iterator i = namespaces().begin(); i != namespaces().end(); ++i) {
			for(ClassMap::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
				if(*((const GCTP_TYPEINFO *)j->second)==typeinfo) {
					ret.ns = i->first.c_str();
					ret.classname = j->first.c_str();
					return ret;
				}
			}
		}
		return ret;
	}

	const GCTP_TYPEINFO *Class::getBase(const char *classname)
	{
		const GCTP_TYPEINFO *derived = get(classname);
#ifdef GCTP_USE_STD_RTTI
		if(derived) return basis().get(*derived);
		return 0;
#else
		return derived->getBase();
#endif
	}
	
	const GCTP_TYPEINFO *Class::getBase(const GCTP_TYPEINFO &typeinfo)
	{
#ifdef GCTP_USE_STD_RTTI
		return basis().get(typeinfo);
#else
		return typeinfo.getBase();
#endif
	}

} // namespace gctp
