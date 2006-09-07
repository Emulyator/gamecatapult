#ifndef _GCTP_PATHFINDER_HPP_
#define _GCTP_PATHFINDER_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file 経路探索テンプレートクラス
 *
 * Aアルゴリズムを行うクラスをテンプレート化
 *
 */

namespace gctp {

	/** Aアルゴリズムテンプレート
	 *
	 * search時に渡すClosureのestimateCost（予想コスト）が実際のコストと常に同じか低いとき、
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
			Node *root_;
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
		
		typedef gctp::PathSearcher<Node *, int> AStar;

		AStar astar(buffer, buffer_size);
		Closure closure;
		closure.root_ = node_root; // 必要な情報はあらかじめClosureに設定しておく
		AStar::NodeAndCost *path_term = astar.search<Node::SiblingIterator>(from_node, to_node, closure);
		// path_termは見つかったパスの終端。最初からたどるには、逆にたどる必要がある。
	 *@endcode
	 * @author sowwa ray
	 * @date 2005/11/30
	 * &copy; 2005 SowwaRay All Right Reserved.
	 */
	template<typename NodeType, typename CostType>
	class PathFinder {
	public:
		/** コンストラクタ
		 *
		 * アルゴリズムの動作に必要なバッファをあらかじめ割り当てておく。\n
		 * このクラスは途中で動的にバッファを割りあてることをしない。
		 *
		 * @param buf 作業用バッファ
		 * @param size 作業用バッファのサイズ
		 */
		PathFinder(void *buf, std::size_t size)
			: opened_(reinterpret_cast<NodeAndCost **>(buf), size/(sizeof(NodeAndCost*)+sizeof(NodeAndCost)))
			, closed_(reinterpret_cast<NodeAndCost *>(buf)+
				(size/(sizeof(NodeAndCost*)+sizeof(NodeAndCost))*sizeof(NodeAndCost *)+sizeof(NodeAndCost)-1)/sizeof(NodeAndCost)
				,size/sizeof(NodeAndCost)-(size/(sizeof(NodeAndCost*)+sizeof(NodeAndCost))*sizeof(NodeAndCost *)+sizeof(NodeAndCost)-1)/sizeof(NodeAndCost))
		{
		}

		/** 指定の最大パス長で動作可能な、PathSearcherへ渡すバッファ長を返す。
		 *
		 */
		static inline std::size_t calcBufferSize(int max_path)
		{
			return (sizeof(NodeAndCost)+sizeof(NodeAndCost*))*(max_path+1);
		}

		/** ノードと、そのコスト
		 *
		 * searchの結果は、このポインターチェーンで返される。\n
		 * 先頭はパスの終端を示している。（だから、そのコストは累計コストで、そのパスの
		 * 総コストになる)\n
		 * parentをたどっていくことで、開始点にたどり着く。開始点のNodeAndCostはparentがヌルである。
		 */
		struct NodeAndCost {
			NodeType node; ///< ノードを示す値
			CostType cost; ///< このノードまでの累計コスト
			CostType estimated_cost; ///< このノードの予想コスト（探索動作時に、予想コストを再計算する手間を省くためにここにキャッシュされる）
			NodeAndCost *parent; ///< 一つ前のNodeAndCostへのポインタ
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
						estimated_cost = child->estimated_cost;
						if(child->cost+estimated_cost <= cost+estimated_cost) continue;
					}
					else {
						child = &closed_.push();
						estimated_cost = c.estimateCost(*i, to);
					}
					opened_.push(child);
					child->parent = n;
					child->node = *i;
					child->cost = cost;
					child->estimated_cost = estimated_cost;
				}
				opened_.sort();
			}
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
				//if(length >= capacity) throw "capacity over.";
				if(length >= capacity) OS_Panic("capacity over. capacity = %d\n", capacity);
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
				//if(length >= capacity) throw "capacity over.";
				if(length >= capacity) OS_Panic("capacity over.");
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
#endif // _GCTP_PATHFINDER_HPP_
