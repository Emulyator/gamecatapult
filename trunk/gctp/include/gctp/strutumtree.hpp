#ifndef _GCTP_STRUTUMTREE_HPP_
#define _GCTP_STRUTUMTREE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �K�w�c���[�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/strutumnode.hpp>
#include <gctp/class.hpp>
#include <iosfwd>               // for std::basic_ostream

namespace gctp {

	/** �K�w�c���[�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:38:41
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class StrutumTree : public Object, public Tree<Strutum>
	{
	public:
		/// �f�t�H���g�R���X�g���N�^
		StrutumTree() {}
		/// �R���X�g���N�^
		StrutumTree(Tree<Strutum> const & src) : Tree<Strutum>(src) {}

		/// ���[�g����
		void setUp(const Matrix &val = MatrixC(true))
		{
			*this = Tree<Strutum>(val);
		}

		/// ����
		void merge(const StrutumTree &src);

		/// �v�Z�ς݃}�g���N�X���Z�b�g
		StrutumTree &setTransform();
		/// �K�w�����ׂĒP�ʍs���
		StrutumTree &setIdentity();

		/// �C���f���g�t���ŏo��
		std::ostream &print(std::ostream &os) const;

	GCTP_DECLARE_CLASS
	};

} // namespace gctp

#endif //_GCTP_STRUTUMTREE_HPP_