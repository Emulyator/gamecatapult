/** @file
 * GameCatapult ポインター志向の双方向リスト
 *
 * どーも、gctpがでかい原因はstd::mapじゃ無いみたい？
 * なんかこれ作る気無くした…
 * これをもとにTree BinTree Mapを実現するつもりだったのに
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/list.hpp>
#ifdef GCTP_FOR_EMBEDDED
#include <boost/pool/simple_segregated_storage.hpp>
#else
#include <boost/pool/singleton_pool.hpp>
#endif

using namespace std;

namespace gctp {

#ifdef GCTP_FOR_EMBEDDED
	namespace {
		class Pool {
		public:
			enum {
				ALIGNED_CHUNKSIZE = ((sizeof(List::Node)+sizeof(void *)-1)/sizeof(void *)*sizeof(void *))
			};
			Pool(uint num) : block_(new char[ALIGNED_CHUNKSIZE*num])
			{
				pool_.segregate(reinterpret_cast<void *>(block), ALIGNED_CHUNKSIZE*num, ALIGNED_CHUNKSIZE);
			}
			~Pool()
			{
				delete[] block_;
			}
			boost::simple_segregated_storage<> pool_;
			char *block_;
		}
		boost::simple_segregated_storage<> &pool()
		{
			static Pool _pool(GCTP_LIST_MAX);
			return _pool.pool_;
		}
	}
#define MALLOC() pool().malloc()
#define FREE(p) pool().free(p)
#else
#define MALLOC() boost::singleton_pool<List, sizeof(List::Node)>::malloc()
#define FREE(p) boost::singleton_pool<List, sizeof(List::Node)>::free(p)
#endif

	List::List() : begin_(0), end_(0)
	{
		begin_ = end_ = allocNode();
		end_->p = 0;
	}

	List::~List()
	{
	}

	void List::pushBack( void *p )
	{
		Node *node = allocNode();
		node->p = p;
		node->prev = end_->prev;
		node->next = end_;
		end_->prev = node;
		if(begin_ == end_) begin_ = node;
	}

	void List::pushFront( void *p )
	{
		Node *node = allocNode();
		node->p = p;
		node->prev = 0;
		node->next = begin_;
		begin_ = node;
	}

	List::Itr List::insert( Itr pos, void *p )
	{
		Node *node = allocNode();
		node->p = p;
		node->next = pos.base();
		node->prev = pos.base()->prev;
		pos.base()->prev = node;
		return Itr(node);
	}

	void List::insert( Itr pos, ConstItr _begin, ConstItr _end )
	{
		for(ConstItr i = _begin; i != _end; ++i) {
			insert(pos, const_cast<void *>(*i));
		}
	}

	List::Itr List::erase( Itr pos )
	{
		Node *ret = pos.base()->next;
		ret->prev = pos.base()->prev;
		if(ret->prev) ret->prev->next = ret;
		freeNode(pos.base());
		return Itr(ret);
	}

	void List::erase( Itr _begin, Itr _end )
	{
		for(Itr i = _begin; i != _end;) {
			i = erase(i);
		}
	}

	List::Node *List::allocNode()
	{
		return reinterpret_cast<List::Node *>(MALLOC());
	}

	void List::freeNode(List::Node *p)
	{
		FREE(p);
	}

} // namespace gctp
