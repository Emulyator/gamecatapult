#ifndef _GCTP_ALLOCMAP_HPP_
#define _GCTP_ALLOCMAP_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 2D�󂫊Ǘ��N���X�e���v���[�g�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/07/15 2:42:16
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/quad.hpp>
#include <map>
#include <bitset>

namespace gctp {
	
	/** �񎟌��̈�̎l���؂ɂ��󂫊Ǘ��N���X�e���v���[�g
	 *
	 * @todo ���������l���؂Ȃ񂾂��Aand_map_ or_map_ ���g���č����ȋ󂫔��肪�ł���悤�ɂ���
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

		/// �u���b�N�����烌�x�����ɕϊ�
		static unsigned int blockToLevel(unsigned int w)
		{
			return _LEVEL-intlog2(w-1);
		}

		struct Block {
			Point2 pos;
			unsigned int w;
			/// �l���؂𕝗D��g���o�[�X�����Ƃ��̏�������ϊ�
			void set(unsigned int idx)
			{
				unsigned int level = quadlevel(idx, idx);
				w = 1<<(_LEVEL-level);
				pos = quadpos(idx)*static_cast<float>(w);
			}
			/// �l���؂𕝗D��g���o�[�X�����Ƃ��̏����ɕϊ�
			unsigned int index() const
			{
				unsigned int level = blockToLevel(w);
				return (level>0 ? quadprog(level-1) : 0)+quadidx(pos/static_cast<float>(w));
			}
			bool operator ! () const { return w <= 0; }
			operator bool() const { return w > 0; }
		};

		AllocBitMap() { alloc_map_.reset(); }

		/** �w��l���؃��x���Ŋm�ۉ\�ȗ̈��Ԃ�
		 *
		 * Block.w==0�͋󂫗̈�Ȃ�
		 *
		 * @param level ���x��
		 * @param ret �m�ۂ�������(�֐��̕Ԃ�l��true�̂Ƃ��L���Ȓl������)
		 * @return �m�ۂł������H
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/18 5:43:13
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		bool alloc(unsigned int level, unsigned int &ret)
		{
			unsigned int w = quadsq(_LEVEL-level);
			ret = level > 0 ? quadprog(level-1) : 0;
			for(unsigned int i = 0; i < QuadSq<_LEVEL>::value; i+=w, ret++) {
				if(!alloc_map_[i]) {
					for(unsigned int j = 0; j < w; j++) alloc_map_.set(i+j);
					return true;
				}
			}
			return false;
		}

		/** �w��̈�̃r�b�g�𕚂���
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/18 5:43:13
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		void free(unsigned int idx)
		{
			unsigned int level = 0;
			while(idx >= quadsq(level)) idx -= quadsq(level++);
			unsigned int w = quadsq(_LEVEL-level);
			for(unsigned int i = 0; i < w; i++) alloc_map_.set(idx*w+i, false);
		}

		/** �w��̈悪�󂢂Ă��邩�H
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/18 5:43:13
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		bool isFree(unsigned int idx)
		{
			unsigned int level = 0;
			while(idx >= quadsq(level)) idx -= quadsq(level++);
			unsigned int w = quadsq(_LEVEL-level);
			for(unsigned int i = 0; i < w; i++) {
				if(alloc_map_[idx*w+i]) return false;
			}
			return true;
		}

		/** �S���J��
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