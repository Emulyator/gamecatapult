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

	/** D3DX���b�V�����_�f�[�^�̃��b�N�Ǝ����J��
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 15:38:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class MeshVertexLock {
		ID3DXMeshPtr mesh_;
		void *base_;
		void *p_;
	public:
		MeshVertexLock(ID3DXMeshPtr mesh)
		{
			if(mesh && HRslt(mesh->LockVertexBuffer(0, &p_)) && p_) {
				base_ = p_;
				mesh_ = mesh;
			}
		}
		~MeshVertexLock()
		{
			if(mesh_) mesh_->UnlockVertexBuffer();
		}
		void step()
		{
			p_ = ((uchar *)p_) + D3DXGetFVFVertexSize(mesh_->GetFVF());
		}

		Vector &operator[](ushort i)
		{
			return *reinterpret_cast<Vector *>(((uchar *)base_) + D3DXGetFVFVertexSize(mesh_->GetFVF())*i);
		}
		const Vector &operator[](ushort i) const
		{
			return *reinterpret_cast<const Vector *>(((const uchar *)base_) + D3DXGetFVFVertexSize(mesh_->GetFVF())*i);
		}
		void *get() { return p_; }
		const void *get() const { return p_; }
		operator Vector *() { return reinterpret_cast<Vector *>(p_); }
		operator const Vector *() const { return reinterpret_cast<Vector *>(p_); }
	};

	/** D3DX���b�V���C���f�b�N�X�f�[�^�̃��b�N�Ǝ����J��
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 15:38:20
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class MeshIndexLock {
		ID3DXMeshPtr mesh_;
		void *base_;
		void *p_;
	public:
		MeshIndexLock(ID3DXMeshPtr mesh)
		{
			if(mesh && HRslt(mesh->LockIndexBuffer(0, &p_)) && p_) {
				base_ = p_;
				mesh_ = mesh;
			}
		}
		~MeshIndexLock()
		{
			if(mesh_) mesh_->UnlockIndexBuffer();
		}
		void step()
		{
			p_ = ((uchar *)p_) + sizeof(ushort);
		}

		ushort &operator[](std::size_t i)
		{
			return *reinterpret_cast<ushort *>(((uchar *)base_) + sizeof(ushort)*i);
		}
		const ushort &operator[](std::size_t i) const
		{
			return *reinterpret_cast<const ushort *>(((const uchar *)base_) + sizeof(ushort)*i);
		}
		void *get() { return p_; }
		const void *get() const { return p_; }
		operator ushort *() { return reinterpret_cast<ushort *>(p_); }
		operator const ushort *() const { return reinterpret_cast<ushort *>(p_); }
	};

	struct SubsetInfo
	{
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

		/// �}�e���A�����w�肵�ĕ`��A�̏���
		HRslt begin() const;
		/// �}�e���A�����w�肵�ĕ`��
		HRslt drawSubset(const Matrix &mat, int mtrlno) const;
		/// �}�e���A�����w�肵�ĕ`��
		HRslt drawSubset(const Skeleton &skl, int mtrlno) const;
		/// �}�e���A�����w�肵�ĕ`��I��
		HRslt end() const;

		/// �}�e���A�����X�g
		std::vector<Material>	mtrls;
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
		/// �g�p����gctp::Shader��Ԃ�
		Handle<Shader> shader() const { return shader_; }
		/// �g�p����gctp::Shader��ݒ肷��
		void setShader(Handle<Shader> shader);

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
		Pointer<Brush> brush_;
		mutable Handle<Shader> shader_;
		Sphere bs_;
		int offset_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_MODEL_HPP_