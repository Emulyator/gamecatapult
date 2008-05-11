/** @file
 * GameCatapult レンダーツリークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/rendertree.hpp>

using namespace std;

namespace gctp { namespace scene {

	GCTP_IMPLEMENT_CLASS_NS(gctp, RenderTree, Object);

	/** 指定の階層ツリーの複製を、子ノードに連結。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/12 5:19:18
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void RenderTree::merge(const RenderTree &src)
	{
		get()->push(src->dup());
	}

	namespace {
		class RenderVisitor {
		public:
			RenderVisitor(float delta) : delta_(delta) {}
			bool operator()(const RenderTree::NodeType &n)
			{
				if(n.val->onEnter(delta_)) n.visitChildrenConst(*this);
				return n.val->onLeave(delta_);
			}
		private:
			float delta_;
		};
	}

	void RenderTree::draw(float delta) const
	{
		visit(RenderVisitor(delta));
	}

}} // namespace gctp::scene
