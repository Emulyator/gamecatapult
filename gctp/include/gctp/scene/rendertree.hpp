#ifndef _GCTP_SCENE_RENDERTREE_HPP_
#define _GCTP_SCENE_RENDERTREE_HPP_
/** @file
 * GameCatapult �����_�[�c���[�N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/scene/renderer.hpp>
#include <gctp/tree.hpp>
#include <gctp/tuki.hpp>
#include <gctp/stringmap.hpp>
#include <gctp/signal.hpp>
#include <iosfwd>               // for std::basic_ostream

namespace gctp { namespace scene {

	/** �����_�[�c���[�N���X
	 *
	 * Pointer<Renderer>��Tree�ł���_�ɒ���
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 18:38:41
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class RenderTree : public Object, public Tree< Handle<Renderer> >
	{
	public:
		typedef Tree< Handle<Renderer> > TreeType;

		/// �f�t�H���g�R���X�g���N�^
		RenderTree() { draw_slot.bind(this); }
		/// �R���X�g���N�^
		explicit RenderTree(Tree< Handle<Renderer> > const & src) : Tree< Handle<Renderer> >(src) { draw_slot.bind(this); }
		RenderTree(RenderTree &src)
		{
			root() = src.root();
			index = src.index;
			dummynode_ = 0;
		}
		
		/// ���[�g����
		void setUp(const Handle<Renderer> renderer)
		{
			root() = TreeType(renderer).root();
		}

		/// ����
		void merge(const RenderTree &src);

		/// �Q�Ɛ؂�m�[�h���폜
		void clean();
		
		/// �m�[�h��H���ĕ`��
		bool draw(float delta) const;
		
		/// �`��X���b�g
		ConstMemberSlot1<const RenderTree, float /*delta*/, &RenderTree::draw> draw_slot;

		typedef StringMap<NodeType *> NodeIndex;
		NodeIndex index;
		
		/// ���O�ɂ��m�[�h����
		NodeType *get(const char *name) const {
			return index.get(name);
		}
		/// ���O�ɂ��m�[�h����
		NodeType *get(const CStr name) const {
			return index.get(name);
		}
		/// ���O�ɂ��m�[�h����
		NodeType *operator[](const char *name) const {
			return get(name);
		}
		/// ���O�ɂ��m�[�h����
		NodeType *operator[](const CStr name) const {
			return get(name);
		}

		/// �m�[�h���疼�O������
		const char *getName(const NodeType &node) const {
			for(NodeIndex::const_iterator i = index.begin(); i != index.end(); ++i) {
				if(i->second == reinterpret_cast<const void *const>(&node)) return i->first.c_str();
			}
			return 0;
		}

	protected:
		bool setUp(luapp::Stack &L);
		int set(luapp::Stack &L);
		bool set(const luapp::Table &tbl, NodePtr parent_node, int &i);
		int get(luapp::Stack &L);
		void show(luapp::Stack &L);
		void hide(luapp::Stack &L);

	private:
		Pointer<Renderer> dummynode_;

	GCTP_DECLARE_CLASS
	TUKI_DECLARE(RenderTree)
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_RENDERTREE_HPP_