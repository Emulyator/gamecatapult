/**@file
 * GameCatapult プロファイル計測クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/03/21 6:42:54
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/profiler.hpp>

namespace gctp {
	
	Profiler::Profiler() : next_(0), prev_(0), begun_(false)
	{
		prof.count = 0;
		prof.rate = 1.0f;
		prof.total = prof.ave = prof.max = 0.0f;
		prof.min = FLT_MAX;
	}
	
	Profiler::Profiler(const char *name) : name_(name), next_(0), prev_(0), begun_(false)
	{
		prof.count = 0;
		prof.rate = 1.0f;
		prof.total = prof.ave = prof.max = 0.0f;
		prof.min = FLT_MAX;
	}

	Profiler &Profiler::begin(const char *name)
	{
		Profiler *ret = 0;
		for(SubEntries::iterator i = subentries_.begin(); i != subentries_.end(); ++i) {
			if(i->name_ == name) {
				ret = &(*i);
				break;
			}
		}
		if(!ret) {
			subentries_.push_back(Profiler());
			ret = &subentries_.back();
			ret->name_ = name;
		}
		next_ = ret;
		ret->prev_ = this;
		ret->begin();
		return *ret;
	}

	void Profiler::begin()
	{
		if(!begun_) {
			begun_ = true;
			timer_.reset();
		}
	}

	void Profiler::end()
	{
		if(begun_) {
			begun_ = false;
			float lap = timer_.elapsed();
			prof.count++;
			prof.total += lap;
			if(lap < prof.min) prof.min = lap;
			if(lap > prof.max) prof.max = lap;
			calcStat();
			if(prev_) prev_->next_ = 0;
		}
	}

	Profiler *Profiler::find(const char *name)
	{
		if(name_ == name) return this;
		for(SubEntries::iterator i = subentries_.begin(); i != subentries_.end(); ++i)
		{
			Profiler *ret = i->find(name);
			if(ret) return ret;
		}
		return 0;
	}

	void Profiler::calcStat()
	{
		prof.ave = prof.total/prof.count;
		float child_total = 0;
		float child_rate_total = 0;
		for(SubEntries::iterator i = subentries_.begin(); i != subentries_.end(); ++i)
		{
			i->end();
			i->prof.rate = i->prof.total/prof.total;
			child_total += i->prof.total;
			child_rate_total += i->prof.rate;
		}
		other_total = prof.total - child_total;
		other_rate = 1.0f - child_rate_total;
	}

	void Profiler::clear()
	{
		prof.count = 0;
		prof.rate = 1.0f;
		prof.total = prof.ave = prof.max = 0.0f;
		prof.min = FLT_MAX;

		subentries_.clear();
	}

} // namespace gctp
