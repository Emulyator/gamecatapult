#ifndef _GCTP_PATHFINDER_HPP_
#define _GCTP_PATHFINDER_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file �o�H�T���e���v���[�g�N���X
 *
 * A�A���S���Y�����s���N���X���e���v���[�g��
 *
 */

namespace gctp {

	/** A�A���S���Y���e���v���[�g
	 *
	 * search���ɓn��Closure��estimateCost�i�\�z�R�X�g�j�����ۂ̃R�X�g�Ə�ɓ������Ⴂ�Ƃ��A
	 * ����͕K���œK����Ԃ�A*�A���S���Y���ɂȂ�B
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
				return �אڃm�[�h�Ԃ̎��R�X�g;
			}
			int estimateCost(Node *from, Node *to) const
			{
				return �w��m�[�h�Ԃ̗\�z�R�X�g;
			}
		};
		
		typedef gctp::PathSearcher<Node *, int> AStar;

		AStar astar(buffer, buffer_size);
		Closure closure;
		closure.root_ = node_root; // �K�v�ȏ��͂��炩����Closure�ɐݒ肵�Ă���
		AStar::NodeAndCost *path_term = astar.search<Node::SiblingIterator>(from_node, to_node, closure);
		// path_term�͌��������p�X�̏I�[�B�ŏ����炽�ǂ�ɂ́A�t�ɂ��ǂ�K�v������B
	 *@endcode
	 * @author sowwa ray
	 * @date 2005/11/30
	 * &copy; 2005 SowwaRay All Right Reserved.
	 */
	template<typename NodeType, typename CostType>
	class PathFinder {
	public:
		/** �R���X�g���N�^
		 *
		 * �A���S���Y���̓���ɕK�v�ȃo�b�t�@�����炩���ߊ��蓖�ĂĂ����B\n
		 * ���̃N���X�͓r���œ��I�Ƀo�b�t�@�����肠�Ă邱�Ƃ����Ȃ��B
		 *
		 * @param buf ��Ɨp�o�b�t�@
		 * @param size ��Ɨp�o�b�t�@�̃T�C�Y
		 */
		PathFinder(void *buf, std::size_t size)
			: opened_(reinterpret_cast<NodeAndCost **>(buf), size/(sizeof(NodeAndCost*)+sizeof(NodeAndCost)))
			, closed_(reinterpret_cast<NodeAndCost *>(buf)+
				(size/(sizeof(NodeAndCost*)+sizeof(NodeAndCost))*sizeof(NodeAndCost *)+sizeof(NodeAndCost)-1)/sizeof(NodeAndCost)
				,size/sizeof(NodeAndCost)-(size/(sizeof(NodeAndCost*)+sizeof(NodeAndCost))*sizeof(NodeAndCost *)+sizeof(NodeAndCost)-1)/sizeof(NodeAndCost))
		{
		}

		/** �w��̍ő�p�X���œ���\�ȁAPathSearcher�֓n���o�b�t�@����Ԃ��B
		 *
		 */
		static inline std::size_t calcBufferSize(int max_path)
		{
			return (sizeof(NodeAndCost)+sizeof(NodeAndCost*))*(max_path+1);
		}

		/** �m�[�h�ƁA���̃R�X�g
		 *
		 * search�̌��ʂ́A���̃|�C���^�[�`�F�[���ŕԂ����B\n
		 * �擪�̓p�X�̏I�[�������Ă���B�i������A���̃R�X�g�͗݌v�R�X�g�ŁA���̃p�X��
		 * ���R�X�g�ɂȂ�)\n
		 * parent�����ǂ��Ă������ƂŁA�J�n�_�ɂ��ǂ蒅���B�J�n�_��NodeAndCost��parent���k���ł���B
		 */
		struct NodeAndCost {
			NodeType node; ///< �m�[�h�������l
			CostType cost; ///< ���̃m�[�h�܂ł̗݌v�R�X�g
			CostType estimated_cost; ///< ���̃m�[�h�̗\�z�R�X�g�i�T�����쎞�ɁA�\�z�R�X�g���Čv�Z�����Ԃ��Ȃ����߂ɂ����ɃL���b�V�������j
			NodeAndCost *parent; ///< ��O��NodeAndCost�ւ̃|�C���^
		};

		/** �p�X�T��
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
