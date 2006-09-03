#ifndef _GCTP_MEMORY_HPP_
#define _GCTP_MEMORY_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file
 * 組み込み向けメモリ管理
 *
 * まだreallocとか怪しい
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 * @date 2005/04/07 3:24:01
 */
#include <gctp/def.hpp>
// 微小領域用
#include <boost/pool/simple_segregated_storage.hpp>
#ifdef GCTP_MEMORY_CHECK
#ifdef GCTP_MEMORY_CHECK_CPPUNIT
#include <cppunit/TestAssert.h>
#define ASSERT(_m)	CPPUNIT_ASSERT(_m)
#else
#include <assert.h>
#define ASSERT(_m)	assert(_m)
#endif
#endif

namespace gctp {

	/** 組み込み向けメモリ管理
	 *
	 * @note アライメントは2^nである必要がある。
	 * また、それは最低ブロックサイズ以上である必要があるため、実質最低値は32
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/04/07 9:49:47
	 */
	template<typename _SizeType = std::size_t, _SizeType _MIN_ALIGNMENT = sizeof(void *)*8, _SizeType _SMALLMEMORY_CHUNKSIZE = _MIN_ALIGNMENT*2, int SMALLMEMORY_DEFAULT_RATE = 5>
	class Memory {
	public:
		typedef _SizeType SizeType;
		enum {
			MIN_ALIGNMENT = _MIN_ALIGNMENT,
			SMALLMEMORY_CHUNKSIZE = _SMALLMEMORY_CHUNKSIZE,
			USE_DEFAULT	= -1,
		};

		/// 空で初期化
		Memory() : memory_(0), begin_(0), alloc_count_(0), small_alloc_count_(0), pool_size_(0), pool_memory_begin_(0), pool_memory_end_(0), parent_(0) {}
		/// メモリを任せる
		Memory(void *memory, SizeType size, SizeType smallsize = USE_DEFAULT) : memory_(0), begin_(0), alloc_count_(0), small_alloc_count_(0), pool_size_(0), pool_memory_begin_(0), pool_memory_end_(0), parent_(0)
		{
			entrust(memory, size, smallsize);
		}

		/// メモリを任せる
		bool entrust(void *memory, SizeType size, SizeType smallsize = USE_DEFAULT)
		{
			initialize(memory, size);
			if(smallsize == USE_DEFAULT) smallsize = (SizeType)(size*((float)SMALLMEMORY_DEFAULT_RATE/(float)100));
			if(smallsize < size && !empty()) {
				SizeType pool_size = smallsize/SMALLMEMORY_CHUNKSIZE*SMALLMEMORY_CHUNKSIZE;
				if(pool_size > 0) {
					pool_memory_begin_ = malloc(pool_size);
					if(pool_memory_begin_) {
						pool_memory_end_ = (void *)((SizeType)pool_memory_begin_ + pool_size);
						pool_size_ = pool_size/SMALLMEMORY_CHUNKSIZE;
						pool_.add_block(pool_memory_begin_, pool_size, SMALLMEMORY_CHUNKSIZE);
					}
				}
			}
			return !empty();
		}

		/// メモリ管理を止める（以降のfreeでなにもしなくなる）
		void purge()
		{
			memory_ = 0;
			begin_ = 0;
		}

		/// 管理しているメモリがあるか？
		bool empty() const { return memory_ == 0; }

		/// malloc
		void *malloc(SizeType size)
		{
			if(!empty()) {
				if(size <= SMALLMEMORY_CHUNKSIZE) {
					void *ret = poolAlloc();
					if(ret) return ret;
				}
				return alloc(size);
			}
			return 0;
		}

		/** realloc
		 * @note prevが0の場合、mallocと同じように動作する。
		 * prev != 0 で size==0 の場合、free(prev)と等価。
		 * メモリ確保に失敗した場合、既存のprevを変更しない、はず。
		 */
		void *realloc(void *prev, SizeType size)
		{
			if(!empty()) {
				if(prev) {
					if(size) {
						if(isInPool(prev)) {
							if(size <= SMALLMEMORY_CHUNKSIZE) return prev;
							else {
								void *ret = malloc(size);
								if(ret) {
									memcpy(ret, prev, SMALLMEMORY_CHUNKSIZE);
									poolFree(prev);
									return ret;
								}
							}
						}
						else {
							if(size <= SMALLMEMORY_CHUNKSIZE) {
								void *ret = poolAlloc();
								if(ret) {
									memcpy(ret, prev, SMALLMEMORY_CHUNKSIZE);
									free(prev);
									return ret;
								}
							}
							return resize(prev, size);
						}
					}
					else free(prev);
					return 0;
				}
				else if(size) return malloc(size);
				return alloc(size);
			}
			return 0;
		}

		/** アライメント指定malloc
		 * @note アライメントは2^nである必要がある。
		 */
		void *memalign(SizeType size, SizeType alignment)
		{
			if(!empty()) {
				alignment = align(alignment, MIN_ALIGNMENT);
				if(size <= SMALLMEMORY_CHUNKSIZE && alignment <= SMALLMEMORY_CHUNKSIZE) {
					void *ret = poolAlloc();
					if(ret) return ret;
				}
				if(alignment <= MIN_ALIGNMENT) return alloc(size);
				return allocAligned(size, alignment);
			}
			return 0;
		}

		/// free
		void free(void *chunk)
		{
			if(!empty() && chunk && memory_ < chunk && chunk < (void *)((SizeType)(memory_)+size_)) {
				if(!poolFree(chunk)) dealloc(chunk);
			}
		}

		/// 子をプッシュ
		Memory *push(SizeType smallsize = USE_DEFAULT)
		{
			if(!empty()) {
				size_t size;
				Memory *memory = reinterpret_cast<Memory *>(allocChildMemory(size));
				if(memory) {
					if(size > MIN_ALIGNMENT) return new (memory) Memory(this, memory+1, size-sizeof(Memory), smallsize);
					else dealloc(memory);
				}
			}
			return 0;
		}

		/** メモリフレームからポップ
         * @return 親が返る
		 */
		Memory *pop()
		{
			purge();
			Memory *ret = parent_;
			ret->free(this);
			return ret;
		}

		/// 親Memory
		const Memory *parent() const { return parent_; }

		static inline SizeType align(SizeType raw, SizeType alignment)
		{
			return (raw+alignment-1)&~(alignment-1);
		}
		
	private:
		void *memory_;
		SizeType size_;
		struct Block;
		Block *begin_/*, *mid_, *end_*/;
		boost::simple_segregated_storage<SizeType> pool_;
		SizeType pool_size_;
		void *pool_memory_begin_, *pool_memory_end_;
		int alloc_count_;
		int small_alloc_count_;
		Memory *parent_;

		/// 子メモリとして初期化
		Memory(Memory *parent, void *memory, SizeType size, SizeType smallsize = USE_DEFAULT) : memory_(0), begin_(0), alloc_count_(0), small_alloc_count_(0), pool_size_(0), pool_memory_begin_(0), pool_memory_end_(0), parent_(parent)
		{
			entrust(memory, size, smallsize);
		}

		bool isInPool(void *chunk) const
		{
			// poolは満杯の場合でも、pool_.empty()== trueを返しやがる！
			//return (!pool_.empty() && pool_memory_begin_ <= chunk && chunk < pool_memory_end_);
			return (pool_memory_begin_ && pool_memory_begin_ <= chunk && chunk < pool_memory_end_);
		}

		void *poolAlloc()
		{
			if(!pool_.empty()) {
				void *ret = pool_.malloc();
				if(ret) {
					alloc_count_++;
					small_alloc_count_++;
					return ret;
				}
			}
			return 0;
		}

		bool poolFree(void *chunk)
		{
			if(isInPool(chunk)) {
				pool_.free(chunk);
				alloc_count_--;
				small_alloc_count_--;
				return true;
			}
			return false;
		}

		struct Block {
#ifdef GCTP_MEMORY_CHECK
			void *sentinel;
#endif
			SizeType length; // Block単位の長さ(自分も含めるので、length>0)
			union {
				struct FreeList {
					Block *prev;
					Block *next;
				} list;
				struct Header {
					SizeType alignment;
#ifdef GCTP_MEMORY_CHECK
					void *pad[MIN_ALIGNMENT/sizeof(void *)-4]; // パディング
#else
					void *pad[MIN_ALIGNMENT/sizeof(void *)-3]; // パディング
#endif
					Block *head; // 便宜上。アライメントするときはbodyがこの位置にこないことが多い。
				} header;
			};

			void chainAfter(Block *where)
			{
				list.prev = where;
				list.next = where->list.next;
				where->list.next = this;
				if(list.next) list.next->list.prev = this;
			}
			void chainBefore(Block *where)
			{
				list.next = where;
				list.prev = where->list.prev;
				where->list.prev = this;
				if(list.prev) list.prev->list.next = this;
			}
			void remove()
			{
				if(list.prev) list.prev->list.next = list.next;
				if(list.next) list.next->list.prev = list.prev;
#ifdef GCTP_MEMORY_CHECK
				sentinel = (void*)(int)0xFEEDBEEF;
#endif
			}
			void split(SizeType len)
			{
				if(len+1 < length) { // 長さ１のブロックを作るのは無駄なので
					(this + len)->chainAfter(this);
					(this + len)->length = length-len;
#ifdef GCTP_MEMORY_CHECK
					(this + len)->sentinel = (void *)(int)0xDEADBEEF;
#endif
					length = len;
				}
			}
			Block *merge()
			{
				Block *_this = this;
				if(list.prev && ((list.prev + list.prev->length) == this)) {
					list.prev->list.next = list.next;
					list.prev->length += length;
					if(list.next) list.next->list.prev = list.prev;
					_this = list.prev;
				}
				if(_this->list.next && ((_this + _this->length) == _this->list.next)) {
					_this->length += _this->list.next->length;
					_this->list.next = _this->list.next->list.next;
					if(_this->list.next) _this->list.next->list.prev = _this;
				}
				return _this;
			}
			void *body()
			{
				return (void *)align((SizeType)(this+1), header.alignment);
			}
		};

		void initialize(void *memory, SizeType size)
		{
			if(memory && size > sizeof(Block)) {
				memory_ = (void *)align((SizeType)memory, MIN_ALIGNMENT);
				begin_ = reinterpret_cast<Block *>(memory_);
				begin_->list.prev = begin_->list.next = 0;
				begin_->length = (size-((SizeType)memory_-(SizeType)memory))/sizeof(Block);
#ifdef GCTP_MEMORY_CHECK
				begin_->sentinel = (void*)(int)0xDEADBEEF;
#endif
				size_ = begin_->length*sizeof(Block);
			}
			alloc_count_ = 0;
			small_alloc_count_ = 0;
		}

		static Block *findFirstFit(Block *_begin, SizeType length)
		{
			for(Block *i = _begin; i != 0; i = i->list.next) {
				if(length <= i->length) return i;
			}
			return 0;
		}

		static Block *findLargest(Block *_begin, SizeType &length)
		{
			Block *largest = 0;
			length = 0;
			for(Block *i = _begin; i != 0; i = i->list.next) {
				if(length < i->length) {
					largest = i;
					length = i->length;
				}
			}
			return largest;
		}

		static Block *findForward(Block *_begin, Block *block)
		{
			if(block > _begin) {
				for(Block *i = _begin; i; i = i->list.next) {
					if( i->list.next == 0 || (i < block && block < i->list.next) ) return i;
				}
			}
			return 0;
		}

#ifdef GCTP_MEMORY_CHECK
		// 整合性チェック
		void check() const
		{
			for(Block *i = begin_; i; i = i->list.next) {
				ASSERT(i->sentinel == (void *)(int)0xDEADBEEF);
				if((SizeType)(i+i->length) < (SizeType)memory_+size_/sizeof(Block)*sizeof(Block)) {
					ASSERT((i+i->length)->sentinel == (void *)(int)0xDEADBEEF || (i+i->length)->sentinel == (void *)(int)0xFEEDBEEF);
				}
				if(i->list.prev) {
					ASSERT(i->list.prev->sentinel == (void *)(int)0xDEADBEEF);
					ASSERT(memory_ < i->list.prev && i->list.prev < (void *)((SizeType)(memory_)+size_));
				}
				if(i->list.next) {
					ASSERT(i->list.next->sentinel == (void *)(int)0xDEADBEEF);
					ASSERT(memory_ < i->list.next && i->list.next < (void *)((SizeType)(memory_)+size_));
				}
			}
		}
#endif

		void useThisBlock(Block *block, SizeType alignment, SizeType length)
		{
			block->split(length);
			block->remove();
			if(block->list.next) block->list.next = block->list.next->merge();
			if(begin_ == block) begin_ = block->list.next;
			block->header.alignment = alignment;
			*(((void **)block->body())-1) = block;
		}

		void *allocChildMemory(SizeType &size)
		{
#ifdef GCTP_MEMORY_CHECK
			check();
#endif
			SizeType length;
			Block *free_block = findLargest(begin_, length);
			if(free_block) {
				useThisBlock(free_block, MIN_ALIGNMENT, length);
				alloc_count_++;
				size = (length-1)*sizeof(Block);
				return free_block->body();
			}
			return 0;
		}

		void *alloc(SizeType size)
		{
#ifdef GCTP_MEMORY_CHECK
			check();
#endif
			SizeType length = (size+sizeof(Block)-1)/sizeof(Block)+1;
			Block *free_block = findFirstFit(begin_, length);
			if(free_block) {
				useThisBlock(free_block, MIN_ALIGNMENT, length);
				alloc_count_++;
				return free_block->body();
			}
			return 0;
		}

		void *allocAligned(SizeType size, SizeType alignment)
		{
#ifdef GCTP_MEMORY_CHECK
			check();
#endif
			SizeType length = (size+sizeof(Block)-1)/sizeof(Block)+1;
			Block *_begin = begin_;
			while(_begin) {
				Block *free_block = findFirstFit(_begin, length);
				if(free_block) {
					SizeType aligned = align((SizeType)(free_block+1), alignment);
					SizeType aligned_length = ((aligned-(SizeType)(free_block+1)+size)+sizeof(Block)-1)/sizeof(Block)+1;
					if(aligned_length <= free_block->length) {
						useThisBlock(free_block, alignment, aligned_length);
						alloc_count_++;
						return free_block->body();
					}
					_begin = _begin->list.next;
				}
				else break;
			}
			return 0;
		}

		void dealloc(void *chunk)
		{
#ifdef GCTP_MEMORY_CHECK
			check();
#endif
			Block *for_free = reinterpret_cast<Block *>(*(((void **)chunk)-1));
#ifdef GCTP_MEMORY_CHECK
			ASSERT(for_free->sentinel == (void *)(int)0xFEEDBEEF);
#endif
			Block *forward_block = findForward(begin_, for_free);
			if(forward_block) {
				for_free->chainAfter(forward_block);
				for_free = for_free->merge();
			}
			else {
				for_free->chainBefore(begin_);
				begin_ = for_free = for_free->merge();
			}
#ifdef GCTP_MEMORY_CHECK
			for_free->sentinel = (void *)(int)0xDEADBEEF;
#endif
			alloc_count_--;
		}

		void *resize(void *chunk, SizeType size)
		{
#ifdef GCTP_MEMORY_CHECK
			check();
#endif
			Block *for_resize = reinterpret_cast<Block *>(*(((void **)chunk)-1));
#ifdef GCTP_MEMORY_CHECK
			ASSERT(for_resize->sentinel == (void *)(int)0xFEEDBEEF);
#endif
			SizeType offset = (SizeType)chunk-(SizeType)for_resize;
			SizeType alignment = for_resize->header.alignment;
			SizeType aligned_length = (offset+size+sizeof(Block)-1)/sizeof(Block)+1;
			Block *forward_block = findForward(begin_, for_resize);
			if(aligned_length < for_resize->length) { // 縮小
				if(forward_block) {
					for_resize->chainAfter(forward_block);
					useThisBlock(for_resize, alignment, aligned_length);
				}
				else {
					for_resize->chainBefore(begin_);
					begin_ = for_resize;
					useThisBlock(for_resize, alignment, aligned_length);
				}
			}
			else if(aligned_length > for_resize->length) { // 拡張
				if(forward_block) for_resize->chainAfter(forward_block);
				else {
					for_resize->chainBefore(begin_);
					begin_ = for_resize;
				}
			
				SizeType original_length = for_resize->length;
				Block *new_block = for_resize->merge();
				if(aligned_length <= new_block->length) {
					useThisBlock(new_block, alignment, aligned_length);
					if(new_block != for_resize) {
						SizeType copy_size = (for_resize->length-1)*sizeof(Block)-((SizeType)chunk-(SizeType)for_resize);
						memmove(new_block->body(), chunk, copy_size);
						for_resize = new_block;
					}
				}
				else {
					// 巻き戻し…
					if(new_block != for_resize) new_block->split(for_resize-new_block);
					useThisBlock(for_resize, alignment, original_length);
					// 別領域に確保してコピー後消す
					void *new_heap = (alignment>MIN_ALIGNMENT)? alloc(size) : allocAligned(size, alignment);
					if(new_heap) {
						SizeType copy_size = (for_resize->length-1)*sizeof(Block)-((SizeType)chunk-(SizeType)for_resize);
						memcpy(new_heap, chunk, copy_size);
						dealloc(chunk);
					}
					return new_heap;
				}
			}
			return for_resize->body();
		}

	public:
		/// _msize
		SizeType msize(void *chunk) const
		{
			if(!empty() && chunk && memory_ < chunk && chunk < (void *)((SizeType)(memory_)+size_)) {
				if(isInPool(chunk)) return SMALLMEMORY_CHUNKSIZE;
#ifdef GCTP_MEMORY_CHECK
				ASSERT(reinterpret_cast<Block *>(*(((void **)chunk)-1))->sentinel == (void *)(int)0xFEEDBEEF);
#endif
				return (reinterpret_cast<Block *>(*(((void **)chunk)-1))->length-1) * sizeof(Block);
			}
			return -1;
		}

		struct Info {
			SizeType heap_size;
			int free_block_num;
			SizeType largest_block_size;
			SizeType total_free_size;
			int alloc_count;
			int small_alloc_count;
			SizeType pool_free;
		};

		/// mallinfo
		Info info() const
		{
			Info ret;
			ret.heap_size = size_;
			ret.free_block_num = 0;
			ret.largest_block_size = 0;
			ret.total_free_size = 0;
			Block *block = begin_;
			while(block) {
				ret.free_block_num++;
				if(ret.largest_block_size < block->length) ret.largest_block_size = block->length;
				ret.total_free_size += block->length;
				block = block->list.next;
			}
			ret.largest_block_size *= sizeof(Block);
			ret.total_free_size *= sizeof(Block);
			ret.alloc_count = alloc_count_;
			ret.small_alloc_count = small_alloc_count_;
			ret.pool_free = pool_size_ - small_alloc_count_;
			ret.total_free_size += ret.pool_free*sizeof(SMALLMEMORY_CHUNKSIZE);
			if(ret.pool_free > 0 && ret.largest_block_size < SMALLMEMORY_CHUNKSIZE) ret.largest_block_size = SMALLMEMORY_CHUNKSIZE;
			return ret;
		}
	};
}

#endif //_GCTP_MEMORY_HPP_