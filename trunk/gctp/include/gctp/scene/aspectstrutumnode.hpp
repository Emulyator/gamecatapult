#ifndef _GCTP_SCENE_STRUTUMNODE_HPP_
#define _GCTP_SCENE_STRUTUMNODE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ノード操作アスペクトテンプレートヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/strutumnode.hpp>
#include <gctp/scene/world.hpp>
#include <gctp/tuki.hpp>

namespace gctp { namespace scene {

	/** ノード操作のアスペクト
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 8:10:04
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class T>
	class AspectStrutumNode : public T
	{
	public:
		/// 新規にノードを持つ
		void newNode()
		{
			own_node_ = StrutumNode::create();
			node_ = own_node_;
		}

		/// 既存ノードにアタッチ
		void attach(Handle<StrutumNode> node)
		{
			if(own_node_) {
				own_node_->remove();
				own_node_ = 0;
			}
			node_ = node;
		}

		/// アタッチしているノード
		Handle<StrutumNode> node() const { return node_; }

		/// 独自ノードをワールドにぶら下げる
		void enter(World &world)
		{
			if(own_node_) world.strutum_tree.root()->push(own_node_);
		}

		/// 独自ノードをワールドからはずす
		void exit(World &world)
		{
			if(own_node_) own_node_->remove();
		}

	protected:
		void newNode(luapp::Stack &L)
		{
			newNode();
		}

		void attach(luapp::Stack &L)
		{
			if(L.top() >= 1) {
				attach(tuki_cast<StrutumNode>(L[1]));
			}
		}

		void enter(luapp::Stack &L)
		{
			if(L.top() >= 1) {
				Pointer<World> world = tuki_cast<World>(L[1]);
				if(world) enter(*world);
			}
		}

		void exit(luapp::Stack &L)
		{
			if(L.top() >= 1) {
				Pointer<World> world = tuki_cast<World>(L[1]);
				if(world) exit(*world);
			}
		}

		void setPosition(luapp::Stack &L)
		{
			if(L.top() >= 3) {
				if(node_) node_->val.getLCM().setPos(VectorC((float)L[1].toNumber(),(float)L[2].toNumber(),(float)L[3].toNumber()));
			}
		}

		int getPosition(luapp::Stack &L)
		{
			if(node_) {
				Vector v = node_->val.lcm().position();
				L << v.x << v.y << v.z;
				return 3;
			}
			return 0;
		}

		void setPosture(luapp::Stack &L)
		{
			if(node_) {
				Coord c = node_->val.lcm();
				if(L.top() == 1) {
					c.posture = QuatC((float)L[1].toNumber(), 0.0f, 0.0f);
				}
				else if(L.top() == 3) {
					c.posture = QuatC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber());
				}
				else if(L.top() >= 4) {
					c.posture = QuatC((float)L[1].toNumber(), (float)L[2].toNumber(), (float)L[3].toNumber(), (float)L[4].toNumber());
				}
				node_->val.getLCM() = c.toMatrix();
			}
		}

		int getPosture(luapp::Stack &L)
		{
			if(node_) {
				Coord c = node_->val.lcm();
				L << c.posture.w << c.posture.x << c.posture.y << c.posture.z;
				return 4;
			}
			return 0;
		}

		int getDirection(luapp::Stack &L)
		{
			Vector dir = node_->val.wtm().forward();
			L << dir.x << dir.y << dir.z;
			return 3;
		}

	private:
		Handle<StrutumNode> node_;
		Pointer<StrutumNode> own_node_;
	};

}} // namespace gctp::scene

#define GCTP_SCENE_ASPECTSTRUTUMNODE_TUKI_METHODS(_Class)	\
	TUKI_METHOD(_Class, newNode)		\
	TUKI_METHOD(_Class, attach)			\
	TUKI_METHOD(_Class, enter)			\
	TUKI_METHOD(_Class, exit)			\
	TUKI_METHOD(_Class, setPosition)	\
	TUKI_METHOD(_Class, getPosition)	\
	TUKI_METHOD(_Class, setPosture)		\
	TUKI_METHOD(_Class, getPosture)		\
	TUKI_METHOD(_Class, getDirection)

#endif //_GCTP_SCENE_STRUTUMNODE_HPP_