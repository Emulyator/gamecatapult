#ifndef _GCTP_TREE_HPP_
#define _GCTP_TREE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult �c���[�e���v���[�g�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 19:28:21
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointer.hpp>
#include <boost/iterator_adaptors.hpp>
#include <boost/function.hpp>

namespace gctp {

	/** �m�[�h�^�̃x�[�X
	 *
	 * ���ʕ������e���v���[�g��p��������
	 */
	class TreeNodeBase : public Object {
	public:
		/// �R���X�g���N�^
		TreeNodeBase() : parent_(0), older_(0) {}

		/// �q���X�g�̍ŏ����w���|�C���^��Ԃ�
		TreeNodeBase *begin() { return children_.get(); }
		/// �q���X�g�̏I�[���w���|�C���^��Ԃ�
		TreeNodeBase *end() { return 0; }

		/// �q���X�g�̍ŏ����w���|�C���^��Ԃ�
		const TreeNodeBase *begin() const { return children_.get(); }
		/// �q���X�g�̏I�[���w���|�C���^��Ԃ�
		const TreeNodeBase *end() const { return 0; }

		/// pos�̒��O�Ɏq��}��
		void insert(TreeNodeBase *pos, Pointer<TreeNodeBase> node)
		{
			/// �f�o�b�O�łł͖{�Ƃ͓n���ꂽ�C�e���[�^���{���Ɏ����̎q�����ׂ��ق����ǂ���
			if(pos) {
				if(!pos->_isFront()) pos->older_->_insert(node);
				else {
					node->remove();
					node->parent_ = pos->parent_;
					node->older_ = pos->older_;
					node->younger_ = &(*pos);
				}
			}
			else push(node);
		}
		/// pos���폜���A�����Itr��Ԃ�
		TreeNodeBase *erase(TreeNodeBase *pos)
		{
			if(pos != end()) {
				TreeNodeBase *ret = pos->younger_.get();
				pos->remove();
				return ret;
			}
			return end();
		}

		/// �������͂���
		void remove()
		{
			Pointer<TreeNodeBase> lock = this;
			// �Z��֌W��؂�
			if(!_isFront() && older_) older_->younger_ = younger_;
			if(younger_) younger_->older_ = older_;
			// �e�q�֌W��؂�
			if(parent_ && parent_->children_ == this) parent_->children_ = 0;
			younger_ = older_ = parent_ = 0;
		}
		/// �q�ɒǉ�
		void push(Pointer<TreeNodeBase> node)
		{
			if(children_) {
				if(children_->older_) { // �擪��older_��back���w��
					children_->older_->_insert(node);
					children_->older_ = node.get();
				}
				else children_->_back()._insert(node);
			}
			else {
				node->remove();
				children_ = node;
				node->parent_ = this;
				node->older_ = node.get();
			}
		}
		/// �q�����邩�H
		bool empty() const { return !children_; }
		/// �q�̐擪
		TreeNodeBase &front() { return *children_; }
		/// �q�̍Ō��
		TreeNodeBase &back() { return *children_->older_; }
		/// �q�̐擪
		const TreeNodeBase &front() const { return *children_; }
		/// �q�̍Ō��
		const TreeNodeBase &back() const { return *children_->older_; }

	GCTP_DEFINE_TYPEINFO(TreeNodeBase, Object)

	protected:
		TreeNodeBase *parent_;
		Pointer<TreeNodeBase> children_;
		TreeNodeBase *older_;
		Pointer<TreeNodeBase> younger_;

		static TreeNodeBase *_next_DF2(TreeNodeBase *root, TreeNodeBase *cur)
		{
			if(cur->younger_) return cur->younger_.get();
			else if(cur->parent_ && cur->parent_ != root) return _next_DF2(root, cur->parent_);
			return 0;
		}
		static TreeNodeBase *_next_DF(TreeNodeBase *root, TreeNodeBase *cur)
		{
			if(cur->children_) return cur->children_.get();
			else return _next_DF2(root, cur);
		}
		static TreeNodeBase *_prev_DF(TreeNodeBase *root, TreeNodeBase *cur)
		{
			if(cur->older_) return cur->older_;
			else if(cur->parent_ && cur->parent_ != root) return _prev_DF(root, cur->parent_);
			return 0;
		}
		static const TreeNodeBase *_next_DF2(const TreeNodeBase *root, const TreeNodeBase *cur)
		{
			if(cur->younger_) return cur->younger_.get();
			else if(cur->parent_ && cur->parent_ != root) return _next_DF2(root, cur->parent_);
			return 0;
		}
		static const TreeNodeBase *_next_DF(const TreeNodeBase *root, const TreeNodeBase *cur)
		{
			if(cur->children_) return cur->children_.get();
			else return _next_DF2(root, cur);
		}
		static const TreeNodeBase *_prev_DF(const TreeNodeBase *root, const TreeNodeBase *cur)
		{
			if(cur->older_) return cur->older_;
			else if(cur->parent_ && cur->parent_ != root) return _prev_DF(root, cur->parent_);
			return 0;
		}
		static TreeNodeBase *_next_WF(TreeNodeBase *root, TreeNodeBase *cur)
		{
			if(cur->younger_) return cur->younger_.get();
			else if(cur->younger_) return _next_WF(root, cur->younger_.get());
			else if(cur->parent_ && cur->parent_ != root) return _next_WF(root, cur->parent_);
			return 0;
		}
		static TreeNodeBase *_prev_WF(TreeNodeBase *root, TreeNodeBase *cur)
		{
			if(!cur->_isFront() && cur->older_->children_) return cur->older_->children_.get();
			else if(!cur->_isFront()) return _prev_WF(root, cur->older_);
			else if(cur->parent_ && cur->parent_ != root) return _prev_WF(root, cur->parent_);
			return 0;
		}
		static const TreeNodeBase *_next_WF(const TreeNodeBase *root, const TreeNodeBase *cur)
		{
			if(cur->younger_ && cur->younger_->children_) return cur->younger_->children_.get();
			else if(cur->younger_) return _next_WF(root, cur->younger_.get());
			else if(cur->parent_ && cur->parent_ != root) return _next_WF(root, cur->parent_);
			return 0;
		}
		static const TreeNodeBase *_prev_WF(const TreeNodeBase *root, const TreeNodeBase *cur)
		{
			if(!cur->_isFront() && cur->older_->children_) return cur->older_->children_.get();
			else if(!cur->_isFront()) return _prev_WF(root, cur->older_);
			else if(cur->parent_ && cur->parent_ != root) return _prev_WF(root, cur->parent_);
			return 0;
		}
		// ��[���H
		bool _isFront() const
		{
			return ((parent_ && parent_->children_==this) || older_ == 0);
		}
		// �Z�탊�X�g�̑O�[
		TreeNodeBase &_front()
		{
			if(_isFront()) return older_->_front();
			else return *this;
		}
		// �Z�탊�X�g�̌�[
		TreeNodeBase &_back()
		{
			if(younger_) return younger_->_back();
			else return *this;
		}
		// �����̒���ɑ}��
		void _insert(Pointer<TreeNodeBase> node)
		{
			node->remove();
			node->parent_ = parent_;
			node->older_ = this;
			node->younger_ = younger_;
			younger_ = node;
		}
#ifdef GCTP_DONT_USE_BOOST_ITERATOR_ADAPTOR
		friend struct TreeIteratorBase;
		friend struct TreeChildIteratorBase;
		friend struct TreeDFIteratorBase;
		friend struct TreeWFIteratorBase;
#endif
	};

#ifdef GCTP_DONT_USE_BOOST_ITERATOR_ADAPTOR
	struct TreeIteratorBase {
		typedef size_t                     size_type;
		typedef ptrdiff_t                  difference_type;
		typedef bidirectional_iterator_tag iterator_category;

		const TreeNodeBase *node_;

		TreeIteratorBase(const TreeNodeBase *node) : node_(node) {}
		TreeIteratorBase() {}

		bool operator==(const TreeIteratorBase &rhs) const { 
			return node_ == rhs.node_;
		}
		bool operator!=(const TreeIteratorBase &rhs) const { 
			return node_ != rhs.node_; 
		}
	};
	struct TreeSiblingIteratorBase : public TreeIteratorBase {
		TreeSiblingIteratorBase(const TreeNodeBase *node) : TreeIteratorBase(node) {}
		TreeSiblingIteratorBase() {}
		void increment() { node_ = node_->younger_.get(); }
		void decrement() { node_ = node_->older_; }
	};
	struct TreeDFIteratorBase : public TreeIteratorBase {
		const TreeNodeBase *root_;
		TreeChildIteratorBase(const TreeNodeBase *root, const TreeNodeBase *node) : TreeIteratorBase(node), root_(root) {}
		TreeChildIteratorBase() {}
		void increment()
		{ 
			if(node_->children_) node_ = &node_->front();
			else node_ = TreeNodeBase::_next_DF(root_, node_);
		}
		void decrement()
		{
			if(node_->children_) node_ = &node_->back();
			else node_ = TreeNodeBase::_prev_DF(root_, node_);
		}
	};
	struct TreeWFIteratorBase : public TreeIteratorBase {
		const TreeNodeBase *root_;
		TreeChildIteratorBase(const TreeNodeBase *root, const TreeNodeBase *node) : TreeIteratorBase(node), root_(root) {}
		TreeChildIteratorBase() {}
		void increment()
		{
			if(node_->younger_) node_ = node_->younger_.get();
			else if(node_->parent_) node_ = TreeNodeBase::_next_WF(root_, node_->parent_);
			else node_ = 0;
		}
		void decrement()
		{
			if(!node_->_isFront()) node_ = node_->older_;
			else if(node_->parent_) node_ = TreeNodeBase::_prev_WF(root_, node_->parent_);
			else node_ = 0;
		}
	};
	template<typename _NodeType, typename _BaseType>
	struct ConstTreeNodeIterator : public _BaseType {
		typedef typename const _NodeType	&reference;
		typedef bidirectional_iterator_tag iterator_category;
		TreeNodeIterator(const _NodeType *p) : _BaseType(p) {}
		TreeNodeIterator() {}

		reference operator*() const { return *static_cast<const _NodeType *>(node_); }

		ConstTreeNodeIterator &operator++() { 
			this->increment();
			return *this;
		}
		ConstTreeNodeIterator operator++(int) { 
			ConstTreeNodeIterator __tmp = *this;
			this->increment();
			return __tmp;
		}
		ConstTreeNodeIterator &operator--() { 
			this->decrement();
			return *this;
		}
		ConstTreeNodeIterator operator--(int) { 
			ConstTreeNodeIterator __tmp = *this;
			this->decrement();
			return __tmp;
		}
	};
	template<typename _NodeType, typename _BaseType>
	struct TreeNodeIterator : public _BaseType {
		typedef typename const _NodeType	&reference;
		typedef bidirectional_iterator_tag iterator_category;
		TreeNodeIterator(_NodeType *p) : _BaseType(p) {}
		TreeNodeIterator() {}

		reference operator*() const { return *static_cast<_NodeType *>(const_cast<TreeNodeBase *>(node_)); }

		TreeNodeIterator &operator++() { 
			this->increment();
			return *this;
		}
		TreeNodeIterator operator++(int) { 
			ConstTreeNodeIterator __tmp = *this;
			this->increment();
			return __tmp;
		}
		TreeNodeIterator &operator--() { 
			this->decrement();
			return *this;
		}
		TreeNodeIterator operator--(int) { 
			ConstTreeNodeIterator __tmp = *this;
			this->decrement();
			return __tmp;
		}
	};
#endif

	/** �m�[�h�̌^�e���v���[�g
	 *
	 * Object�h����Pointer�ŕߑ�����
	 */
	template<typename T, class Alloc = std::allocator<T> >
	class TreeNode : public TreeNodeBase {
	public:
		typedef T					ValueType;
		typedef ValueType&			Reference;
		typedef const ValueType&	ConstReference;
#if defined(_STLPORT_VERSION) && !defined(_STLP_USE_NESTED_TCLASS_THROUGHT_TPARAM)
		typedef typename std::_Alloc_traits<TreeNode, Alloc>::allocator_type AllocatorType;
#else
		//typedef typename Alloc::template rebind<TreeNode>::other AllocatorType;// ���ł���ȏ��������H
		typedef typename Alloc::rebind<TreeNode>::other AllocatorType;
#endif
		static AllocatorType allocator() { AllocatorType alloc; return alloc; }

		static void *operator new( size_t size, AllocatorType& allocator ) {
			return allocator.allocate( 1 );
		}
		static void operator delete( void *p, AllocatorType& allocator ) {
			allocator.deallocate( reinterpret_cast<AllocatorType::pointer>(p), 1 );
		}
		static void operator delete( void *p ) { allocator().deallocate( reinterpret_cast<AllocatorType::pointer>(p), 1 ); }

		/// �t�@�N�g���[���\�b�h�inew(TreeNode<T>::allocator()) TreeNode<T>()�Ƃ�����y�j
		static TreeNode *create()
		{
			return new(allocator()) TreeNode;
		}
		/// �t�@�N�g���[���\�b�h�inew(TreeNode<T>::allocator()) TreeNode<T>(src)�Ƃ�����y�j
		static TreeNode *create(const T &src)
		{
			return new(allocator()) TreeNode(src);
		}

#ifndef BOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX
		/// �g���o�[�X�֐��̃r�W�^�[�^ (�[���D��T��������ꍇ�Aif(!node->visitChildren(visitor)) return false;��visitor�̒��ōs���ċA�T������悤�ɂ���)
		typedef boost::function<bool (TreeNode &)> Visitor;
		/// �g���o�[�X�֐��̃r�W�^�[�^�iconst�Łj  (�[���D��T��������ꍇ�Aif(!node->visitChildren(visitor)) return false��visitor�̒��ōs���ċA�T������悤�ɂ���)
		typedef boost::function<bool (const TreeNode &)> ConstVisitor;
#else
		/// �g���o�[�X�֐��̃r�W�^�[�^ (�[���D��T��������ꍇ�Aif(!node->visitChildren(visitor)) return false;��visitor�̒��ōs���ċA�T������悤�ɂ���)
		typedef boost::function1<bool, TreeNode &> Visitor;
		/// �g���o�[�X�֐��̃r�W�^�[�^�iconst�Łj  (�[���D��T��������ꍇ�Aif(!node->visitChildren(visitor)) return false��visitor�̒��ōs���ċA�T������悤�ɂ���)
		typedef boost::function1<bool, const TreeNode &> ConstVisitor;
#endif
#ifdef GCTP_DONT_USE_BOOST_ITERATOR_ADAPTOR
		/// �Z��C�e���[�^
		typedef TreeNodeIterator<TreeNode, TreeSiblingIteratorBase> Itr;
		/// �Z��C�e���[�^
		typedef ConstTreeNodeIterator<TreeNode, TreeSiblingIteratorBase> ConstItr;
		/// �[���D��T���C�e���[�^
		typedef TreeNodeIterator<TreeNode, TreeDFIteratorBase> DFItr;
		/// �[���D��T���C�e���[�^
		typedef ConstTreeNodeIterator<TreeNode, TreeDFIteratorBase> ConstDFItr;
		/// ���D��T���C�e���[�^
		typedef TreeNodeIterator<TreeNode, TreeWFIteratorBase> WFItr;
		/// ���D��T���C�e���[�^
		typedef ConstTreeNodeIterator<TreeNode, TreeWFIteratorBase> ConstWFItr;
#else
		/// �Z��C�e���[�^
		struct Itr : boost::iterator_adaptor<Itr, TreeNode*, boost::use_default, boost::bidirectional_traversal_tag>
		{
			Itr( TreeNode* p ) : boost::iterator_adaptor<Itr, TreeNode*, boost::use_default, boost::bidirectional_traversal_tag>( p ) {}
			void increment() { base_reference() = reinterpret_cast<TreeNode*>(base()->younger_.get()); }
			void decrement() { if(base()->_isFront()) base_reference() = 0; else base_reference() = reinterpret_cast<TreeNode*>(base()->older_); }
		};

		/// �Z��C�e���[�^
		struct ConstItr : boost::iterator_adaptor<ConstItr, const TreeNode*, boost::use_default, boost::bidirectional_traversal_tag>
		{
			ConstItr( const TreeNode* p ) : boost::iterator_adaptor<ConstItr, const TreeNode*, boost::use_default, boost::bidirectional_traversal_tag>( p ) {}
			void increment() { base_reference() = reinterpret_cast<const TreeNode *>(base()->younger_.get()); }
			void decrement() { if(base()->_isFront()) base_reference() = 0; else base_reference() = reinterpret_cast<const TreeNode *>(base()->older_); }
		};

		/// �[���D��T���C�e���[�^
		struct DFItr : boost::iterator_adaptor<DFItr, TreeNode *, boost::use_default, boost::forward_traversal_tag>
		{
			TreeNode *root_;
			DFItr(TreeNode *root, TreeNode *p) : boost::iterator_adaptor<DFItr, TreeNode *, boost::use_default, boost::forward_traversal_tag>( p ), root_(root) {}
			void increment()
			{
				base_reference() = reinterpret_cast<TreeNode *>(_next_DF(root_, base()));
			}
		};

		/// �[���D��T���C�e���[�^
		struct ConstDFItr : boost::iterator_adaptor<ConstDFItr, const TreeNode *, boost::use_default, boost::forward_traversal_tag>
		{
			const TreeNode *root_;
			ConstDFItr(const TreeNode *root, const TreeNode *p) : boost::iterator_adaptor<ConstDFItr, const TreeNode *, boost::use_default, boost::forward_traversal_tag>( p ), root_(root) {}
			void increment()
			{
				base_reference() = reinterpret_cast<const TreeNode *>(_next_DF(root_, base()));
			}
		};

		/// ���D��T���C�e���[�^
		struct WFItr : boost::iterator_adaptor<WFItr, TreeNode*, boost::use_default, boost::forward_traversal_tag>
		{
			TreeNode *root_;
			WFItr( TreeNode* root, TreeNode* p ) : boost::iterator_adaptor<WFItr, TreeNode*, boost::use_default, boost::forward_traversal_tag>( p ), root_(root) {}
			TreeNode *_next(TreeNodeBase *cur)
			{
				return reinterpret_cast<TreeNode *>(_next_WF(root_, cur));
			}
			void increment()
			{
				if(base()->younger_) base_reference() = reinterpret_cast<TreeNode*>(base()->younger_.get());
				else if(base()->parent_) base_reference() = _next(base()->parent_);
				else base_reference() = 0;
			}
		};

		/// ���D��T���C�e���[�^
		struct ConstWFItr : boost::iterator_adaptor<ConstWFItr, const TreeNode*, boost::use_default, boost::forward_traversal_tag>
		{
			const TreeNode *root_;
			ConstWFItr( const TreeNode* root, const TreeNode* p ) : boost::iterator_adaptor<ConstWFItr, const TreeNode*, boost::use_default, boost::forward_traversal_tag>( p ), root_(root) {}
			const TreeNode *_next(const TreeNodeBase *cur)
			{
				return reinterpret_cast<const TreeNode *>(_next_WF(root_, cur));
			}
			void increment()
			{
				if(base()->younger_) base_reference() = reinterpret_cast<const TreeNode*>(base()->younger_.get());
				else if(base()->parent_) base_reference() = _next(base()->parent_);
				else base_reference() = 0;
			}
		};
#endif
		/// �q���X�g�̍ŏ����w���C�e���[�^��Ԃ�
		Itr begin() { return Itr(reinterpret_cast<TreeNode *>(TreeNodeBase::begin())); }
		/// �q���X�g�̏I�[���w���C�e���[�^��Ԃ�
		Itr end() { return Itr(reinterpret_cast<TreeNode *>(TreeNodeBase::end())); }
		/// �q���X�g�̍ŏ����w���C�e���[�^��Ԃ�
		ConstItr begin() const { return ConstItr(reinterpret_cast<const TreeNode *>(TreeNodeBase::begin())); }
		/// �q���X�g�̏I�[���w���C�e���[�^��Ԃ�
		ConstItr end() const { return ConstItr(reinterpret_cast<const TreeNode *>(TreeNodeBase::end())); }

		/// �[���D��T���̍ŏ����w���C�e���[�^��Ԃ�
		DFItr beginDF() { return DFItr(this, this); }
		/// �[���D��T���̏I�[���w���C�e���[�^��Ԃ�
		DFItr endDF() { return DFItr(this, 0); }
		/// �[���D��T���̍ŏ����w���C�e���[�^��Ԃ�
		ConstDFItr beginDF() const { return ConstDFItr(this, this); }
		/// �[���D��T���̏I�[���w���C�e���[�^��Ԃ�
		ConstDFItr endDF() const { return ConstDFItr(this, 0); }

		/// �[���D��T���̍ŏ����w���C�e���[�^��Ԃ�
		WFItr beginWF() { return WFItr(this, this); }
		/// �[���D��T���̏I�[���w���C�e���[�^��Ԃ�
		WFItr endWF() { return WFItr(this, 0); }
		/// �[���D��T���̍ŏ����w���C�e���[�^��Ԃ�
		ConstWFItr beginWF() const { return ConstWFItr(this, this); }
		/// �[���D��T���̏I�[���w���C�e���[�^��Ԃ�
		ConstWFItr endWF() const { return ConstWFItr(this, 0); }

		/// �i�[���Ă���l
		T val;

		/// �R���X�g���N�^
		TreeNode() : TreeNodeBase() {}
		/// �R���X�g���N�^
		TreeNode(const T &_val) : TreeNodeBase(), val(_val) {}

		/// �i�[���Ă���l��Ԃ�
		Reference operator*() {
			return val;
		}
		/// �i�[���Ă���l��Ԃ�
		ConstReference operator*() const {
			return val;
		}
		
		/// pos�̒��O�Ɏq��}��
		void insert(Itr pos, Pointer<TreeNode> node)
		{
			/// �f�o�b�O�łł͖{�Ƃ͓n���ꂽ�C�e���[�^���{���Ɏ����̎q�����ׂ��ق����ǂ���
			TreeNodeBase::insert(&*pos, node);
		}
		/// pos�̒��O�Ɏq��}��
		Itr insert(Itr pos, const ValueType &val)
		{
			TreeNode *ret = new(allocator()) TreeNode(val);
			insert(pos, ret);
			return Itr(ret);
		}
		/// pos�̒��O�Ɏq��}��
		Itr insert(Itr pos)
		{
			TreeNode *ret = new(allocator()) TreeNode();
			insert(pos, ret);
			return Itr(ret);
		}
		/// pos���폜���A�����Itr��Ԃ�
		Itr erase(Itr pos)
		{
			return Itr(reinterpret_cast<TreeNode *>(TreeNodeBase::erase(&*pos)));
		}
		/// �q�ɒǉ�
		Itr push(Pointer<TreeNode> node)
		{
			TreeNodeBase::push(node);
			return Itr(node.get());
		}
		/// �q�ɒǉ�
		Itr push(const ValueType &val)
		{
			TreeNode *ret = new(allocator()) TreeNode(val);
			push(ret);
			return Itr(ret);
		}
		/// �q�ɒǉ�
		Itr push()
		{
			TreeNode *ret = new(allocator()) TreeNode();
			push(ret);
			return Itr(ret);
		}

		struct FindValuePred {
			const T &rhs;
			FindValuePred(const T &rhs) : rhs(rhs) {}
			bool operator()(const TreeNode &lhs)
			{
				return lhs.val == rhs;
			}
		};

		/// ����
		Itr find(const T &val)
		{
			return std::find_if(begin(), end(), FindValuePred(val));
		}
		/// ����
		ConstItr find(const T &val) const
		{
			return std::find_if(begin(), end(), FindValuePred(val));
		}

		/// �K�w�𕡐�
		Pointer<TreeNode> dup() const
		{
			Pointer<TreeNode> ret = new(allocator()) TreeNode(val);
			for(TreeNode::ConstItr it = begin(); it != end(); ++it) {
				ret->push(it->dup());
			}
			return ret;
		}

		template<class E, class T>
		void printIndented(std::basic_ostream<E, T> & os, int indent) const
		{
			for(int i = 0; i < indent; i++) os << "\t";
			os << val << std::endl;
			for(TreeNode::ConstItr it = begin(); it != end(); ++it) {
				it->printIndented(os, indent+1);
			}
		}

		/** �q�m�[�h�S���Ƀr�W�^�[�𑗂�
		 *
		 * �[���D��T���̏ꍇ�Avisitor��node->visitChidren(visitor)�ƍċA�Ăяo�����Ă��K�v������\n
		 * �e�m�[�h���ƂɃr�W�^�[���Ăяo��
		 * @return ���f���ꂸ�ɂ��ׂĎ��s�������H
		 * @param Visitor �e�m�[�h���Ƃ̏���  ���f����ꍇ��false��Ԃ�
		 */
		bool visitChildren(Visitor visitor)
		{
			for(Itr it = begin(); it != end(); ++it) {
				if(!visitor(*it)) return false;
			}
			return true;
		}
		/** �q�m�[�h�S���Ƀr�W�^�[�𑗂�(const��)
		 *
		 * �[���D��T���̏ꍇ�Avisitor��node->visitChidren(visitor)�ƍċA�Ăяo�����Ă��K�v������\n
		 * �e�m�[�h���ƂɃr�W�^�[���Ăяo��
		 * @return ���f���ꂸ�ɂ��ׂĎ��s�������H
		 * @param Visitor �e�m�[�h���Ƃ̏���  ���f����ꍇ��false��Ԃ�
		 * @todo �ق�Ƃɖ��O�ς��Ȃ���_���H(Pointer::get����const�|�C���^��Ԃ��̂��ǂ��Ȃ��A���ۂ�)
		 */
		bool visitChildrenConst(ConstVisitor visitor) const
		{
			for(ConstItr it = begin(); it != end(); ++it) {
				if(!visitor(*it)) return false;
			}
			return true;
		}

		/// �e�̃|�C���^
		TreeNode *parent() { return static_cast<TreeNode *>(parent_); }
		/// �q�̐擪
		TreeNode &front() { return static_cast<TreeNode &>(TreeNodeBase::front()); }
		/// �q�̍Ō��
		TreeNode &back() { return static_cast<TreeNode &>(TreeNodeBase::back()); }
		/// �q�̐擪
		const TreeNode &front() const { return static_cast<const TreeNode &>(TreeNodeBase::front()); }
		/// �q�̍Ō��
		const TreeNode &back() const { return static_cast<const TreeNode &>(TreeNodeBase::back()); }
		
	GCTP_DEFINE_TYPEINFO(TreeNode, TreeNodeBase)
	};

	template<class E, class T, typename TV>
	std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, TreeNode<TV> const & node)
	{
		os << *node;
		for(TreeNode<TV>::ConstItr it = node.begin(); it != node.end(); ++it) {
			os << *it;
		}
		return os;
	}

	/** �c���[�N���X�e���v���[�g�N���X
	 *
	 * �B��̃��[�g������A�H�������Ȃ��i�z�Q�ƂȂ��A�����̐e�ɑ����邱�Ƃ��Ȃ��j�P�����O���t�B
	 *
	 * �R�c�O���t�B�b�N�ł̊K�w�c���[��\�����Ƃ�ړI�ɂ��Ă��邽�߁A�g�E�ݏグ�h�i���܂łƂ͈Ⴄ�m�[�h�����[�g�Ɏw�肵�A
	 * �w�d�͂Ɏ䂩�ꂽ�悤�Ɂx�e�q�֌W�C������j�͑S���O���ɂ����Ȃ��B�P���ȃc���[�B
	 *
	 * �i�[����l�́A�f�t�H���g�R���X�g���N�^�ƃR�s�[�R���X�g���N�^�����K�v������B
	 *
	 * ���[�g�m�[�h�ւ̃X�}�[�g�|�C���^�Ƃ��ĐU����
	 * @code
	    Tree< TreeNode<Foo> > tree;
	   @endcode
	 * @note allocator�w��̓m�[�h�̊m�ۂ̂��߂����Ɏg�p����ATree���g�̊m�ۂɂ͊֗^���Ȃ��B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/27 2:51:14
	 */
	template<class T, class Alloc = std::allocator<T> >
	class Tree : public Pointer< TreeNode<T, Alloc> > {
	public:
		typedef TreeNode<T, Alloc>				NodeType;
		typedef Pointer<NodeType>				NodePtr;
		typedef typename NodeType::ValueType	ValueType;
		typedef typename NodeType::Itr			Itr;
		typedef typename NodeType::ConstItr		ConstItr;
		typedef typename NodeType::DFItr		DFItr;
		typedef typename NodeType::ConstDFItr	ConstDFItr;
		typedef typename NodeType::WFItr		WFItr;
		typedef typename NodeType::ConstWFItr	ConstWFItr;
		typedef typename NodeType::Visitor		Visitor;
		typedef typename NodeType::ConstVisitor	ConstVisitor;

		/// �c���[�m�[�h�n���h��
		struct NodeHandle
		{
			T &operator*() { return node->value; }
			const T &operator*() const { return node->value; }
			T *operator->() { return &node->value; }
			const T *operator->() const { return &node->value; }
			operator T *()
			{
				return node.get();
			}
			operator const T *() const
			{
				return node.get();
			}
			Handle< NodeType > node;
		};
		/// �c���[�m�[�h�|�C���^�[
		struct NodePointer
		{
			T &operator*() { return node->value; }
			const T &operator*() const { return node->value; }
			T *operator->() { return &node->value; }
			const T *operator->() const { return &node->value; }
			operator T *()
			{
				return node.get();
			}
			operator const T *() const
			{
				return node.get();
			}
			NodePtr node;
		};

		/// �f�t�H���g�R���X�g���N�^
		Tree() {}
		/// �R���X�g���N�^
		Tree(const ValueType &val) : NodePtr(NodeType::create(val)) {}
		/// �R���X�g���N�^
		Tree(NodePtr const & rhs): NodePtr(rhs) {}

		/** visitor�Ăяo��
		 *
		 * ���[�g�m�[�h��K��
		 */
		void visit(Visitor visitor)
		{
			if(get()) visitor(*get());
		}
		/** visitor�Ăяo��(const��)
		 *
		 * ���[�g�m�[�h��K��
		 */
		void visit(ConstVisitor visitor) const
		{
			if(get()) visitor(*get());
		}

		/// �q���X�g�̍ŏ����w���C�e���[�^��Ԃ�
		Itr begin() { return Itr(get()); }
		/// �q���X�g�̏I�[���w���C�e���[�^��Ԃ�
		Itr end() { return Itr(0); }
		/// �q���X�g�̍ŏ����w���C�e���[�^��Ԃ�
		ConstItr begin() const { return ConstItr(get()->children_.get()); }
		/// �q���X�g�̏I�[���w���C�e���[�^��Ԃ�
		ConstItr end() const { return ConstItr(0); }

		/// �[���D��T���̍ŏ����w���C�e���[�^��Ԃ�
		DFItr beginDF() { return DFItr(get(), get()); }
		/// �[���D��T���̏I�[���w���C�e���[�^��Ԃ�
		DFItr endDF() { return DFItr(get(), 0); }
		/// �[���D��T���̍ŏ����w���C�e���[�^��Ԃ�
		ConstDFItr beginDF() const { return ConstDFItr(get(), get()); }
		/// �[���D��T���̏I�[���w���C�e���[�^��Ԃ�
		ConstDFItr endDF() const { return ConstDFItr(get(), 0); }

		/// �[���D��T���̍ŏ����w���C�e���[�^��Ԃ�
		WFItr beginWF() { return WFItr(get(), get()); }
		/// �[���D��T���̏I�[���w���C�e���[�^��Ԃ�
		WFItr endWF() { return WFItr(get(), 0); }
		/// �[���D��T���̍ŏ����w���C�e���[�^��Ԃ�
		ConstWFItr beginWF() const { return ConstWFItr(get(), get()); }
		/// �[���D��T���̏I�[���w���C�e���[�^��Ԃ�
		ConstWFItr endWF() const { return ConstWFItr(get(), 0); }

		/// ���[�g�������C�e���[�^
		Itr root() { return Itr(get()); }
		/// ���[�g�������C�e���[�^
		ConstItr root() const { return ConstItr(get()); }

		/// ���[�g�����邩�H
		bool empty() const { return !get(); }

		/// �q�̐擪
		NodeType &front() { return get()->front(); }
		/// �q�̍Ō��
		NodeType &back() { return get()->back(); }
		/// �q�̐擪
		const NodeType &front() const { return get()->front(); }
		/// �q�̍Ō��
		const NodeType &back() const { return get()->back(); }

		/// �K�w�𕡐�
		Tree dup() const
		{
			return Tree(get()->dup());
		}
		
		template<class E, class T>
		void printIndented(std::basic_ostream<E, T> & os, int indent) const
		{
			get()->printIndented(os, indent);
		}
		
		template<class E, class T>
		void print(std::basic_ostream<E, T> & os) const
		{
			get()->printIndented(os, 0);
		}
	};

	template<class E, class T, typename TV>
	std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, Tree<TV> const & tree)
	{
		if(tree) return os << *tree;
		return os;
	}

} // namespace gctp

#endif //_GCTP_TREE_HPP_