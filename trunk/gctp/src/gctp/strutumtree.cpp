/** @file
 * GameCatapult 階層ツリークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/pointer.hpp>
#include <gctp/strutumtree.hpp>

using namespace std;

namespace gctp {

	GCTP_IMPLEMENT_CLASS_NS(gctp, StrutumTree, Object);

	/** 指定の階層ツリーの複製を、子ノードに連結。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/12 5:19:18
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void StrutumTree::merge(const StrutumTree &src)
	{
		get()->push(src->dup());
	}

	namespace {
		class PrintVisitor {
			std::ostream &os_;
			int	indent_;
		public:
			PrintVisitor(std::ostream &os) : os_(os), indent_(0) {}
			bool operator()(const StrutumTree::NodeType &n)
			{
				for(int i = 0; i < indent_; i++) os_ << "\t";
				os_ << n.val << endl;
				indent_++;
				n.visitChildrenConst(*this);
				indent_--;
				return true;
			}
		};

		bool transformVisitor(StrutumTree::NodeType &n)
		{
			n.val.preupdate();
			if(n.parent()) {
				if(n.parent()->val.isUpdated() || n.val.isDirty()) n.val.update(n.parent()->val.wtm());
			}
			else if(n.val.isDirty()) {
				n.val.wtm() = n.val.lcm();
				n.val.refresh();
			}
			return true;
		}

		bool transformVisitor_(StrutumTree::NodeType &n)
		{
			n.val.preupdate();
			if(n.parent()) {
				if(n.parent()->val.isUpdated() || n.val.isDirty()) n.val.update(n.parent()->val.wtm());
			}
			else if(n.val.isDirty()) {
				n.val.wtm() = n.val.lcm();
				n.val.refresh();
			}
			n.visitChildren(transformVisitor_);
			return true;
		}

		bool identifyVisitor(StrutumTree::NodeType &n)
		{
			n.val.getLCM().identify();
			return true;
		}
	}

	std::ostream &StrutumTree::print(std::ostream &os) const
	{
		PrintVisitor visitor(os);
		visit(visitor);
		return os;
	}

	StrutumTree &StrutumTree::setTransform()
	{
		for_each(beginTraverse(), endTraverse(), transformVisitor); // 動いてないじゃん
		//visit(transformVisitor_);
		return *this;
	}

	StrutumTree &StrutumTree::setIdentity()
	{
		for_each(beginTraverse(), endTraverse(), identifyVisitor);
		return *this;
	}

} // namespace gctp
