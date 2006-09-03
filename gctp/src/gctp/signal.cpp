/** @file
 * GameCatapult シグナル・スロットクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:10:51
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/signal.hpp>

using namespace std;

namespace gctp {

	namespace {

		struct Compare {
			bool operator()(const Handle<Slot> &lhs, const Handle<Slot> &rhs)
			{
				return !rhs || (lhs && (lhs->priority() > rhs->priority()));
			}
		};

	}

	bool Signal::isUnique(const Slot &slot)
	{
		for(SlotList::iterator i = slots_.begin(); i != slots_.end(); ++i) {
			if((*i).get() == &slot) return false;
		}
		return true;
	}

	void Signal::connect(const Slot &slot)
	{
		slots_.push_back(Handle<Slot>(&const_cast<Slot &>(slot)));
	}

	void Signal::disconnect(const Slot &slot)
	{
		slots_.remove(Handle<Slot>(&const_cast<Slot &>(slot)));
	}

	std::size_t Signal::num() const
	{
		return slots_.size();
	}
	
	void Signal::sort()
	{
		if(flags_&SORT) slots_.sort(Compare());
	}

	bool Signal::canInvoke(const Slot &slot)
	{
		if((flags_&MASK)&&(!(mask_&slot.mask()))) return false;
		return true;
	}

} // namespace gctp
