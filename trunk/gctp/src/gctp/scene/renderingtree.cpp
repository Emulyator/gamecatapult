/** @file
 * GameCatapult レンダリングツリークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/renderingtree.hpp>

using namespace std;

namespace gctp { namespace scene {

	GCTP_IMPLEMENT_CLASS_NS(gctp, RenderingTree, RenderingNode);

	/** 指定の階層ツリーの複製を、子ノードに連結。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/12 5:19:18
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void RenderingTree::merge(const RenderingTree &src)
	{
		get()->push(src->dup());
	}

	namespace {
		bool renderingVisitor(const RenderingTree::NodeType &n)
		{
			if(n.val->onEnter()) n.visitChildrenConst(renderingVisitor);
			return n.val->onLeave();
		}
	}

	void RenderingTree::draw() const
	{
		visit(renderingVisitor);
	}

}} // namespace gctp::scene
