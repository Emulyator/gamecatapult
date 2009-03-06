/** @file
 * GameCatapult レンダーツリークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/rendertree.hpp>
#include <gctp/app.hpp>

using namespace std;

namespace gctp { namespace scene {

	/** 指定の階層ツリーの複製を、子ノードに連結。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/12 5:19:18
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void RenderTree::merge(const RenderTree &src)
	{
		root()->push(src.dup().root());
	}

	namespace {
		class RenderVisitor {
		public:
			bool need_clean;
			RenderVisitor(float delta) : need_clean(false), delta_(delta) {}
			bool operator()(const RenderTree::NodeType &n)
			{
				if(n.val) {
					if(n.val->onReach(delta_)) n.visitChildrenConst(*this);
					return n.val->onLeave(delta_);
				}
				else need_clean = true;
				return true;
			}
		private:
			float delta_;
		};
	}

	bool RenderTree::draw(float delta) const
	{
		RenderVisitor visitor(delta);
		visit(visitor);
		if(visitor.need_clean) const_cast<RenderTree *>(this)->clean();
		return true;
	}

	void RenderTree::clean()
	{
		for(TraverseItr i = beginTraverse(); i != endTraverse();)
		{
			if(i->val) ++i;
			else i = Tree< Handle<Renderer> >::erase(i);
		}
	}

	bool RenderTree::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	/*
	 * @code
camera = context:create("gctp.Camera")
shadow = context:create("xxxx.my.ShadowRenderer") -- （例）シャドウマップレンダラー
useshadow = context:create("xxxx.my.ShadowedRenderer") -- （例）シャドウマップ参照レンダラー
world = context:create("gctp.World")
rendertree = context:create("gctp.RenderTree")
rendertree:set({
	-- 名前→レンダラー→子の順
	
	-- 無名ノード（もっとも（一個なら）rootとしてアクセスできる）
	camera,
	{
		-- 名前付きノード
		"shadowmap",
		shadow,
		{
			world
		},
		-- 名前付きノード
		"mainnode",
		useshadow,
		{
			world
		}
	},
	-- ルートに複数あった場合、自動でダミールートが挿入され、その子になる。
	camera2,
	{
		....
	}
})
rendertree:replace("mainnode", { world }) -- 名前をつけとくと後で操作できる
rendertree:erase("shadowmap") -- 名前をつけとくと後で操作できる
	 * @endcode
	 */
	int RenderTree::set(luapp::Stack &L)
	{
		if(L.top() >= 1 && L[1].isTable()) {
			NodeType *parent = 0;
			int i = 1;
			do {
				if(i > 1 && !parent) {
					Pointer<NodeType> p = root();
					dummynode_ = new Renderer();
					setUp(dummynode_);
					root()->push(p);
					parent = root().get();
				}
				if(!set(L[1].toTable(), parent, i)) {
					L << "description error";
					return 1;
				}
			} while(!L[1].toTable()[i].isNil());
			return 0;
		}
		L << "invalid param";
		return 1;
	}

	bool RenderTree::set(const luapp::Table &tbl, NodePtr parent_node, int &i)
	{
		const char *name = 0;
		if(tbl[i].isString()) name = tbl[i++].toCStr();
		Handle<Renderer> renderer = tuki_cast<Renderer>(tbl[i]);
		if(renderer) {
			i++;
			NodePtr it;
			if(parent_node) {
				it = parent_node->push(renderer);
			}
			else {
				setUp(renderer);
				it = root();
			}
			if(it) {
				if(name && strlen(name)) index[name] = it.get();
				if(tbl[i].isTable()) {
					luapp::Table child = tbl[i++];
					int ii = 1;
					set(child, it, ii);
				}
			}
		}
		else return false;
		if(parent_node && !tbl[i].isNil()) return set(tbl, parent_node, i);
		return true;
	}

	namespace {
		class MakeTableVisitor {
		public:
			MakeTableVisitor(const RenderTree &tree, luapp::Stack &L) : tree_(tree), L_(L), count_(1) {}
			bool operator()(const RenderTree::NodeType &n)
			{
				if(n.val) {
					const char *name = tree_.getName(n);
					if(name && strlen(name)) {
						lua_pushstring(L_, name);
						lua_rawseti(L_, -2, count_++);
					}
					TukiRegister::push(L_, n.val);
					lua_rawseti(L_, -2, count_++);
					if(!n.empty()) {
						lua_newtable(L_);
						lua_rawseti(L_, -2, count_);
						lua_rawgeti(L_, -1, count_++);
						n.visitChildrenConst(MakeTableVisitor(tree_, L_));
						lua_pop(L_, 1);
					}
				}
				return true;
			}
		private:
			const RenderTree &tree_;
			luapp::Stack &L_;
			int count_;
		};
	}

	int RenderTree::get(luapp::Stack &L)
	{
		MakeTableVisitor visitor(*this, L);
		lua_newtable(L);
		visit(visitor);
		return 1;
	}

	void RenderTree::show(luapp::Stack &L)
	{
		app().draw_signal.connectOnce(draw_slot);
	}

	void RenderTree::hide(luapp::Stack &L)
	{
		app().draw_signal.disconnect(draw_slot);
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, RenderTree, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, RenderTree)
		TUKI_METHOD(RenderTree, set)
		TUKI_METHOD(RenderTree, get)
		TUKI_METHOD(RenderTree, show)
		TUKI_METHOD(RenderTree, hide)
	TUKI_IMPLEMENT_END(RenderTree)

}} // namespace gctp::scene
