#ifndef _GCTP_AALGO_HPP_
#define _GCTP_AALGO_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult Aアルゴリズムテンプレートクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 13:29:41
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */

namespace gctp {

	/** Aアルゴリズムテンプレート
	 *
	 * search時に渡すClosureのestimateCostが実際のコストと常に同じか低いとき、
	 * これは必ず最適解を返すA*アルゴリズムになる。
	 *@code
		struct Node {
			Node *next_sibling;
			Node *parent;
			Node *child;
			struct SiblingIterator {
				Node *node;
				SiblingIterator &operator++()
				{
					node = node->next_sibling;
					return *this;
				}
				Node &operator*()
				{
					return *node;
				}
				bool operator!=(const SiblingIterator &rhs) const
				{
					return node != rhs.node;
				}
			};
		};

		struct Closure {
			Node::SiblingIterator begin(Node *node) const
			{
				return (Node::SiblingIterator){node->child};
			}
			Node::SiblingIterator end(Node *node) const
			{
				return (Node::SiblingIterator){0};
			}
			int traverseCost(Node *from, Node *to) const
			{
				return 隣接ノード間の実コスト;
			}
			int estimateCost(Node *from, Node *to) const
			{
				return 指定ノード間の予想コスト;
			}
		};
		
		AAlgorithm<Node *, int> aalgo(buffer, buffer_size);
		Closure closure;
		aalgo.search<Node::SiblingIterator>::NodeAndCost *path_term =
			aalgo.search<Node::SiblingIterator>(from_node, to_node, closure);
		// path_termは見つかったパスの終端。最初からたどるには、逆にたどる必要がある。
	 *@endcode
	 * @author sowwa ray
	 * @date 2005/11/30
	 * &copy; 2005 SowwaRay All Right Reserved.
	 */
	template<typename NodeType, typename CostType>
	class AAlgorithm {
	public:
		AAlgorithm(void *buf, std::size_t len)
			: opened_(reinterpret_cast<NodeAndCost **>(buf), len/(sizeof(NodeAndCost*)+sizeof(NodeAndCost)))
			, closed_(reinterpret_cast<NodeAndCost *>(buf)+
				(len/(sizeof(NodeAndCost*)+sizeof(NodeAndCost))*sizeof(NodeAndCost *)+sizeof(NodeAndCost)-1)/sizeof(NodeAndCost)
				,len/sizeof(NodeAndCost)-(len/(sizeof(NodeAndCost*)+sizeof(NodeAndCost))*sizeof(NodeAndCost *)+sizeof(NodeAndCost)-1)/sizeof(NodeAndCost))
		{
		}

		struct NodeAndCost {
			NodeType node;
			CostType cost;
			CostType estimated_cost;
			NodeAndCost *parent;
		};

		/** パス探索
		 *
		 */
		template<typename IteratorType, typename Closure>
		const NodeAndCost *search(NodeType from, NodeType to, const Closure &c) const
		{
			opened_.clear();
			closed_.clear();
			NodeAndCost &start = closed_.push();
			start.node = from;
			start.cost = 0;
			start.parent = 0;
			opened_.push(&start);
			while(!opened_.isEmpty()) {
				NodeAndCost *n = opened_.pop();
				if(n->node == to) return n;
				for(IteratorType i = c.begin(n->node); i != c.end(n->node); ++i) {
					CostType cost = n->cost + c.traverseCost(n->node, *i);
					CostType estimated_cost = 0;
					NodeAndCost *child = closed_.find(*i);
					if(child) {
						//printf("already exist %d\n", (int)child->node);
						estimated_cost = child->estimated_cost;
						if(child->cost+estimated_cost <= cost+estimated_cost) continue;
						//printf("update\n");
					}
					else {
						//printf("new node\n");
						child = &closed_.push();
						estimated_cost = c.estimateCost(*i, to);
					}
					opened_.push(child);
					//printf("parent %d child %d\n", n->node, *i);
					child->parent = n;
					child->node = *i;
					child->cost = cost;
					child->estimated_cost = estimated_cost;
				}
				opened_.sort();
			}
			GCTP_TRACE("パスが見つからない");
			return 0;
		}
	
	private:
		struct NodeAndCostArray {
			NodeAndCost *buffer;
			std::size_t length;
			std::size_t capacity;
			NodeAndCostArray(NodeAndCost *buffer, std::size_t capacity)
				: buffer(buffer), length(0), capacity(capacity) {}
			NodeAndCost &push()
			{
				if(length >= capacity) throw "capacity over.";
				return buffer[length++];
			}
			void clear()
			{
				length = 0;
			}
			NodeAndCost *find(NodeType n)
			{
				for(int i = 0; i < length; i++) {
					if(buffer[i].node == n) return &buffer[i];
				}
				return 0;
			}
		};
		struct OpenList {
			NodeAndCost **buffer;
			std::size_t length;
			std::size_t capacity;
			OpenList(NodeAndCost **buffer, std::size_t capacity)
				: buffer(buffer), length(0), capacity(capacity) {}
			bool isEmpty() const
			{
				return length == 0;
			}
			void push(NodeAndCost *n)
			{
				if(contains(n)) return;
				if(length >= capacity) throw "capacity over.";
				buffer[length++] = n;
			}
			NodeAndCost *pop()
			{
				if(length > 0) return buffer[--length];
				return 0;
			}
			void clear()
			{
				length = 0;
			}
			bool contains(NodeAndCost *n)
			{
				for(int i = 0; i < length; i++) {
					if(buffer[i] == n) return true;
				}
				return false;
			}
			struct Compare : public std::binary_function<NodeAndCost *, NodeAndCost *, bool> {
				bool operator()(const NodeAndCost *lhs, const NodeAndCost *rhs) const {
					return lhs->cost > rhs->cost;
				}
			};
			void sort()
			{
				std::sort(&buffer[0], &buffer[length], Compare());
			}
		};
		
		mutable OpenList opened_;
		mutable NodeAndCostArray closed_;
	};

} // namespace gctp

#endif //_GCTP_AALGO_HPP_
