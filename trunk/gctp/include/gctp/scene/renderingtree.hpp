#ifndef _GCTP_SCENE_RENDERINGTREE_HPP_
#define _GCTP_SCENE_RENDERINGTREE_HPP_
/** @file
 * GameCatapult レンダリングツリークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/scene/renderer.hpp>
#include <gctp/tree.hpp>

namespace gctp { namespace scene {

	/// レンダーノード
	typedef TreeNode< Pointer<Renderer> > RenderingNode;

	/** レンダリングツリークラス
	 *
	 * Pointer<Renderer>のTreeである点に注意
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:38:41
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class RenderingTree : public Object, public Tree< Pointer<Renderer> >
	{
	public:
		/// デフォルトコンストラクタ
		RenderingTree() {}
		/// コンストラクタ
		RenderingTree(Tree< Pointer<Renderer> > const & src) : Tree< Pointer<Renderer> >(src) {}
		
		/// ルート製作
		void setUp(const Pointer<Renderer> renderer)
		{
			*this = Tree< Pointer<Renderer> >(renderer);
		}

		/// 結合
		void merge(const RenderingTree &src);

		/// ノードを辿って描画
		void draw() const;
	
	GCTP_DECLARE_CLASS
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_RENDERINGTREE_HPP_