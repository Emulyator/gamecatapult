#ifndef _GCTP_SKELETON_HPP_
#define _GCTP_SKELETON_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���f���̍��g�݊K�w�\���N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/strutumtree.hpp>
#include <gctp/stringmap.hpp>
#include <iosfwd>               // for std::basic_ostream

namespace gctp {

	/** ���f���̍��g�݊K�w�\���N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:38:41
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Skeleton : public StrutumTree
	{
	public:
		Skeleton() {}
		Skeleton(const StrutumTree &src) : StrutumTree(src) {}

		/// ����
		void copy(const Skeleton &src);
		/// �Z��
		void merge(const Skeleton &src);

		/// ���[�g�m�[�h����
		void setUp(const Matrix &value = MatrixC(true)) {
			StrutumTree::setUp(value);
		}
		/// ���[�g�m�[�h����
		void setUp(const char *name, const Matrix &value = MatrixC(true)) {
			StrutumTree::setUp(value);
			if(name && strlen(name)) index[name] = this;
		}

		/// �q�m�[�h����
		NodePtr add(NodeType &parent, const char *name = NULL, const Matrix *_new = 0);
		/// �q�m�[�h����
		NodePtr add(NodeType &parent, const CStr name, const Matrix *_new = 0);

		/// ���O�ɂ��m�[�h����
		NodePtr get(const char *name) const {
			return index.get(name);
		}
		/// ���O�ɂ��m�[�h����
		NodePtr get(const CStr name) const {
			return index.get(name);
		}
		/// ���O�ɂ��m�[�h����
		NodePtr operator[](const char *name) const {
			return get(name);
		}
		/// ���O�ɂ��m�[�h����
		NodePtr operator[](const CStr name) const {
			return get(name);
		}

		/// �m�[�h���疼�O������
		const char *getName(const NodeType &node) const {
			for(NodeIndex::const_iterator i = index.begin(); i != index.end(); ++i) {
				if(i->second == reinterpret_cast<const void *const>(&node)) return i->first.c_str();
			}
			return 0;
		}

		/// ���X�P���g���̓��e�ŏ㏑��
		Skeleton &sync(const Skeleton &src);

		/// �v�Z�ς݃}�g���N�X���Z�b�g
		Skeleton &setTransform() {
			StrutumTree::setTransform();
			return *this;
		}
		/// �K�w�����ׂĒP�ʍs���
		Skeleton &setIdentity() {
			StrutumTree::setIdentity();
			return *this;
		}

		typedef StringMap<NodeType *> NodeIndex;
		NodeIndex index;

	GCTP_DECLARE_CLASS

	private:
		template<class E, class T>
		class _PrintVisitor {
			std::basic_ostream<E, T> & os_;
			const Skeleton &skel_;
			int indent_;
		public:
			_PrintVisitor(std::basic_ostream<E, T> &os, const Skeleton &skel, int indent) : os_(os), skel_(skel), indent_(indent) {}
			bool operator()(const NodeType &n)
			{
				for(int i = 0; i < indent_; i++) os << "\t";
				const char *nodename = src.getName(n);
				if(nodename) os_ << nodename << std::endl;
				os_ << *n << endl;
				indent_++;
				n.visitChidrenConst(*this);
				indent_--;
				return true;
			}
		};

	public:
		/// �C���f���g�t���ŏo��
		template<class E, class T>
		void printIndented(std::basic_ostream<E, T> & os, int indent) const
		{
			_PrintVisitor visitor(os, *this, indent);
			visit(visitor);
		}
		
		/// �C���f���g�t���ŏo��
		template<class E, class T>
		void print(std::basic_ostream<E, T> & os) const
		{
			printIndented(os, 0);
		}
	};

	template<class E, class T>
	std::basic_ostream<E, T> &operator<< (std::basic_ostream<E, T> & os, const Skeleton & skl)
	{
		if(!skl.empty()) {
			os << *skl << ";BoneName{";
			for(Skeleton::NodeIndex::const_iterator i = skl.index.begin(); i != skl.index.end(); ++i) {
				os << i->first << ":" << i->second;
			}
			os << "}";
		}
		return os;
	}

} // namespace gctp

#endif //_GCTP_SKELETON_HPP_