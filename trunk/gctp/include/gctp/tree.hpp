#ifndef _GCTP_TREE_HPP_
#define _GCTP_TREE_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ツリーテンプレートヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 19:28:21
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointer.hpp>
#include <boost/iterator_adaptors.hpp>
#include <boost/function.hpp>

namespace gctp {

	/** ノード型のベース
	 *
	 * 共通部分をテンプレートを用いず実装
	 */
	class TreeNodeBase : public Object {
	public:
		/// コンストラクタ
		TreeNodeBase() : parent_(0), older_(0) {}

		/// 子リストの最初を指すポインタを返す
		TreeNodeBase *begin() { return children_.get(); }
		/// 子リストの終端を指すポインタを返す
		TreeNodeBase *end() { return 0; }

		/// 子リストの最初を指すポインタを返す
		const TreeNodeBase *begin() const { return children_.get(); }
		/// 子リストの終端を指すポインタを返す
		const TreeNodeBase *end() const { return 0; }

		/// posの直前に子を挿入
		void insert(TreeNodeBase *pos, Pointer<TreeNodeBase> node)
		{
			/// デバッグ版では本とは渡されたイテレータが本当に自分の子か調べたほうが良いな
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
		/// posを削除し、直後のItrを返す
		TreeNodeBase *erase(TreeNodeBase *pos)
		{
			if(pos != end()) {
				TreeNodeBase *ret = pos->younger_.get();
				pos->remove();
				return ret;
			}
			return end();
		}

		/// 自分をはずす
		void remove()
		{
			Pointer<TreeNodeBase> lock = this;
			// 兄弟関係を切る
			if(!_isFront() && older_) older_->younger_ = younger_;
			if(younger_) younger_->older_ = older_;
			// 親子関係を切る
			if(parent_ && parent_->children_ == this) parent_->children_ = 0;
			younger_ = older_ = parent_ = 0;
		}
		/// 子に追加
		void push(Pointer<TreeNodeBase> node)
		{
			if(children_) {
				if(children_->older_) { // 先頭のolder_はbackを指す
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
		/// 子があるか？
		bool empty() const { return !children_; }
		/// 子の先頭
		TreeNodeBase &front() { return *children_; }
		/// 子の最後尾
		TreeNodeBase &back() { return *children_->older_; }
		/// 子の先頭
		const TreeNodeBase &front() const { return *children_; }
		/// 子の最後尾
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
		// 先端か？
		bool _isFront() const
		{
			return ((parent_ && parent_->children_==this) || older_ == 0);
		}
		// 兄弟リストの前端
		TreeNodeBase &_front()
		{
			if(_isFront()) return older_->_front();
			else return *this;
		}
		// 兄弟リストの後端
		TreeNodeBase &_back()
		{
			if(younger_) return younger_->_back();
			else return *this;
		}
		// 自分の直後に挿入
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
	struct TreeTraverseIteratorBase {
		typedef size_t                     size_type;
		typedef ptrdiff_t                  difference_type;
		typedef forward_iterator_tag       iterator_category;

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
	struct TreeTraverseIteratorBase : public TreeTraverseIteratorBase {
		const TreeNodeBase *root_;
		TreeChildIteratorBase(const TreeNodeBase *root, const TreeNodeBase *node) : TreeIteratorBase(node), root_(root) {}
		TreeChildIteratorBase() {}
		void increment()
		{ 
			if(node_->children_) node_ = &node_->front();
			else node_ = TreeNodeBase::_next_DF(root_, node_);
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

	/** ノードの型テンプレート
	 *
	 * Object派生でPointerで捕捉する
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
		//typedef typename Alloc::template rebind<TreeNode>::other AllocatorType;// 何でこんな書き方を？
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

		/// ファクトリーメソッド（new(TreeNode<T>::allocator()) TreeNode<T>()とするより楽）
		static TreeNode *create()
		{
			return new(allocator()) TreeNode;
		}
		/// ファクトリーメソッド（new(TreeNode<T>::allocator()) TreeNode<T>(src)とするより楽）
		static TreeNode *create(const T &src)
		{
			return new(allocator()) TreeNode(src);
		}

#ifndef BOOST_FUNCTION_NO_FUNCTION_TYPE_SYNTAX
		/// トラバース関数のビジター型 (深さ優先探索をする場合、if(!node->visitChildren(visitor)) return false;をvisitorの中で行い再帰探索するようにする)
		typedef boost::function<bool (TreeNode &)> Visitor;
		/// トラバース関数のビジター型（const版）  (深さ優先探索をする場合、if(!node->visitChildren(visitor)) return falseをvisitorの中で行い再帰探索するようにする)
		typedef boost::function<bool (const TreeNode &)> ConstVisitor;
#else
		/// トラバース関数のビジター型 (深さ優先探索をする場合、if(!node->visitChildren(visitor)) return false;をvisitorの中で行い再帰探索するようにする)
		typedef boost::function1<bool, TreeNode &> Visitor;
		/// トラバース関数のビジター型（const版）  (深さ優先探索をする場合、if(!node->visitChildren(visitor)) return falseをvisitorの中で行い再帰探索するようにする)
		typedef boost::function1<bool, const TreeNode &> ConstVisitor;
#endif
#ifdef GCTP_DONT_USE_BOOST_ITERATOR_ADAPTOR
		/// 兄弟イテレータ
		typedef TreeNodeIterator<TreeNode, TreeSiblingIteratorBase> SiblingItr;
		/// 兄弟イテレータ
		typedef ConstTreeNodeIterator<TreeNode, TreeSiblingIteratorBase> ConstSiblingItr;
		/// 深さ優先探索イテレータ
		typedef TreeNodeIterator<TreeNode, TreeTraverseIteratorBase> TraverseItr;
		/// 深さ優先探索イテレータ
		typedef ConstTreeNodeIterator<TreeNode, TreeTraverseIteratorBase> ConstTraverseItr;
#else
		/// 兄弟イテレータ
		struct SiblingItr : boost::iterator_adaptor<SiblingItr, TreeNode*, boost::use_default, boost::bidirectional_traversal_tag>
		{
			SiblingItr( TreeNode* p ) : boost::iterator_adaptor<SiblingItr, TreeNode*, boost::use_default, boost::bidirectional_traversal_tag>( p ) {}
			void increment() { base_reference() = reinterpret_cast<TreeNode*>(base()->younger_.get()); }
			void decrement() { if(base()->_isFront()) base_reference() = 0; else base_reference() = reinterpret_cast<TreeNode*>(base()->older_); }
		};

		/// 兄弟イテレータ
		struct ConstSiblingItr : boost::iterator_adaptor<ConstSiblingItr, const TreeNode*, boost::use_default, boost::bidirectional_traversal_tag>
		{
			ConstSiblingItr( const TreeNode* p ) : boost::iterator_adaptor<ConstSiblingItr, const TreeNode*, boost::use_default, boost::bidirectional_traversal_tag>( p ) {}
			void increment() { base_reference() = reinterpret_cast<const TreeNode *>(base()->younger_.get()); }
			void decrement() { if(base()->_isFront()) base_reference() = 0; else base_reference() = reinterpret_cast<const TreeNode *>(base()->older_); }
		};

		/// 深さ優先探索イテレータ
		struct TraverseItr : boost::iterator_adaptor<TraverseItr, TreeNode *, boost::use_default, boost::forward_traversal_tag>
		{
			TreeNode *root_;
			TraverseItr(TreeNode *root, TreeNode *p) : boost::iterator_adaptor<TraverseItr, TreeNode *, boost::use_default, boost::forward_traversal_tag>( p ), root_(root) {}
			void increment()
			{
				base_reference() = reinterpret_cast<TreeNode *>(_next_DF(root_, base()));
			}
		};

		/// 深さ優先探索イテレータ
		struct ConstTraverseItr : boost::iterator_adaptor<ConstTraverseItr, const TreeNode *, boost::use_default, boost::forward_traversal_tag>
		{
			const TreeNode *root_;
			ConstTraverseItr(const TreeNode *root, const TreeNode *p) : boost::iterator_adaptor<ConstTraverseItr, const TreeNode *, boost::use_default, boost::forward_traversal_tag>( p ), root_(root) {}
			void increment()
			{
				base_reference() = reinterpret_cast<const TreeNode *>(_next_DF(root_, base()));
			}
		};
#endif
		/// 子リストの最初を指すイテレータを返す
		SiblingItr begin() { return SiblingItr(reinterpret_cast<TreeNode *>(TreeNodeBase::begin())); }
		/// 子リストの終端を指すイテレータを返す
		SiblingItr end() { return SiblingItr(reinterpret_cast<TreeNode *>(TreeNodeBase::end())); }
		/// 子リストの最初を指すイテレータを返す
		ConstSiblingItr begin() const { return ConstSiblingItr(reinterpret_cast<const TreeNode *>(TreeNodeBase::begin())); }
		/// 子リストの終端を指すイテレータを返す
		ConstSiblingItr end() const { return ConstSiblingItr(reinterpret_cast<const TreeNode *>(TreeNodeBase::end())); }

		/// 深さ優先探索の最初を指すイテレータを返す
		TraverseItr beginTraverse() { return TraverseItr(this, this); }
		/// 深さ優先探索の終端を指すイテレータを返す
		TraverseItr endTraverse() { return TraverseItr(this, 0); }
		/// 深さ優先探索の最初を指すイテレータを返す
		ConstTraverseItr beginTraverse() const { return ConstTraverseItr(this, this); }
		/// 深さ優先探索の終端を指すイテレータを返す
		ConstTraverseItr endTraverse() const { return ConstTraverseItr(this, 0); }

		/// 格納している値
		T val;

		/// コンストラクタ
		TreeNode() : TreeNodeBase() {}
		/// コンストラクタ
		TreeNode(const T &_val) : TreeNodeBase(), val(_val) {}

		/// 格納している値を返す
		Reference operator*() {
			return val;
		}
		/// 格納している値を返す
		ConstReference operator*() const {
			return val;
		}
		
		/// posの直前に子を挿入
		void insert(SiblingItr pos, Pointer<TreeNode> node)
		{
			/// デバッグ版では本とは渡されたイテレータが本当に自分の子か調べたほうが良いな
			TreeNodeBase::insert(&*pos, node);
		}
		/// posの直前に子を挿入
		SiblingItr insert(SiblingItr pos, const ValueType &val)
		{
			TreeNode *ret = new(allocator()) TreeNode(val);
			insert(pos, ret);
			return SiblingItr(ret);
		}
		/// posの直前に子を挿入
		SiblingItr insert(SiblingItr pos)
		{
			TreeNode *ret = new(allocator()) TreeNode();
			insert(pos, ret);
			return SiblingItr(ret);
		}
		/// posを削除し、直後のItrを返す
		SiblingItr erase(SiblingItr pos)
		{
			return SiblingItr(reinterpret_cast<TreeNode *>(TreeNodeBase::erase(&*pos)));
		}
		/// 子に追加
		SiblingItr push(Pointer<TreeNode> node)
		{
			TreeNodeBase::push(node);
			return SiblingItr(node.get());
		}
		/// 子に追加
		SiblingItr push(const ValueType &val)
		{
			TreeNode *ret = new(allocator()) TreeNode(val);
			push(ret);
			return SiblingItr(ret);
		}
		/// 子に追加
		SiblingItr push()
		{
			TreeNode *ret = new(allocator()) TreeNode();
			push(ret);
			return SiblingItr(ret);
		}

		struct FindValuePred {
			const T &rhs;
			FindValuePred(const T &rhs) : rhs(rhs) {}
			bool operator()(const TreeNode &lhs)
			{
				return lhs.val == rhs;
			}
		};

		/// 検索
		SiblingItr find(const T &val)
		{
			return std::find_if(begin(), end(), FindValuePred(val));
		}
		/// 検索
		ConstSiblingItr find(const T &val) const
		{
			return std::find_if(begin(), end(), FindValuePred(val));
		}

		/// 階層を複製
		Pointer<TreeNode> dup() const
		{
			Pointer<TreeNode> ret = new(allocator()) TreeNode(val);
			for(TreeNode::ConstSiblingItr it = begin(); it != end(); ++it) {
				ret->push(it->dup());
			}
			return ret;
		}

		template<class E, class T>
		void printIndented(std::basic_ostream<E, T> & os, int indent) const
		{
			for(int i = 0; i < indent; i++) os << "\t";
			os << val << std::endl;
			for(TreeNode::ConstSiblingItr it = begin(); it != end(); ++it) {
				it->printIndented(os, indent+1);
			}
		}

		/** 子ノード全部にビジターを送る
		 *
		 * 深さ優先探索の場合、visitorがnode->visitChidren(visitor)と再帰呼び出ししてやる必要がある\n
		 * 各ノードごとにビジターを呼び出す
		 * @return 中断されずにすべて実行したか？
		 * @param Visitor 各ノードごとの処理  中断する場合はfalseを返す
		 */
		bool visitChildren(Visitor visitor)
		{
			for(SiblingItr it = begin(); it != end(); ++it) {
				if(!visitor(*it)) return false;
			}
			return true;
		}
		/** 子ノード全部にビジターを送る(const版)
		 *
		 * 深さ優先探索の場合、visitorがnode->visitChidren(visitor)と再帰呼び出ししてやる必要がある\n
		 * 各ノードごとにビジターを呼び出す
		 * @return 中断されずにすべて実行したか？
		 * @param Visitor 各ノードごとの処理  中断する場合はfalseを返す
		 * @todo ほんとに名前変えなきゃダメ？(Pointer::getが非constポインタを返すのが良くない、っぽい)
		 */
		bool visitChildrenConst(ConstVisitor visitor) const
		{
			for(ConstSiblingItr it = begin(); it != end(); ++it) {
				if(!visitor(*it)) return false;
			}
			return true;
		}

		/// 親のポインタ
		TreeNode *parent() { return static_cast<TreeNode *>(parent_); }
		/// 子の先頭
		TreeNode &front() { return static_cast<TreeNode &>(TreeNodeBase::front()); }
		/// 子の最後尾
		TreeNode &back() { return static_cast<TreeNode &>(TreeNodeBase::back()); }
		/// 子の先頭
		const TreeNode &front() const { return static_cast<const TreeNode &>(TreeNodeBase::front()); }
		/// 子の最後尾
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

	/** ツリークラステンプレートクラス
	 *
	 * 唯一のルートがあり、閉路を持たない（循環参照なし、複数の親に属することもなし）単方向グラフ。
	 *
	 * ３Ｄグラフィックでの階層ツリーを表すことを目的にしているため、“摘み上げ”（今までとは違うノードをルートに指定し、
	 * 『重力に惹かれたように』親子関係修正する）は全く念頭におかない。単純なツリー。
	 *
	 * 格納する値は、デフォルトコンストラクタとコピーコンストラクタを持つ必要がある。
	 *
	 * ルートノードへのスマートポインタとして振舞う
	 * @code
	    Tree< TreeNode<Foo> > tree;
	   @endcode
	 * @note allocator指定はノードの確保のためだけに使用され、Tree自身の確保には関与しない。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/27 2:51:14
	 */
	template<class T, class Alloc = std::allocator<T> >
	class Tree : public Pointer< TreeNode<T, Alloc> > {
	public:
		typedef TreeNode<T, Alloc>						NodeType;
		typedef Pointer<NodeType>						NodePtr;
		typedef typename NodeType::ValueType			ValueType;
		typedef typename NodeType::SiblingItr			SiblingItr;
		typedef typename NodeType::ConstSiblingItr		ConstSiblingItr;
		typedef typename NodeType::TraverseItr			TraverseItr;
		typedef typename NodeType::ConstTraverseItr		ConstTraverseItr;
		typedef typename NodeType::Visitor				Visitor;
		typedef typename NodeType::ConstVisitor			ConstVisitor;

		/// ツリーノードハンドル
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
		/// ツリーノードポインター
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

		/// デフォルトコンストラクタ
		Tree() {}
		/// コンストラクタ
		Tree(const ValueType &val) : NodePtr(NodeType::create(val)) {}
		/// コンストラクタ
		Tree(NodePtr const & rhs): NodePtr(rhs) {}

		/** visitor呼び出し
		 *
		 * ルートノードを訪問
		 */
		void visit(Visitor visitor)
		{
			if(get()) visitor(*get());
		}
		/** visitor呼び出し(const版)
		 *
		 * ルートノードを訪問
		 */
		void visit(ConstVisitor visitor) const
		{
			if(get()) visitor(*get());
		}

		/// 子リストの最初を指すイテレータを返す
		SiblingItr begin() { return SiblingItr(get()); }
		/// 子リストの終端を指すイテレータを返す
		SiblingItr end() { return SiblingItr(0); }
		/// 子リストの最初を指すイテレータを返す
		ConstSiblingItr begin() const { return ConstSiblingItr(get()->children_.get()); }
		/// 子リストの終端を指すイテレータを返す
		ConstSiblingItr end() const { return ConstSiblingItr(0); }

		/// 深さ優先探索の最初を指すイテレータを返す
		TraverseItr beginTraverse() { return TraverseItr(get(), get()); }
		/// 深さ優先探索の終端を指すイテレータを返す
		TraverseItr endTraverse() { return TraverseItr(get(), 0); }
		/// 深さ優先探索の最初を指すイテレータを返す
		ConstTraverseItr beginTraverse() const { return ConstTraverseItr(get(), get()); }
		/// 深さ優先探索の終端を指すイテレータを返す
		ConstTraverseItr endTraverse() const { return ConstTraverseItr(get(), 0); }

		/// ルートをさすイテレータ
		SiblingItr root() { return SiblingItr(get()); }
		/// ルートをさすイテレータ
		ConstSiblingItr root() const { return ConstSiblingItr(get()); }

		/// ルートがあるか？
		bool empty() const { return !get(); }

		/// 子の先頭
		NodeType &front() { return get()->front(); }
		/// 子の最後尾
		NodeType &back() { return get()->back(); }
		/// 子の先頭
		const NodeType &front() const { return get()->front(); }
		/// 子の最後尾
		const NodeType &back() const { return get()->back(); }

		/// 階層を複製
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