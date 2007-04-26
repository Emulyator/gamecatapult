#ifndef _GCTP_PROFILER_HPP_
#define _GCTP_PROFILER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �v���t�@�C���v���N���X�w�b�_�t�@�C��
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

	/** �v���t�@�C���f�[�^
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:09:26
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct Profile
	{
		float rate; ///< ���̃v���t�@�C���G���g���̎g�p�䗦
		float total; ///< ���̃v���t�@�C���G���g���̑��g�p����
		float ave; ///< ���̃v���t�@�C���G���g���̕��ώg�p����
		float min; ///< ���̃v���t�@�C���G���g���̍ŒZ�g�p����
		float max; ///< ���̃v���t�@�C���G���g���̍Œ��g�p����
		int   count; ///< ���̃v���t�@�C���G���g���̌Ăяo����
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

	/** �v���t�@�C���v���N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:09:26
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Profiler
	{
	public:
		/// �q�K�w���X�g
		typedef std::list<Profiler> SubEntries;

		/// �f�t�H���g�Ő���
		Profiler();
		/// ���O���w�肵�Đ���
		Profiler(const char *name);

		/// ����q�̌v�����J�n
		Profiler &begin(const char *name);
		/// ���̃G���g���̌v�����J�n
		void begin();
		/// ���̃G���g���̌v�����I��
		void end();

		/// �X�^�b�N�g�b�v��Ԃ�
		Profiler &top() { if(next_) return next_->top(); return *this; }
		/// �X�^�b�N�g�b�v��Ԃ�
		const Profiler &top() const { if(next_) return next_->top(); return *this; }
		/// ���[�g�܂ł̋�����Ԃ�
		const int depth() const
		{
			int ret = 0;
			for(Profiler *prev = prev_; prev; prev = prev->prev_) ret++;
			return ret;
		}
		/// ���O����v���t�@�C��������
		Profiler *find(const char *name);
		/// �q�G���g���̃��X�g
		SubEntries &subentries() { return subentries_; }
		/// �q�G���g���̃��X�g
		const SubEntries &subentries() const { return subentries_; }
		/// ���O
		const char *name() const { return name_.c_str(); }

		/// �T�u�G���g�����N���A
		void clear();

		Profile prof; ///< �v���t�@�C���f�[�^
		float other_rate; ///< ���̃v���t�@�C���G���g���̕s���g�p�䗦
		float other_total; ///< ���̃v���t�@�C���G���g���̑��s���g�p����

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