#ifndef _GCTP_ALLOCMAP_HPP_
#define _GCTP_ALLOCMAP_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 2D空き管理クラステンプレートヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/quad.hpp>
#include <map>
#include <bitset>

namespace gctp {
	
	/** 二次元領域の四分木による空き管理クラステンプレート
	 *
	 * @todo せっかく四分木なんだし、and_map_ or_map_ を使って高速な空き判定ができるようにする
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/18 10:37:40
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<int _LEVEL>
	class AllocBitMap {
	public:
		typedef std::bitset<QuadSq<_LEVEL>::value> AllocBitSet;
		//typedef std::bitset<QuadProg<_LEVEL-1>::value> AndBitSet;
		//typedef std::bitset<QuadProg<_LEVEL-1>::value> OrBitSet;

		/// ブロック幅からレベル数に変換
		static unsigned int blockToLevel(unsigned int w)
		{
			return _LEVEL-(intlog2(w-1)+1);
		}

		struct Block {
			Point2 pos;
			unsigned int w;
			/// 四分木を幅優先トラバースしたときの序数から変換
			void set(unsigned int idx)
			{
				if(idx > 0) {
					unsigned int level = quadlevel(idx, idx);
					w = 1<<(_LEVEL-level);
					pos = quadlocalpos(idx)*static_cast<float>(w);
				}
				else w = 0;
			}
			/// 四分木を幅優先トラバースしたときの序数に変換
			unsigned int index() const
			{
				if(w > 0) {
					unsigned int level = blockToLevel(w);
					return quadprog(level)+quadlocalidx(pos/static_cast<float>(w));
				}
				return 0;
			}
			bool operator ! () const { return w <= 0; }
			operator bool() const { return w > 0; }
		};

		AllocBitMap() { alloc_map_.reset(); }

		/** 指定四分木レベルで確保可能な領域を返す
		 *
		 * Block.w==0は空き領域なし
		 *
		 * @param level レベル
		 * @return 確保した序数(失敗は無効値の0)
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/18 5:43:13
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		unsigned int alloc(unsigned int level)
		{
			unsigned int w = quadsq(_LEVEL-level);
			unsigned int ret = quadprog(level);
			for(unsigned int i = 0; i < QuadSq<_LEVEL>::value; i+=w, ret++) {
				if(!alloc_map_[i]) {
					for(unsigned int j = 0; j < w; j++) alloc_map_.set(i+j);
					return ret;
				}
			}
			return 0;
		}

		/** 指定領域のビットを伏せる
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/18 5:43:13
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		void free(unsigned int idx)
		{
			if(idx > 0) {
				unsigned int level = 0;
				level = quadlevel(idx, idx);
				unsigned int w = quadsq(_LEVEL-level);
				for(unsigned int i = 0; i < w; i++) alloc_map_.set(idx*w+i, false);
			}
		}

		/** 指定領域が空いているか？
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/18 5:43:13
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		bool isFree(unsigned int idx)
		{
			if(idx == 0) return false;
			unsigned int level = 0;
			level = quadlevel(idx, idx);
			unsigned int w = quadsq(_LEVEL-level);
			for(unsigned int i = 0; i < w; i++) {
				if(alloc_map_[idx*w+i]) return false;
			}
			return true;
		}

		/** 全部開放
		 *
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/19 20:29:18
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		void reset()
		{
			alloc_map_.reset();
			//and_map_.reset();
			//or_map_.reset();
		}

		const AllocBitSet &allocMap() const { return alloc_map_; }

	private:
		AllocBitSet alloc_map_;
		//AndBitSet and_map_;
		//OrBitSet or_map_;
	};

} //namespace gctp

#endif //_GCTP_ALLOCMAP_HPP_