/** @file
 * GameCatapult ファクトリーメソッドデータベースクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:29:04
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/factory.hpp>
#include <gctp/typemap.hpp>
#include <gctp/class.hpp>
#include <gctp/pointer.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp {

	namespace {

		typedef TypeMap<FactoryMethod> FactoryMap;
		static FactoryMap &factories()
		{
			static FactoryMap factories;
			return factories;
		}

	}

	Factory::Factory(const GCTP_TYPEINFO &typeinfo, const FactoryMethod f)
	{
		factories().put(typeinfo, f);
	}

	Ptr Factory::create(const GCTP_TYPEINFO &typeinfo)
	{
		const FactoryMethod f = factories().get(typeinfo);
		if(f) return f();
		return 0;
	}

	Ptr Factory::create(const char *classname)
	{
		const GCTP_TYPEINFO *tinfo = Class::get(classname);
		if(tinfo) return Ptr(create(*tinfo));
		dbgout << _T("Factory::create : ")<<classname<<_T("は登録されていない") << endl;
		return 0;
	}

} // namespace gctp
