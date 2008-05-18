#ifndef _GCTP_LIST_HPP_
#define _GCTP_LIST_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file
 * �|�C���^�[�u���̑o�������X�g
 *
 * �R���e���c�̊m�ۂ��s��Ȃ����X�g�N���X�B@n
 * �A���P�[�^�̎w��́A�m�[�h�̊m�ە��@�̎w����Ӗ�����B
 * �R���e���c���̂��̂��ܗL���ė~�����Ȃ�Astd::list���g���B
 *
 * stl�̃R�[�h�����ւ̑Ώ��̂��߁Atemplate���g��Ȃ������B
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 * @date 2005/04/07 3:24:01
 */
#include <gctp/object.hpp>
#include <boost/iterator_adaptors.hpp>
#include <boost/iterator/reverse_iterator.hpp>

namespace gctp {

	/** �|�C���^�[���X�g
	 *
	 * ���ۃN���X�ł͂Ȃ����A�̐S�ȑ��삪protected�Ȃ̂ł��̂܂܂ł͗��p�ł��Ȃ��B\n
	 * �K���h�����K�v�B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/04/07 9:49:47
	 */
	class List : public Object {
	protected:
		struct Node;
	public:
		struct Itr : boost::iterator_adaptor<Itr, Node *, void *, boost::bidirectional_traversal_tag> {
			friend List;
			Itr( Node *p ) : boost::iterator_adaptor<Itr, Node *, void *, boost::bidirectional_traversal_tag>( p ) {}
			void increment() { base_reference() = base()->next; }
			void decrement() { base_reference() = base()->prev; }
			reference dereference() const { return base_reference()->p; }
		};

		typedef boost::reverse_iterator<Itr> RItr;

		struct ConstItr : boost::iterator_adaptor<ConstItr, const Node *, void * const, boost::bidirectional_traversal_tag> {
			friend List;
			ConstItr( const Node *p ) : boost::iterator_adaptor<ConstItr, const Node *, void * const, boost::bidirectional_traversal_tag>( p ) {}
			void increment() { base_reference() = base()->next; }
			void decrement() { base_reference() = base()->prev; }
			reference dereference() const { return base_reference()->p; }
		};

		typedef boost::reverse_iterator<ConstItr> ConstRItr;

		List();
		~List();

		void insert( Itr pos, ConstItr _begin, ConstItr _end );
		Itr erase( Itr pos );
		void erase( Itr _begin, Itr _end );

		ConstItr test() const
		{
			return begin();
		}

		void *&front() { return begin_->p; }
		void *front() const { return begin_->p; }
		void *&back() { return end_->prev->p; }
		void *back() const { return end_->prev->p; }

		Itr begin() { return Itr(begin_); }
		ConstItr begin() const { return ConstItr(begin_); }
		Itr end() { return Itr(end_); }
		ConstItr end() const { return ConstItr(end_); }

		RItr rbegin() { return RItr(end_); }
		ConstRItr rbegin() const { return ConstRItr(begin_); }
		RItr rend() { return RItr(end_); }
		ConstRItr rend() const { return ConstRItr(begin_); }

	protected:
		struct Node {
			void *p;
			Node *prev;
			Node *next;
		};
		void pushBack( void *p );
		void pushFront( void *p );
		Itr insert( Itr pos, void *p );
		virtual Node *allocNode();
		virtual void freeNode(Node *p);

	private:
		Node *begin_;
		Node *end_;
	};

}

#endif //_GCTP_LIST_HPP_