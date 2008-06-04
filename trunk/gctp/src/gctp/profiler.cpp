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
		clear();
	}
	
	Profiler::Profiler(const char *name) : name_(name), next_(0), prev_(0), begun_(false)
	{
		clear();
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
			work.count++;
			work.total += lap;
			if(lap < work.min) work.min = lap;
			if(lap > work.max) work.max = lap;
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
		work.ave = work.total/work.count;
		float child_total = 0;
		float child_rate_total = 0;
		for(SubEntries::iterator i = subentries_.begin(); i != subentries_.end(); ++i)
		{
			i->end();
			i->work.rate = i->work.total/work.total;
			child_total += i->work.total;
			child_rate_total += i->work.rate;
		}
		other_work.total = work.total - child_total;
		other_work.rate = 1.0f - child_rate_total;
		other_work.count = 1;
		if(other_work.rate < 0) other_work.rate = 0;
		if(other_work.total < 0) other_work.total = 0;
		if(other_work.total < other_work.min) other_work.min = other_work.total;
		if(other_work.total > other_work.max) other_work.max = other_work.total;
	}

	void Profiler::commit()
	{
		if(work.count > 0) {
			result = work;
			other_result = other_work;
		}
		work.count = 0;
		work.total = 0;
		other_work.count = 0;
		other_work.total = 0;
		for(SubEntries::iterator i = subentries_.begin(); i != subentries_.end();)
		{
			if(i->work.count > 0) {
				i->commit();
				++i;
			}
			else {
				i = subentries_.erase(i);
			}
		}
	}

	void Profiler::clear()
	{
		result.clear();
		result.min = result.max = 0;
		other_result.clear();
		other_result.min = other_result.max = 0;
		work.clear();
		other_work.clear();
		subentries_.clear();
	}

} // namespace gctp
