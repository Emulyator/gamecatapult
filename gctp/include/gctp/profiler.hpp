#ifndef _GCTP_PROFILER_HPP_
#define _GCTP_PROFILER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult プロファイル計測クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:09:11
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/timer.hpp>
#include <string>
#include <list>
#include <iosfwd> // for std::ostream
#include <boost/format.hpp>

namespace gctp {

	/** プロファイルデータ
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:09:26
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct Profile
	{
		float rate; ///< このプロファイルエントリの使用比率
		float total; ///< このプロファイルエントリの総使用時間
		float ave; ///< このプロファイルエントリの平均使用時間
		float min; ///< このプロファイルエントリの最短使用時間
		float max; ///< このプロファイルエントリの最長使用時間
		int   count; ///< このプロファイルエントリの呼び出し回数
	};

	template<class T>
	std::basic_ostream<char, T> & operator<< (std::basic_ostream<char, T> & os, Profile const & p)
	{
	//	return os<<p.rate*100.0f<<"% "<<p.total<<"s ave:"<<p.ave<<"s min:"<<p.min<<"s max:"<<p.max<<"s  "<<p.count<<"times";
		return os << boost::basic_format<char, T>("%1$#04.1f%% %2$#08.6fs %3$#08.6fs %4$#08.6fs %5$#08.6fs %6$dtimes") % (p.rate*100.0f) % p.total % p.ave % p.min % p.max % p.count;
	}
	template<class T>
	std::basic_ostream<wchar_t, T> & operator<< (std::basic_ostream<wchar_t, T> & os, Profile const & p)
	{
	//	return os<<p.rate*100.0f<<"% "<<p.total<<"s ave:"<<p.ave<<"s min:"<<p.min<<"s max:"<<p.max<<"s  "<<p.count<<"times";
		return os << boost::basic_format<wchar_t, T>(L"%1$#04.1f%% %2$#08.6fs %3$#08.6fs %4$#08.6fs %5$#08.6fs %6$dtimes") % (p.rate*100.0f) % p.total % p.ave % p.min % p.max % p.count;
	}

	/** プロファイル計測クラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:09:26
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Profiler
	{
	public:
		/// 子階層リスト
		typedef std::list<Profiler> SubEntries;

		/// デフォルトで製作
		Profiler();
		/// 名前を指定して製作
		Profiler(const char *name);

		/// 入れ子の計測を開始
		Profiler &begin(const char *name);
		/// このエントリの計測を開始
		void begin();
		/// このエントリの計測を終了
		void end();

		/// スタックトップを返す
		Profiler &top() { if(next_) return next_->top(); return *this; }
		/// スタックトップを返す
		const Profiler &top() const { if(next_) return next_->top(); return *this; }
		/// ルートまでの距離を返す
		const int depth() const
		{
			int ret = 0;
			for(Profiler *prev = prev_; prev; prev = prev->prev_) ret++;
			return ret;
		}
		/// 名前からプロファイラを検索
		Profiler *find(const char *name);
		/// 子エントリのリスト
		SubEntries &subentries() { return subentries_; }
		/// 子エントリのリスト
		const SubEntries &subentries() const { return subentries_; }
		/// 名前
		const char *name() const { return name_.c_str(); }

		/// サブエントリをクリア
		void clear();

		Profile prof; ///< プロファイルデータ
		float other_rate; ///< このプロファイルエントリの不明使用比率
		float other_total; ///< このプロファイルエントリの総不明使用時間

	private:
		void calcStat();

		std::string name_;
		Profiler *next_;
		Profiler *prev_;
		SubEntries subentries_;
		Timer timer_;
		bool begun_;
	};

	template<class T>
	std::basic_ostream<char, T> & operator<< (std::basic_ostream<char, T> & os, Profiler const & p)
	{
		os<<"#"<<p.name()<<"\t"<<p.prof<<endl;
		for(Profiler::SubEntries::const_iterator i = p.subentries().begin(); i != p.subentries().end(); ++i)
		{
			for(int tab = i->depth(); tab > 0; tab--) os << "\t";
			os<<(*i);
		}
		if(!p.subentries().empty()) {
			for(int tab = p.depth()+1; tab > 0; tab--) os << "\t";
			os<<"--------\t"<<boost::basic_format<char, T>("%1$#04.1f%% %2$#08.6fs")%(p.other_rate*100.0f)%p.other_total<<endl;
		}
		return os;
	}
	template<class T>
	std::basic_ostream<wchar_t, T> & operator<< (std::basic_ostream<wchar_t, T> & os, Profiler const & p)
	{
		os<<"#"<<p.name()<<"\t"<<p.prof<<endl;
		for(Profiler::SubEntries::const_iterator i = p.subentries().begin(); i != p.subentries().end(); ++i)
		{
			for(int tab = i->depth(); tab > 0; tab--) os << "\t";
			os<<(*i);
		}
		if(!p.subentries().empty()) {
			for(int tab = p.depth()+1; tab > 0; tab--) os << "\t";
			os<<"--------\t"<<boost::basic_format<wchar_t, T>(L"%1$#04.1f%% %2$#08.6fs")%(p.other_rate*100.0f)%p.other_total<<endl;
		}
		return os;
	}

} //namespace gctp

#endif //_GCTP_PROFILER_HPP_