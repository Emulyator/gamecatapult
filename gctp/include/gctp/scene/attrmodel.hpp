#ifndef _GCTP_SCENE_ATTRMODEL_HPP_
#define _GCTP_SCENE_ATTRMODEL_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ������񃂃f���N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:42:22
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/sphere.hpp>
#include <gctp/aabox.hpp>
#include <gctp/class.hpp>
#include <gctp/cstr.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/strutumnode.hpp>

class btTriangleIndexVertexArray;
class btBvhTriangleMeshShape;

namespace gctp { namespace scene {

	/** ������񃂃f���N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2008/06/21 17:55:36
	 * Copyright (C) 2008 SAM (T&GG, Org.). All rights reserved.
	 */
	class AttrModel : public Object {
	public:
		AttrModel();
		virtual ~AttrModel();

		/// ���łɃZ�b�g�A�b�v����Ă�����A���X�g�A
		HRslt setUp(CStr name, const void *data);

		void tearDown();
		
		const btBvhTriangleMeshShape *shape() const { return shape_; }

		/// ���O
		const char *name() const { return name_.c_str(); }
		/// ���O������
		void setName(const char *str) { name_ = str; }
		
	private:
		CStr								name_;		// �V�[���t�@�C����ł̖��O

		Vector								*vertices_;
		int									*indices_;
		btTriangleIndexVertexArray			*array_;
		btBvhTriangleMeshShape				*shape_;
		
		GCTP_DECLARE_TYPEINFO;
	};

	/// �������f���C���X�^���X
	class AttrFlesh : public Object {
	public:
		/// �Z�b�g�A�b�v
		HRslt setUp(Handle<AttrModel> model, Handle<StrutumNode> node);

		/// �Q�Ƃ��Ă��鑮�����f���ւ̃|�C���^
		Handle<AttrModel> &model() { return model_ ; }
		/// �Q�Ƃ��Ă��鑮�����f���ւ̃|�C���^
		Handle<AttrModel> model() const { return model_ ; }
		/// �A�^�b�`���Ă���m�[�h
		Handle<StrutumNode> node() const { return node_; }

		//Sphere bc();
		AABox getAABB();

	private:
		Handle<AttrModel>		model_;
		Handle<StrutumNode>		node_;
		GCTP_DECLARE_TYPEINFO;
	};

}} //namespace gctp::scene

#endif //_GCTP_SCENE_ATTRMODEL_HPP_