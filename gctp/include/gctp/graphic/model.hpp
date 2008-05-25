#ifndef _GCTP_GRAPHIC_MODEL_HPP_
#define _GCTP_GRAPHIC_MODEL_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ���f���N���X�w�b�_�t�@�C��
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/28 12:42:22
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic.hpp>
#include <gctp/line.hpp>
#include <gctp/sphere.hpp>
#include <gctp/aabox.hpp>
#include <gctp/skeleton.hpp>
#include <gctp/graphic/material.hpp>
#include <gctp/dxcomptrs.hpp>

#include <gctp/graphic/vertexbuffer.hpp>
#include <gctp/graphic/indexbuffer.hpp>

struct ID3DXMesh;
struct ID3DXSkinInfo;
namespace gctp {
	TYPEDEF_DXCOMPTR(ID3DXMesh);
	TYPEDEF_DXCOMPTR(ID3DXSkinInfo);
}

namespace gctp { namespace graphic {

	class Texture;
	class Shader;
	class Brush;

	struct SubsetInfo
	{
		uint material_no;
		uint index_offset;
		uint primitive_num;
		uint vertex_offset;
		uint vertex_num;
	};

	/** ���f���N���X
	 *
	 * @todo �ǂ��ɂ����āADX�ŗL�̕�����ǂ��o���������c
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 15:10:10
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Model : public Object {
		friend class Brush;
	public:
		enum Type {
			TYPE_POLYGON,
			TYPE_LINE,
		};
		/// ���łɃZ�b�g�A�b�v����Ă�����A���X�g�A
		void setUp(CStr name, ID3DXMeshPtr mesh, ID3DXSkinInfoPtr skin, ID3DXBufferPtr adjc);
		/// ���łɃZ�b�g�A�b�v����Ă�����A���X�g�A
		HRslt setUpWire(CStr name, const void *data, const void *mlist, ID3DXBufferPtr mtrls, ulong mtrl_num);
		
		/// �`��
		HRslt draw(const Matrix &mat) const;
		/// �`��
		HRslt draw(const Skeleton &skel) const;

		/** �}�e���A�����w�肵�ĕ`��A�̏���
		 *
		 * @param template_mtrlno �Z�b�g�A�b�v����Brush/Shader���\����}�e���A���ԍ��B
		 * ����Brush/Shader�̑g�������̂ł���΁A�ǂ̃}�e���A���ł����܂�Ȃ�
		 */
		HRslt begin(int template_mtrlno, int passno) const;
		/// �}�e���A�����w�肵�ĕ`��
		HRslt draw(const Matrix &mat, int mtrlno) const;
		/// �}�e���A�����w�肵�ĕ`��
		HRslt draw(const Skeleton &skl, int mtrlno) const;
		/// �}�e���A�����w�肵�ĕ`��I��
		HRslt end() const;

		/// �}�e���A�����X�g
		std::vector<Material> mtrls;
		/// �ʐڑ����
		const ulong *adjacency() const { return reinterpret_cast<ulong*>(adjc_->GetBufferPointer()); }
		/// ���b�V���f�Џ��
		const std::vector<SubsetInfo> &subsets() const { return subsets_; }

		Model() : bs_(VectorC(0,0,0),0), offset_(0) { vb_.deleteGuard(); ib_.deleteGuard(); }

		/// ���E��
		const Sphere &bs() { return bs_; }

		/// ���E�����Čv�Z
		void updateBS()
		{
			// ���E�����v�Z
			bs_.c = calcCenter();
			bs_.r = calcRadius(bs_.c);
		}

		/// AABB��Ԃ�
		AABox getAABB() const;
		/// ���[���h���W�n�ł�AABB��Ԃ�
		AABox getAABB(const Matrix &mat) const;
		/// �����e�X�g
		bool hasIntersected(const RayLine &ray, ulong &idx, float &u, float &v, float &dist) const;
		/// �����e�X�g
		bool hasIntersected(const Model &with, Vector &mytouch, Vector &histouch) const;

		/// �������f�����畡��
		HRslt copy(const Model &src);

		/// �X�L�����A�b�v�f�[�g
		HRslt update(const Model &src, const Matrix &mat);

		/// �X�L�����f�����H
		bool isSkin();

		/// �X�L�����f�����ς݂��H
		bool isSkinned();

		/// �X�L�����f������߂�
		void solidize();
		/// �\�t�g�E�F�A�X�L�����f����
		HRslt useSoftware();
		/// ���_�u�����h�X�L�����f����
		HRslt useBlended();
		/// �C���f�b�N�X�����_�u�����h�X�L�����f����
		HRslt useIndexed();
		/// ���_�V�F�[�_�[�X�L�����f����
		HRslt useVS();
		/// HLSL�ɂ��X�L�����f����
		HRslt useShader();

		/// D3DX���b�V����Ԃ�
		ID3DXMeshPtr mesh() const { return mesh_; }
		/// D3DX�X�L������Ԃ�
		ID3DXSkinInfoPtr skin() const { return skin_; }

		/// �{�[����
		const char *bonename(uint i) { if(skin_) return skin_->GetBoneName(i); else return NULL; }
		/// �{�[���I�t�Z�b�g�s��
		Matrix *bone(uint i) { if(skin_) return reinterpret_cast<Matrix *>(skin_->GetBoneOffsetMatrix(i)); else return NULL; }
		/// �{�[����
		uint boneNum() { if(skin_) return skin_->GetNumBones(); else return 0; }

		/// ���O
		const char *name() const { return name_.c_str(); }
		/// ���O������
		void setName(const char *str) { name_ = str; }
		/// �i���[�t���́j�I�t�Z�b�g��ݒ�B�������f���̏ꍇ���ꂪ�K�v
		void setOffset(int offset) { offset_ = offset; }
		/// �i���[�t���́j�I�t�Z�b�g
		int offset(int offset) { return offset_; }

		/// �u�����h
		void blend(const Model **models, Real *weights, int num);

		VertexBuffer &vertexbuffer() { return vb_; }
		const VertexBuffer &vertexbuffer() const { return vb_; }
		IndexBuffer &indexbuffer() { return ib_; }
		const IndexBuffer &indexbuffer() const { return ib_; }

	protected:
		CStr								name_;		// �V�[���t�@�C����ł̖��O
		Type								type_;

		ID3DXMeshPtr						mesh_;      // �t�@�C������ǂݍ��񂾂܂܂̂̃��b�V��
		ID3DXSkinInfoPtr					skin_;		// �X�L�����
		ID3DXBufferPtr						adjc_;		// �ʂ̐ڑ����

		std::vector<SubsetInfo>				subsets_;	// �f�Џ��
		mutable VertexBuffer				vb_;
		mutable IndexBuffer					ib_;

		Vector calcCenter() const;
		float calcRadius(const Vector &center) const;

	private:
		mutable int current_template_mtrlno_;
		//std::vector< Pointer<Brush> > brushes_;
		Pointer<Brush> brush_;
		Sphere bs_;
		int offset_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_MODEL_HPP_