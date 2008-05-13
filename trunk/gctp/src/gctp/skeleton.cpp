/** @file
 * GameCatapult モデルの骨組み階層構造クラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/skeleton.hpp>
#include <list>
#include <vector>

using namespace std;

namespace gctp {

	GCTP_IMPLEMENT_CLASS_NS(gctp, Skeleton, StrutumTree);

	/** 既存のツリーを捨てて複製を保持する
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:21:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void Skeleton::copy(const Skeleton &src)
	{
		*this = Skeleton(StrutumTree(src.StrutumTree::dup()));
		index.clear();
		ConstTraverseItr sit = src.beginTraverse();
		for(TraverseItr dit = beginTraverse(); dit != endTraverse(); ++dit, ++sit) {
			const char *nodename = src.getName(*sit);
			if(nodename) index[nodename] = &*dit;
		}
	}

	/** 指定のスケルトンの複製を、子ノードに連結。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/12 5:19:18
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void Skeleton::merge(const Skeleton &src)
	{
		if(empty()) setUp();
		if(!empty() && !src.empty()) {
			StrutumTree::merge(src);
			ConstTraverseItr sit = src.beginTraverse();
			for(TraverseItr dit = back().beginTraverse(); dit != back().endTraverse(); ++dit, ++sit) {
				const char *nodename = src.getName(*sit);
				if(nodename) index[nodename] = &*dit;
			}
		}
	}

	/** 新規ノードを指定のノードの子として製作
	 *
	 * 挿入に失敗するとＮＵＬＬを返す。\n
	 * Nodeのaddメソッドで直接子を追加することも出来るが、その場合Hierarchyのgetメソッドで
	 * 名前による検索が出来なくなるので注意。
	 */
	Skeleton::NodeType *Skeleton::add(NodeType &parent, const char *name, const Matrix *_new)
	{
		if(_new) parent.push(Strutum(*_new));
		else parent.push();
		if(name && strlen(name)) index[name] = &parent.back();
		return &parent.back();
	}

	/** 新規ノードを指定のノードの子として製作
	 *
	 * 挿入に失敗するとＮＵＬＬを返す。\n
	 * Nodeのaddメソッドで直接子を追加することも出来るが、その場合Hierarchyのgetメソッドで
	 * 名前による検索が出来なくなるので注意。
	 */
	Skeleton::NodeType *Skeleton::add(NodeType &parent, const CStr name, const Matrix *_new)
	{
		if(_new) parent.push(Strutum(*_new));
		else parent.push();
		if(!name.empty()) index[name] = &parent.back();
		return &parent.back();
	}

	/** 名前が一致するノードに、srcの値を書き込む
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/09/26 9:01:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	Skeleton &Skeleton::sync(const Skeleton &src)
	{
		for(NodeIndex::const_iterator i = src.index.begin(); i != src.index.end(); ++i) {
			NodeType *node = get(i->first);
			if(node) node->val = reinterpret_cast<NodeType *>(i->second)->val;
		}
		return *this;
	}

} // namespace gctp
