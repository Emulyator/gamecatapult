#ifndef _GCTP_SCENE_RENDERINGTREE_HPP_
#define _GCTP_SCENE_RENDERINGTREE_HPP_
/** @file
 * GameCatapult �����_�����O�c���[�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/scene/renderer.hpp>
#include <gctp/tree.hpp>

namespace gctp { namespace scene {

	/// �����_�[�m�[�h
	typedef TreeNode< Pointer<Renderer> > RenderingNode;

	/** �����_�����O�c���[�N���X
	 *
	 * Pointer<Renderer>��Tree�ł���_�ɒ���
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:38:41
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class RenderingTree : public Object, public Tree< Pointer<Renderer> >
	{
	public:
		/// �f�t�H���g�R���X�g���N�^
		RenderingTree() {}
		/// �R���X�g���N�^
		RenderingTree(Tree< Pointer<Renderer> > const & src) : Tree< Pointer<Renderer> >(src) {}
		
		/// ���[�g����
		void setUp(const Pointer<Renderer> renderer)
		{
			*this = Tree< Pointer<Renderer> >(renderer);
		}

		/// ����
		void merge(const RenderingTree &src);

		/// �m�[�h��H���ĕ`��
		void draw() const;
	
	GCTP_DECLARE_CLASS
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_RENDERINGTREE_HPP_