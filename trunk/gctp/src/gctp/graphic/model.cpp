/** @file
 * GameCatapult �X�L�����b�V���N���X
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: model.cpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/graphic/model.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/graphic/shader.hpp>
#include <gctp/graphic/light.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic/vertexbuffer.hpp>
#include <gctp/graphic/indexbuffer.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/dx/hlslshader.hpp>
#include <gctp/dbgout.hpp>
#include <rmxfguid.h>

using namespace std;
using gctp::graphic::dx::IDirect3DDevicePtr;
using gctp::graphic::dx::IDirect3DVertexBufferPtr;
using gctp::graphic::dx::IDirect3DIndexBufferPtr;

namespace gctp { namespace graphic {

	/** ���f������
	 *
	 * ���łɃZ�b�g�A�b�v����Ă�����A���X�g�A
	 */
	void Model::setUp(CStr name, ID3DXMeshPtr mesh, ID3DXSkinInfoPtr skin, ID3DXBufferPtr adjc)
	{
		name_ = name;
		mesh_ = mesh;
		skin_ = skin;
		adjc_ = adjc;

		if(mesh_) {
			DWORD size;
			mesh_->GetAttributeTable(NULL, &size);
			subsets_.resize(size);
			mesh_->GetAttributeTable(&subsets_[0], &size);
			dx::IDirect3DVertexBufferPtr vb;
			mesh_->GetVertexBuffer(&vb);
			vb_.setUp(vb);
			dx::IDirect3DIndexBufferPtr ib;
			mesh_->GetIndexBuffer(&ib);
			ib_.setUp(ib);
		}
		else {
			vb_.setUp(0);
			ib_.setUp(0);
			subsets_.resize(0);
		}
		if(isSkin() && !isSkinned()) useShader(); // �f�t�H���g��HLSL
		updateBS();
	}

	/** ���C�����f������
	 *
	 * �����܂ŕ\������Model�A�Ƃ����݌v����邽�߂̎b�菈�u�c
	 * �����ƃX�}�[�g�Ȏ�i���l���܂��傤
	 */
	HRslt Model::setUpWire(CStr name, const void *data, const void *mlist, ID3DXBufferPtr mtrls, ulong mtrl_num)
	{
		name_ = name;
		if(!wire_) wire_ = new WireMesh;
		HRslt hr = wire_->setUp(data, mlist, mtrls, mtrl_num);
		if(hr) updateBS();
		return hr;
	}

	/** �u���V�ݒ�
	 *
	 */
	void Model::setShader(Handle<Shader> shader)
	{
		shader_ = shader;
	}

	/** ���f���𕡎�
	 *
	 */
	HRslt Model::copy(const Model &src)
	{
		if(src.mesh_) {
			HRslt ret;
			IDirect3DDevicePtr dev;
			ret = src.mesh_->GetDevice(&dev);
			if(!ret) return ret;
			dx::D3DVERTEXELEMENT declaration[MAX_FVF_DECL_SIZE];
			ret = src.mesh_->GetDeclaration(declaration);
			if(!ret) return ret;
			ret = src.mesh_->CloneMesh(D3DXMESH_SYSTEMMEM, declaration, dev, &mesh_);
			if(!ret) return ret;
			
			mtrls.resize(src.mtrls.size());
			for(size_t i = 0; i < src.mtrls.size(); i++) mtrls[i] = src.mtrls[i];

			name_ = src.name_;
			detail_ = 0;
			return ret;
		}
		return CO_E_NOTINITIALIZED;
	}

	HRslt Model::update(const Model &src, const Matrix &mat)
	{
		if(!mesh_) return CO_E_NOTINITIALIZED;
		if(!src.mesh_) return CO_E_NOTINITIALIZED;
		uint vnum = (max)(mesh_->GetNumVertices(), src.mesh_->GetNumVertices());

		MeshVertexLock _dst(mesh_);
		MeshVertexLock _src(src.mesh_);
		if(_dst && _src) {
			for(uint i = 0; i < vnum; i++, _dst.step(), _src.step()) {
				*_dst = mat * *_src;
			}
		}
		return S_OK;
	}

	Vector Model::calcCenter() const
	{
		AABox aabb = getAABB();
		return aabb.center();
	}

	float Model::calcRadius(const Vector &center) const
	{
		if(!mesh_) return 0;
		float d, ret = 0;
		
		uint vnum = mesh_->GetNumVertices();

		MeshVertexLock vbl(mesh_);
		if(vbl) {
			for(uint i=0; i < vnum; i++, vbl.step()) {
				Vector *pv = vbl;
				//if((pv->x == 0.0) && (pv->y == 0.0) && (pv->z == 0.0)) continue; // �Ȃ�ŁH
				d = distance(*pv, center);
				if(d > ret) ret = d;
			}
		}
		return ret;
	}

	AABox Model::getAABB() const
	{
		if(wire_) return wire_->getAABB();
		if(!mesh_) return AABox(VectorC(0, 0, 0));
		AABox ret;
		
		uint vnum = mesh_->GetNumVertices();

		MeshVertexLock vbl(mesh_);
		if(vbl) {
			for(uint i = 0; i < vnum; i++, vbl.step()) {
				Vector *pv = vbl;
				if(i == 0) ret.initialize(*pv);
				else ret.add(*pv);
			}
		}
		return ret;
	}

	AABox Model::getAABB(const Matrix &mat) const
	{
		if(wire_) return wire_->getAABB(mat);
		if(!mesh_) return AABox(VectorC(0, 0, 0));
		AABox ret;
		Vector vec;
		
		uint vnum = mesh_->GetNumVertices();
		MeshVertexLock vbl(mesh_);
		if(vbl) {
			for(uint i = 0; i < vnum; i++, vbl.step()) {
				//Vector *pv = vbl;
				//mat.transformPoint(vec, *pv);
				vec = mat * *vbl;
				if(i == 0) ret.initialize(vec);
				else ret.add(vec);
			}
		}
		return ret;
	}

	/** ���C�i�[�_�����������ɖ����ɐL�т���j�Ƃ̌���������s���Aboolean�Ō��ʂ�Ԃ��B
	 *
	 * @return �������Ă��邩�H
	 * @par ray �����e�X�g���郌�C
	 * @par idx �������Ă���ꍇ�A�Ԃ������|���S���̏������Ԃ����
	 * @par u �������Ă���ꍇ�A�Ԃ������|���S�����ʂ�u�l���Ԃ����
	 * @par v �������Ă���ꍇ�A�Ԃ������|���S�����ʂ�v�l���Ԃ����
	 * @par dist �������Ă���ꍇ�A���C�̎n�_�Ƃ̋������Ԃ����
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright &copy; 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/20 16:33:00
	 */
	bool Model::hasIntersected(const RayLine &ray, ulong &idx, float &u, float &v, float &dist) const
	{
		HRslt hr;
		BOOL ret;
		hr = D3DXIntersect(mesh_, ray.s, ray.v, &ret, &idx, &u, &v, &dist, 0, 0);
		if(!hr) PRNN(hr);
		return (ret == TRUE);
	}

	/** ���̃��f���Ƃ̌���������s���Aboolean�Ō��ʂ�Ԃ��B
	 *
	 * @return �������Ă��邩�H
	 * @par with �����e�X�g���郂�f��
	 * @par mytouch �������Ă���ꍇ�A���̃��f����ł̐ڐG�_�i�̍ŋߖT���_�j
	 * @par histouch �������Ă���ꍇ�Awith�œn���ꂽ���f����ł̐ڐG�_
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright &copy; 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/20 16:33:00
	 */
	bool Model::hasIntersected(const Model &with, Vector &mytouch, Vector &histouch) const
	{
		MeshVertexLock vbl(with.mesh_);
		MeshIndexLock ibl(with.mesh_);
		RayLine ray;
		ulong idx;
		float u, v, dist, sdist;
		for(uint i = 0; i < with.mesh_->GetNumFaces(); i++) {
			for(int j = 0; j < 6; j++) {
				switch(j) {
				case 0: ray.s = vbl[ibl[i*3]]; ray.v = vbl[ibl[i*3+1]] - vbl[ibl[i*3]]; break;
				case 1: ray.s = vbl[ibl[i*3+1]]; ray.v = vbl[ibl[i*3+2]] - vbl[ibl[i*3+1]]; break;
				case 2: ray.s = vbl[ibl[i*3+2]]; ray.v = vbl[ibl[i*3]] - vbl[ibl[i*3+2]]; break;
				case 3: ray.s = vbl[ibl[i*3]]; ray.v = vbl[ibl[i*3+2]] - vbl[ibl[i*3]]; break;
				case 4: ray.s = vbl[ibl[i*3+1]]; ray.v = vbl[ibl[i*3]] - vbl[ibl[i*3+1]]; break;
				case 5: ray.s = vbl[ibl[i*3+2]]; ray.v = vbl[ibl[i*3+1]] - vbl[ibl[i*3+2]]; break;
				}
				sdist = ray.v.length();
				ray.v.normalize();
				if(hasIntersected(ray, idx, u, v, dist)) {
					if(dist < sdist) {
						MeshVertexLock myvbl(mesh_);
						MeshIndexLock myibl(mesh_);
						mytouch = myvbl[myibl[(size_t)idx*3]];
						histouch = ray.s + ray.v*dist;
						return true;
					}
				}
			}
		}
		return false;
	}

	/** �\���b�h���f���Ƃ��ĕ`��
	 *
	 * �������̂��܂܂�A�x���`�������ꍇ�Amat�̃C���X�^���X��drawDelay���Ă΂��܂�
	 * ���݂��Ă��Ȃ���΂����Ȃ��B
	 * @param mat ���f���̍��W�n
	 * @param delay �x���`�悩�H(�f�t�H���gfalse�j
	 *
	 * @todo Pointer<Matrix>�ł�p�ӂ��āA�x���`��͂������݂̂ɂ���H
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 15:13:42
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Model::draw(const Matrix &mat) const
	{
		HRslt hr;
		if(wire_) {
			Handle<dx::HLSLShader> shader = shader_;
			if(shader && *shader) {
				Matrix wv = mat*getView();
//				hr = (*shader)->SetMatrix("WorldView", &wv);
				hr = (*shader)->SetMatrix("g_mWorldView", wv);
				if(!hr) GCTP_TRACE(hr);
//				hr = (*shader)->SetMatrix("Projection", &getProjection());
				Matrix wvp = wv*getProjection();
				hr = (*shader)->SetMatrix("g_mWorldViewProjection", wvp);
				if(!hr) GCTP_TRACE(hr);
			}
			else {
				device().impl()->SetVertexShader(NULL);
				device().impl()->SetTransform(D3DTS_WORLD, mat);
			}
			if(mtrls.size() > 0) {
				for(uint i = 0; i < mtrls.size(); i++) {
					device().setMaterial(mtrls[i]);
					Handle<dx::HLSLShader> shader = shader_;
					if(shader && (hr = shader->begin())) {
						for(uint ipass = 0; ipass < shader->passnum(); ipass++) {
							hr = shader->beginPass( ipass );
							if(!hr) GCTP_TRACE(hr);
							hr = wire_->draw(i);
							if(!hr) GCTP_TRACE(hr);
							hr = shader->endPass();
							if(!hr) GCTP_TRACE(hr);
						}
						hr = shader->end();
						if(!hr) GCTP_TRACE(hr);
					}
					else hr = wire_->draw(i);

					if(!hr) return hr;
				}
			}
			else {
				Material mtrl;
				mtrl.setUp();
				device().setMaterial(mtrl);
				hr = wire_->draw();
				if(!hr) return hr;
			}
		}
		if(mesh_) {
			Handle<dx::HLSLShader> shader = shader_;
			if(shader && *shader) {
				Matrix wv = mat*getView();
//				hr = (*shader)->SetMatrix("WorldView", &wv);
				hr = (*shader)->SetMatrix("g_mWorldView", wv);
				if(!hr) GCTP_TRACE(hr);
//				hr = (*shader)->SetMatrix("Projection", &getProjection());
				Matrix wvp = wv*getProjection();
				hr = (*shader)->SetMatrix("g_mWorldViewProjection", wvp);
				if(!hr) GCTP_TRACE(hr);
			}
			else {
				IDirect3DDevicePtr dev;
				mesh_->GetDevice(&dev);
				dev->SetVertexShader(NULL);
				dev->SetTransform(D3DTS_WORLD, mat);
			}
			for(uint i = 0; i < mtrls.size(); i++) {
				device().setMaterial(mtrls[i]);
				if(shader_ && (hr = shader_->begin())) {
					for(uint ipass = 0; ipass < shader->passnum(); ipass++) {
						hr = shader_->beginPass( ipass );
						if(!hr) GCTP_TRACE(hr);
						hr = mesh_->DrawSubset(i);
						if(!hr) GCTP_TRACE(hr);
						hr = shader_->endPass();
						if(!hr) GCTP_TRACE(hr);
					}
					hr = shader_->end();
					if(!hr) GCTP_TRACE(hr);
				}
				else hr = mesh_->DrawSubset(i);

				if(!hr) return hr;
			}
		}
		return hr;
	}

	HRslt Model::begin() const
	{
		HRslt hr;
		if(mesh_) {
			Handle<dx::HLSLShader> shader = shader_;
			if(shader && (*shader)) {
				hr = shader->begin();
			}
			else {
				IDirect3DDevicePtr dev;
				mesh_->GetDevice(&dev);
				dev->SetVertexShader(NULL);
			}
			{
				IDirect3DDevicePtr dev;
				mesh_->GetDevice(&dev);
				//dev->SetVertexDeclaration(decl_);
				hr = dev->SetFVF( mesh_->GetFVF() );
				if(!hr) return hr;
				hr = dev->SetIndices(ib_); //< ����Model.begin��
				if(!hr) return hr;
				hr = dev->SetStreamSource(0, vb_, 0, mesh_->GetNumBytesPerVertex());//< ����Model.beginSubset��
				if(!hr) return hr;
				//	Shader.begin
				//		Model.begin
				//				Model.drawSubset
				//				Model.drawSubset
				//				Model.drawSubset
				//		Model.end
				//		Model.begin
				//				Model.drawSubset
				//				Model.drawSubset
				//				Model.drawSubset
				//		Model.end
				//	Shader.end
				//	Model.drawSubset =
				//		for allpass
				//			Shader.beginPass
				//				DrawIndexedPrimitive
				//			Shader.endPass
				// �����H
				//
				//	��ʓI�ɐ؂�ւ��y�i���e�B�[��Shader��Model�炵���B�ƐM����
				// ���̉���ł͏�̂悤�ȏ��ʁB�������A
				//	Model.begin
				//		Shader.begin
				//				Model.drawSubset
				//				Model.drawSubset
				//				Model.drawSubset
				//		Shader.end
				//		Shader.begin
				//				Model.drawSubset
				//				Model.drawSubset
				//				Model.drawSubset
				//		Shader.end
				//	Model.end
				//
			}
			if(!hr) return hr;
		}
		return hr;
	}

	/** �}�e���A�����w�肵�āA�\���b�h���f���Ƃ��ĕ`��
	 *
	 * ��ɔ������̂̒x���`��p�B
	 * @param mat ���f���̍��W�n
	 * @param mtrlno �}�e���A���ԍ�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 15:13:42
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Model::drawSubset(const Matrix &mat, int mtrlno) const
	{
		HRslt hr;
		if(mesh_) {
			IDirect3DDevicePtr dev;
			mesh_->GetDevice(&dev);
			device().setMaterial(mtrls[mtrlno]);
			Handle<dx::HLSLShader> shader = shader_;
			if(shader && *shader) {
//				hr = (*shader)->SetMatrix("WorldView", &(mat*getView()));
				hr = (*shader)->SetMatrix("WorldView", mat);
				if(!hr) GCTP_TRACE(hr);
				hr = (*shader)->SetMatrix("Projection", getProjection());
				if(!hr) GCTP_TRACE(hr);

				// ���C�g�X�^�b�N��́A�ŏ��̃f�B���N�V���i�����C�g���̗p
				for(uint i = 0; i < lightNum(); i++) {
					DirectionalLight light;
					if(getLight(i, light)) {
						hr = (*shader)->SetVector("lightDir", Vector4C(-light.dir, 0));
						if(!hr) GCTP_TRACE(hr);
						hr = (*shader)->SetVector("lightDiffuse", (D3DXVECTOR4*)&light.diffuse);
						if(!hr) GCTP_TRACE(hr);
						break;
					}
				}
			}
			else {
				dev->SetVertexShader(NULL);
				dev->SetTransform(D3DTS_WORLD, mat);
			}
			if(shader) {
				hr = (*shader)->CommitChanges();
				if(!hr) GCTP_TRACE(hr);
				for(uint ipass = 0; ipass < shader->passnum(); ipass++) {
					hr = shader->beginPass( ipass );
					if(!hr) GCTP_TRACE(hr);
					hr = dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, subsets_[mtrlno].VertexStart, 0, subsets_[mtrlno].VertexCount, subsets_[mtrlno].FaceStart, subsets_[mtrlno].FaceCount);
					if(!hr) GCTP_TRACE(hr);
					hr = shader->endPass();
					if(!hr) GCTP_TRACE(hr);
				}
			}
			else hr = dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, subsets_[mtrlno].VertexStart, 0, subsets_[mtrlno].VertexCount, subsets_[mtrlno].FaceStart, subsets_[mtrlno].FaceCount);

			if(!hr) return hr;
		}
		return hr;
	}

	HRslt Model::end() const
	{
		HRslt hr;
		if(mesh_) {
			IDirect3DDevicePtr dev;
			mesh_->GetDevice(&dev);
			hr = dev->SetIndices(0);
			if(!hr) {
				GCTP_TRACE(hr);
				return hr;
			}
			hr = dev->SetStreamSource(0, 0, 0, 0);//< ����Model.beginSubset��
			if(!hr) {
				GCTP_TRACE(hr);
				return hr;
			}
			Handle<dx::HLSLShader> shader = shader_;
			if(shader) {
				hr = shader->end();
				if(!hr) {
					GCTP_TRACE(hr);
					return hr;
				}
			}
			if(!hr) return hr;
		}
		return hr;
	}

	/** (�X�L�j���O�ς݂ł���΁j�X�L�����f���Ƃ��ĕ`��
	 *
	 * �X�L�j���O����ĂȂ��ꍇ�ASkeleton�̃��[�g���g���ă\���b�h���f���Ƃ��ĕ`�悷��B<BR>
	 * �������̂��܂܂�A�x���`�������ꍇ�Askl�̃C���X�^���X��drawDelay���Ă΂��܂�
	 * ���݂��Ă��Ȃ���΂����Ȃ��B
	 * @param skl ���[�V�������Z�b�g���ꂽ�X�P���g��
	 * @param delay �x���`�悩�H(�f�t�H���gfalse�j
	 *
	 * @todo Pointer<Skelton>�ł�p�ӂ��āA�x���`��͂������݂̂ɂ���H
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 15:13:42
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Model::draw(const Skeleton &skl) const
	{
		if(detail_.get()) return detail_->draw(skl);
		else if(!skl.empty()) return draw(skl.root()->val.wtm());
		return E_FAIL;
	}

	/** (�X�L�j���O�ς݂ł���΁j�X�L�����f���Ƃ��ĕ`��
	 *
	 * ��ɔ������̂̒x���`��p�B
	 * @param mat ���f���̍��W�n
	 * @param mtrlno �}�e���A���ԍ�
     *
	 * @todo Pointer<Skelton>�ł�p�ӂ��āA�x���`��͂������݂̂ɂ���H
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 15:13:42
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt Model::drawSubset(const Skeleton &skl, int mtrlno) const
	{
		if(detail_) return detail_->draw(skl, mtrlno);
		else if(!skl.empty()) return drawSubset(skl.root()->val.wtm(), mtrlno);
		return E_FAIL;
	}

	bool Model::isSkin()
	{
		if(boneNum() > 0) return true;
		else return false;
	}

	bool Model::isSkinned()
	{
		if(detail_) return true;
		return false;
	}

	/// �\�t�g�E�F�A�X�L�����f���N���X
	class SoftwareSkinModel : public ModelDetail {
	public:
		SoftwareSkinModel(Model &owner) : ModelDetail(owner), bone_matricies_(NULL) {}
		~SoftwareSkinModel()
		{
			delete[] bone_matricies_;
		}

		HRslt setUp()
		{
			HRslt hr;
			IDirect3DDevicePtr dev;
			hr = owner_.mesh()->GetDevice(&dev);
			if(!hr) return hr;
			hr = owner_.mesh()->CloneMeshFVF(D3DXMESH_MANAGED, owner_.mesh()->GetFVF(), dev, &mesh_);
			if(!hr) return hr;

			ulong attr_num;
			hr = mesh_->GetAttributeTable(NULL, &attr_num);
			if(!hr) return hr;
			attr_tbl_.resize(attr_num);
			hr = mesh_->GetAttributeTable(&attr_tbl_[0], NULL);
			if(!hr) return hr;

			if(bone_matricies_) delete[] bone_matricies_;
			bone_matricies_ = new D3DXMATRIXA16[owner_.boneNum()];
			return hr;
		}

		/// �`��
		HRslt draw(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/) const
		{
			return drawLow(tree, -1);
		}
		/// �`��
		HRslt draw(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/, int mtrlno) const
		{
			return drawLow(tree, mtrlno);
		}

	protected:
		/// �`��
		HRslt drawLow(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/, int mtrlno) const
		{
			HRslt hr;
			if(mesh_) {
				IDirect3DDevicePtr dev;
				mesh_->GetDevice(&dev);

				dev->SetVertexShader(NULL);
				uint bonenum  = owner_.boneNum();
				// set up bone transforms
				for(uint i = 0; i < bonenum; i++) {
					Pointer<Skeleton::NodeType> node = tree.get(owner_.bonename(i));
					if(node) D3DXMatrixMultiply(const_cast<D3DXMATRIXA16 *>(&bone_matricies_[i]), *owner_.bone(i), node->val.wtm());
				}

				setWorld(MatrixC(true));

				MeshVertexLock src_vbl(owner_.mesh());
				MeshVertexLock dst_vbl(mesh_);
				// generate skinned mesh
				hr = owner_.skin()->UpdateSkinnedMesh(&bone_matricies_[0], NULL, src_vbl, dst_vbl);
				if(!hr) return hr;

				for(uint i = 0; i < attr_tbl_.size(); i++) {
					if(mtrlno != -1 && attr_tbl_[i].AttribId != static_cast<uint>(mtrlno)) break;
					device().setMaterial(owner_.mtrls[attr_tbl_[i].AttribId]);
					
					Pointer<Shader> shader = owner_.shader().get();
					if(shader && (hr = shader->begin())) {
						for(uint ipass = 0; ipass < shader->passnum(); ipass++) {
							hr = shader->beginPass( ipass );
							if(!hr) GCTP_TRACE(hr);
							hr = mesh_->DrawSubset(attr_tbl_[i].AttribId);
							if(!hr) GCTP_TRACE(hr);
							hr = shader->endPass();
							if(!hr) GCTP_TRACE(hr);
						}
						hr = shader->end();
						if(!hr) GCTP_TRACE(hr);
					}
					else hr = mesh_->DrawSubset(attr_tbl_[i].AttribId);

					if(!hr) return hr;
				}
			}
			return hr;
		}

		vector<D3DXATTRIBUTERANGE>	attr_tbl_;
		D3DXMATRIXA16				*bone_matricies_;
		ID3DXMeshPtr				mesh_;
	};

	/// ���_�u�����h�X�L�����f���N���X
	class BlendedSkinModel : public ModelDetail {
	public:
		BlendedSkinModel(Model &owner)
			: ModelDetail(owner), max_face_infl_(0), attr_split_(0), attr_num_(0)
		{}

		HRslt setUp()
		{
			HRslt hr;
			mesh_ = 0; bonecb_ = 0; attr_split_ = 0; attr_num_ = 0; max_face_infl_ = 0;
			
			IDirect3DDevicePtr dev;
			hr = owner_.mesh()->GetDevice(&dev);
			if(!hr) return hr;
			
			// Make sure we get HW caps
			hr = dev->SetSoftwareVertexProcessing(FALSE);

			dx::D3DCAPS caps;
			hr = dev->GetDeviceCaps(&caps);
			if(!hr) return hr;
			hr = owner_.skin()->ConvertToBlendedMesh(
				owner_.mesh(),
				D3DXMESH_MANAGED|D3DXMESHOPT_VERTEXCACHE/*����g���Ė����炵�����H*/,
				owner_.adjacency(),
				NULL,//pAdjacencyOut
				NULL,//pFaceRemap
				NULL,//ppVertexRemap
				&max_face_infl_,//pMaxFaceInfl
				&attr_num_,//pNumBoneCombinations
				&bonecb_,
				&mesh_);
			if(!hr) return hr;

			if(mesh_) {
				ID3DXBufferPtr message;
				HRslt hr = D3DXValidMesh(mesh_, owner_.adjacency(), &message);
				if(!hr) PRNN(hr << " : " << reinterpret_cast<char *>(message->GetBufferPointer()));
			}

			/* If the graphic can only do 2 matrix blends, ConvertToBlendedMesh cannot approximate all mesh_es to it
			   Thus we split the mesh_ in two parts: The part that uses at most 2 matrices and the rest. The first is
			   drawn using the graphic's HW vertex_ processing and the rest is drawn using SW vertex_ processing. */
			if(caps.MaxVertexBlendMatrices <= 2) {
				// calculate the index of the attribute table to split on
				D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());

				for(attr_split_ = 0; attr_split_ < attr_num_; attr_split_++) {
					ulong infl = 0;
					for(ulong winfl = 0; winfl < max_face_infl_; winfl++) {
						if(bonecb[attr_split_].BoneId[winfl] != UINT_MAX) infl++;
					}
					if(infl > caps.MaxVertexBlendMatrices) break;
				}

				// if there is both HW and SW, add the Software Processing flag
				if(attr_split_ < attr_num_) {
					ID3DXMeshPtr temp_mesh;
					hr = mesh_->CloneMeshFVF(D3DXMESH_SOFTWAREPROCESSING|mesh_->GetOptions(), mesh_->GetFVF(), dev, &temp_mesh);
					if(!hr) return hr;
					mesh_ = temp_mesh;
				}
			}
			else attr_split_ = attr_num_;
			return hr;
		}

		/// �`��
		HRslt draw(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/) const
		{
			return drawLow(tree, -1);
		}
		/// �`��
		HRslt draw(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/, int mtrlno) const
		{
			return drawLow(tree, mtrlno);
		}

	protected:
		/// �`��
		HRslt drawLow(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/, int mtrlno) const
		{
			HRslt hr;
			IDirect3DDevicePtr dev;
			hr = mesh_->GetDevice(&dev);
			if(!hr) return hr;
			
			// Make sure we get HW caps
			hr = dev->SetSoftwareVertexProcessing(FALSE);

			dx::D3DCAPS caps;
			hr = dev->GetDeviceCaps(&caps);
			if(!hr) return hr;
			
			dev->SetVertexShader(NULL);
			uint bonenum  = owner_.boneNum();
			ulong attr_id_prev = UNUSED32;
			D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
			// Draw using default vtx processing of the graphic (typically HW)
			for(ulong i = 0; i < attr_num_; i++) {
				if(mtrlno != -1 && bonecb[i].AttribId != static_cast<uint>(mtrlno)) break;
				ulong blend_num = 0;
				for(ulong j = 0; j < max_face_infl_; j++) {
					if(bonecb[i].BoneId[j] != UINT_MAX) blend_num = j;
				}

				if(caps.MaxVertexBlendMatrices >= blend_num + 1) {
					for(ulong j = 0; j < max_face_infl_; j++) {
						ulong matid = bonecb[i].BoneId[j];
						if(matid != UINT_MAX) {
							Pointer<Skeleton::NodeType> node = tree.get(owner_.bonename(matid));
							if(node) {
								dev->SetTransform(D3DTS_WORLDMATRIX(j), node->val.wtm());
								dev->MultiplyTransform(D3DTS_WORLDMATRIX(j), *owner_.bone(matid));
							}
						}
					}

					dev->SetRenderState(D3DRS_VERTEXBLEND, blend_num);

					if((attr_id_prev != bonecb[i].AttribId) || (attr_id_prev == UNUSED32)) {
						device().setMaterial(owner_.mtrls[bonecb[i].AttribId]);
						attr_id_prev  = bonecb[i].AttribId;
					}

					hr = mesh_->DrawSubset(i);
					if(!hr) return hr;
				}
			}

			// If necessary, draw parts that HW could not handle using SW
			if(attr_split_ < attr_num_) {
				attr_id_prev = UNUSED32;
				dev->SetSoftwareVertexProcessing(TRUE);
				for(ulong i = attr_split_; i < attr_num_; i++) {
					if(mtrlno != -1 && bonecb[i].AttribId != static_cast<uint>(mtrlno)) break;
					ulong blend_num = 0;
					for(ulong j = 0; j < max_face_infl_; j++) {
						if(bonecb[i].BoneId[j] != UINT_MAX) blend_num = j;
					}

					if(caps.MaxVertexBlendMatrices < blend_num + 1) {
						for(ulong j = 0; j < max_face_infl_; j++) {
							ulong matid = bonecb[i].BoneId[j];
							if(matid != UINT_MAX) {
								dev->SetTransform(D3DTS_WORLDMATRIX(j), tree.get(owner_.bonename(matid))->val.wtm());
								dev->MultiplyTransform(D3DTS_WORLDMATRIX(j), *owner_.bone(matid));
							}
						}

						dev->SetRenderState(D3DRS_VERTEXBLEND, blend_num);

						if((attr_id_prev != bonecb[i].AttribId) || (attr_id_prev == UNUSED32)) {
							device().setMaterial(owner_.mtrls[bonecb[i].AttribId]);
							attr_id_prev = bonecb[i].AttribId;
						}

						hr = mesh_->DrawSubset(i);
						if(!hr) return hr;
					}
				}
				dev->SetSoftwareVertexProcessing(FALSE);
			}
			dev->SetRenderState(D3DRS_VERTEXBLEND, 0);
			return hr;
		}

		ulong					max_face_infl_;
		ulong					attr_split_;
		ulong					attr_num_;
		ID3DXBufferPtr			bonecb_; // �{�[���̑g�ݍ��킹�f�[�^
		ID3DXMeshPtr			mesh_;
	};

	/// �C���f�b�N�X�����_�u�����h�X�L�����f���N���X
	class IndexedSkinModel : public ModelDetail {
	public:
		IndexedSkinModel(Model &owner) : ModelDetail(owner), is_use_sw_(false), pal_size_(0), max_face_infl_(0), attr_num_(0)
		{
		}

		HRslt setUp()
		{
			HRslt hr;
			mesh_ = 0; bonecb_ = 0; pal_size_ = 0; attr_num_ = 0; max_face_infl_ = 0;
			
			IDirect3DDevicePtr dev;
			hr = owner_.mesh()->GetDevice(&dev);
			if(!hr) return hr;

			// Make sure we get HW caps
			hr = dev->SetSoftwareVertexProcessing(FALSE);

			dx::D3DCAPS caps;
			hr = dev->GetDeviceCaps(&caps);
			if(!hr) return hr;
			
			IDirect3DIndexBufferPtr ib;
			hr = owner_.mesh()->GetIndexBuffer(&ib);
			if(!hr) return hr;
			DWORD _max_face_infl;
			hr = owner_.skin()->GetMaxFaceInfluences(ib, owner_.mesh()->GetNumFaces(), &_max_face_infl);

			// 12 entry palette guarantees that any triangle (4 independent influences per vertex of a tri)
			// can be handled
			_max_face_infl = (min)((long)_max_face_infl, (long)12);

			DWORD flags = D3DXMESHOPT_VERTEXCACHE;
			if(caps.MaxVertexBlendMatrixIndex + 1 < _max_face_infl) {
				// HW does not support indexed vertex_ blending. Use SW instead
				pal_size_ = (min)((long)256, (long)owner_.boneNum());
				is_use_sw_ = true;
				flags |= D3DXMESH_SYSTEMMEM;
			}
			else {
				pal_size_ = (min)((long)((caps.MaxVertexBlendMatrixIndex+1)/2), (long)owner_.boneNum());
				is_use_sw_ = false;
				flags |= D3DXMESH_MANAGED;
			}

			hr = owner_.skin()->ConvertToIndexedBlendedMesh(
				owner_.mesh(),
				flags,
				pal_size_,
				owner_.adjacency(),
				NULL,//pAdjacencyOut
				NULL,//pFaceRemap
				NULL,//ppVertexRemap
				&max_face_infl_,
				&attr_num_,
				&bonecb_,
				&mesh_);
			if(!hr) return hr;
			return hr;
		}

		/// �`��
		HRslt draw(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/) const
		{
			return drawLow(tree, -1);
		}
		/// �`��
		HRslt draw(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/, int mtrlno) const
		{
			return drawLow(tree, mtrlno);
		}

	protected:
		/// �`��
		HRslt drawLow(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/, int mtrlno) const
		{
			HRslt hr;
			if(mesh_) {
				IDirect3DDevicePtr dev;
				hr = mesh_->GetDevice(&dev);
				if(!hr) return hr;

				if(is_use_sw_) dev->SetSoftwareVertexProcessing(TRUE);
				else dev->SetSoftwareVertexProcessing(FALSE);
				if(max_face_infl_ == 1) dev->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS);
				else dev->SetRenderState(D3DRS_VERTEXBLEND, max_face_infl_-1);
				if(max_face_infl_) dev->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);
				D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
				dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
				dev->SetVertexShader(NULL);
				for(uint i = 0; i < attr_num_; i++) {
					if(mtrlno != -1 && bonecb[i].AttribId != static_cast<uint>(mtrlno)) break;
					for(uint j = 0; j < pal_size_; j++) {
						uint matid = bonecb[i].BoneId[j];
						if(matid != UINT_MAX) {
							Pointer<Skeleton::NodeType> node = tree.get(owner_.bonename(matid));
							if(node) {
								dev->SetTransform(D3DTS_WORLDMATRIX(j), node->val.wtm());
								dev->MultiplyTransform(D3DTS_WORLDMATRIX(j), *owner_.bone(matid));
							}
						}
					}
					device().setMaterial(owner_.mtrls[bonecb[i].AttribId]);
					hr = mesh_->DrawSubset(i);
					if(!hr) PRNN(hr);
				}
				dev->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
				dev->SetRenderState(D3DRS_VERTEXBLEND, 0);
				if(is_use_sw_) dev->SetSoftwareVertexProcessing(FALSE);
			}
			return hr;
		}

		bool					is_use_sw_;
		ulong					pal_size_;
		ulong					max_face_infl_;
		ulong					attr_num_;
		ID3DXBufferPtr			bonecb_; // �{�[���̑g�ݍ��킹�f�[�^
		ID3DXMeshPtr			mesh_;
	};

	namespace {

		// Infl 1
		const char asm_shaders_1[] =
			"vs.1.1\n"
			";------------------------------------------------------------------------------\n"
			"; v0 = position\n"
			"; v1 = blend weights\n"
			"; v2 = blend indices\n"
			"; v3 = normal\n"
			"; v4 = texture coordinates\n"
			";------------------------------------------------------------------------------\n"
			";------------------------------------------------------------------------------\n"
			"; r0.w = Last blend weight\n"
			"; r1 = Blend indices\n"
			"; r2 = Temp position\n"
			"; r3 = Temp normal\n"
			"; r4 = Blended position in camera space\n"
			"; r5 = Blended normal in camera space\n"
			";------------------------------------------------------------------------------\n"
			";------------------------------------------------------------------------------\n"
			"; Constants specified by the app;\n"
			";\n"
			"; c9-c95 = world-view matrix palette\n"
			"; c8	  = diffuse * light.diffuse\n"
			"; c7	  = ambient color\n"
			"; c2-c5   = projection matrix\n"
			"; c1	  = light direction\n"
			"; c0	  = {1, power, 0, 1020.01};\n"
			";------------------------------------------------------------------------------\n"
			";------------------------------------------------------------------------------\n"
			"; oPos	  = Output position\n"
			"; oD0	  = Diffuse\n"
			"; oD1	  = Specular\n"
			"; oT0	  = texture coordinates\n"
			";------------------------------------------------------------------------------\n"
			"dcl_position v0;\n"
			"dcl_blendweight v1;\n"
			"dcl_blendindices v2;\n"
			"dcl_normal v3;\n"
			"dcl_texcoord0 v4;\n"
			"// Compensate for lack of UBYTE4 on Geforce3\n"
			"mul r1,v2.zyxw,c0.wwww\n"
			"//mul r1,v2,c0.wwww\n"
			"//Set 1\n"
			"mov a0.x,r1.x\n"
			"m4x3 r4.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r5.xyz,v3,c[a0.x + 9]; \n"
			"//compute position\n"
			"mov r4.w,c0.x\n"
			"m4x4 oPos,r4,c2\n"
			"// normalize normals\n"
			"dp3 r5.w, r5, r5;\n"
			"rsq r5.w, r5.w;\n"
			"mul r5, r5, r5.w;\n"
			"; Do the lighting calculation\n"
			"dp3 r1.x, r5, c1      ; normal dot light\n"
			"lit r1, r1\n"
			"mul r0, r1.y, c8      ; Multiply with diffuse\n"
			"add r0, r0, c7        ; Add in ambient\n"
			"min oD0, r0, c0.x     ; clamp if > 1\n"
			"mov oD1, c0.zzzz      ; output specular\n"
			"; Copy texture coordinate\n"
			"mov oT0, v4\n";
		
		// Infl 2
		const char asm_shaders_2[] =
			"vs.1.1\n"
			"dcl_position v0;\n"
			"dcl_blendweight v1;\n"
			"dcl_blendindices v2;\n"
			"dcl_normal v3;\n"
			"dcl_texcoord0 v4;\n"
			"// Compensate for lack of UBYTE4 on Geforce3\n"
			"mul r1,v2.zyxw,c0.wwww\n"
			"//mul r1,v2,c0.wwww\n"
			"//first compute the last blending weight\n"
			"dp3 r0.w,v1.xyz,c0.xzz; \n"
			"add r0.w,-r0.w,c0.x\n"
			"//Set 1\n"
			"mov a0.x,r1.x\n"
			"m4x3 r4.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r5.xyz,v3,c[a0.x + 9];\n"
			"//blend them\n"
			"mul r4.xyz,r4.xyz,v1.xxxx\n"
			"mul r5.xyz,r5.xyz,v1.xxxx\n"
			"//Set 2\n"
			"mov a0.x,r1.y\n"
			"m4x3 r2.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r3.xyz,v3,c[a0.x + 9];\n"
			"//add them in\n"
			"mad r4.xyz,r2.xyz,r0.wwww,r4;\n"
			"mad r5.xyz,r3.xyz,r0.wwww,r5;\n"
			"//compute position\n"
			"mov r4.w,c0.x\n"
			"m4x4 oPos,r4,c2\n"
			"// normalize normals\n"
			"dp3 r5.w, r5, r5;\n"
			"rsq r5.w, r5.w;\n"
			"mul r5, r5, r5.w;\n"
			"; Do the lighting calculation\n"
			"dp3 r1.x, r5, c1      ; normal dot light\n"
			"lit r1, r1\n"
			"mul r0, r1.y, c8      ; Multiply with diffuse\n"
			"add r0, r0, c7        ; Add in ambient\n"
			"min oD0, r0, c0.x     ; clamp if > 1\n"
			"mov oD1, c0.zzzz      ; output specular\n"
			"; Copy texture coordinate\n"
			"mov oT0, v4\n";

		// Infl 3
		const char asm_shaders_3[] =
			"vs.1.1\n"
			"dcl_position v0;\n"
			"dcl_blendweight v1;\n"
			"dcl_blendindices v2;\n"
			"dcl_normal v3;\n"
			"dcl_texcoord0 v4;\n"
			"// Compensate for lack of UBYTE4 on Geforce3\n"
			"mul r1,v2.zyxw,c0.wwww\n"
			"//mul r1,v2,c0.wwww\n"
			"//first compute the last blending weight\n"
			"dp3 r0.w,v1.xyz,c0.xxz; \n"
			"add r0.w,-r0.w,c0.x\n"
			"//Set 1\n"
			"mov a0.x,r1.x\n"
			"m4x3 r4.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r5.xyz,v3,c[a0.x + 9];\n"
			"//blend them\n"
			"mul r4.xyz,r4.xyz,v1.xxxx\n"
			"mul r5.xyz,r5.xyz,v1.xxxx\n"
			"//Set 2\n"
			"mov a0.x,r1.y\n"
			"m4x3 r2.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r3.xyz,v3,c[a0.x + 9];\n"
			"//add them in\n"
			"mad r4.xyz,r2.xyz,v1.yyyy,r4;\n"
			"mad r5.xyz,r3.xyz,v1.yyyy,r5;\n"
			"//Set 3\n"
			"mov a0.x,r1.z\n"
			"m4x3 r2.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r3.xyz,v3,c[a0.x + 9];\n"
			"//add them in\n"
			"mad r4.xyz,r2.xyz,r0.wwww,r4;\n"
			"mad r5.xyz,r3.xyz,r0.wwww,r5;\n"
			"//compute position\n"
			"mov r4.w,c0.x\n"
			"m4x4 oPos,r4,c2\n"
			"// normalize normals\n"
			"dp3 r5.w, r5, r5;\n"
			"rsq r5.w, r5.w;\n"
			"mul r5, r5, r5.w;\n"
			"; Do the lighting calculation\n"
			"dp3 r1.x, r5, c1      ; normal dot light\n"
			"lit r1, r1\n"
			"mul r0, r1.y, c8      ; Multiply with diffuse\n"
			"add r0, r0, c7        ; Add in ambient\n"
			"min oD0, r0, c0.x     ; clamp if > 1\n"
			"mov oD1, c0.zzzz      ; output specular\n"
			"; Copy texture coordinate\n"
			"mov oT0, v4\n";
		
		// Infl 4
		const char asm_shaders_4[] =
			"vs.1.1\n"
			"dcl_position v0;\n"
			"dcl_blendweight v1;\n"
			"dcl_blendindices v2;\n"
			"dcl_normal v3;\n"
			"dcl_texcoord0 v4;\n"
			"// Compensate for lack of UBYTE4 on Geforce3\n"
			"mul r1,v2.zyxw,c0.wwww\n"
			"//mul r1,v2,c0.wwww\n"
			"//first compute the last blending weight\n"
			"dp3 r0.w,v1.xyz,c0.xxx;\n"
			"add r0.w,-r0.w,c0.x\n"
			"//Set 1\n"
			"mov a0.x,r1.x\n"
			"m4x3 r4.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r5.xyz,v3,c[a0.x + 9]; \n"
			"//blend them\n"
			"mul r4.xyz,r4.xyz,v1.xxxx\n"
			"mul r5.xyz,r5.xyz,v1.xxxx\n"
			"//Set 2\n"
			"mov a0.x,r1.y\n"
			"m4x3 r2.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r3.xyz,v3,c[a0.x + 9];\n"
			"//add them in\n"
			"mad r4.xyz,r2.xyz,v1.yyyy,r4;\n"
			"mad r5.xyz,r3.xyz,v1.yyyy,r5;\n"
			"//Set 3\n"
			"mov a0.x,r1.z\n"
			"m4x3 r2.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r3.xyz,v3,c[a0.x + 9];\n"
			"//add them in\n"
			"mad r4.xyz,r2.xyz,v1.zzzz,r4;\n"
			"mad r5.xyz,r3.xyz,v1.zzzz,r5;\n"
			"//Set 4\n"
			"mov a0.x,r1.w\n"
			"m4x3 r2.xyz,v0,c[a0.x + 9];\n"
			"m3x3 r3.xyz,v3,c[a0.x + 9];\n"
			"//add them in\n"
			"mad r4.xyz,r2.xyz,r0.wwww,r4;\n"
			"mad r5.xyz,r3.xyz,r0.wwww,r5;\n"
			"//compute position\n"
			"mov r4.w,c0.x\n"
			"m4x4 oPos,r4,c2\n"
			"// normalize normals\n"
			"dp3 r5.w, r5, r5;\n"
			"rsq r5.w, r5.w;\n"
			"mul r5, r5, r5.w;\n"
			"; Do the lighting calculation\n"
			"dp3 r1.x, r5, c1      ; normal dot light\n"
			"lit r1, r1\n"
			"mul r0, r1.y, c8      ; Multiply with diffuse\n"
			"add r0, r0, c7        ; Add in ambient\n"
			"min oD0, r0, c0.x     ; clamp if > 1\n"
			"mov oD1, c0.zzzz      ; output specular\n"
			"; Copy texture coordinate\n"
			"mov oT0, v4\n";
		
		const char *asm_shaders[] = {
			asm_shaders_1,
			asm_shaders_2,
			asm_shaders_3,
			asm_shaders_4,
		};
		
		const int asm_shader_sizes[] = {
			sizeof(asm_shaders_1),
			sizeof(asm_shaders_2),
			sizeof(asm_shaders_3),
			sizeof(asm_shaders_4),
		};
	}

	class SkinningVertexShader : public Rsrc
	{
	public:
		HRslt setUp()
		{
			HRslt hr;

			// Make sure we get HW caps
			hr = device().impl()->SetSoftwareVertexProcessing(FALSE);

			dx::D3DCAPS caps;
			hr = device().impl()->GetDeviceCaps(&caps);
			if(!hr) return hr;

			static const _TCHAR *fnames[] = {_T("skinmesh1.vsh"), _T("skinmesh2.vsh"), _T("skinmesh3.vsh"), _T("skinmesh4.vsh")};

			ulong flags = D3DUSAGE_SOFTWAREPROCESSING;
			if(caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)) flags = 0;

			for(ulong i = 0; i < 4; i++) {
				ID3DXBufferPtr code;
				// Assemble the vertex shader file
				hr = D3DXAssembleShaderFromFile(fnames[i], NULL, NULL, 0, &code, NULL);
				if(!hr) {
					hr = D3DXAssembleShaderFromResource(NULL, fnames[i], NULL, NULL, 0, &code, NULL);
					if(!hr) {
						hr = D3DXAssembleShader(asm_shaders[i], asm_shader_sizes[i], NULL, NULL, 0, &code, NULL);
						if(!hr) {
							GCTP_TRACE(hr);
							return hr;
						}
					}
				}
				// Create the vertex shader
				hr = device().impl()->CreateVertexShader(reinterpret_cast<DWORD*>(code->GetBufferPointer()), &shader_[i]);
				if(!hr) return hr;
			}

			return hr;
		}

		dx::IDirect3DVertexShaderPtr shader_[4];
	};

	/// ���_�V�F�[�_�[���X�L�����f���N���X
	class VertexShaderSkinModel : public ModelDetail
	{
	public:
		enum { MAX_MATRICIES = 28 };

		VertexShaderSkinModel(Model &owner)
			: ModelDetail(owner), is_use_sw_(false), pal_size_(0), max_face_infl_(0), attr_num_(0)
		{
		}

		HRslt setUp()
		{
			if(!vs_) {
				vs_ = new SkinningVertexShader;
				if(vs_) vs_->setUp();
			}

			HRslt hr;
			mesh_ = 0; bonecb_ = 0; pal_size_ = 0; attr_num_ = 0; max_face_infl_ = 0;
			
			IDirect3DDevicePtr dev;
			hr = owner_.mesh()->GetDevice(&dev);
			if(!hr) return hr;

			// Make sure we get HW caps
			hr = dev->SetSoftwareVertexProcessing(FALSE);

			dx::D3DCAPS caps;
			hr = dev->GetDeviceCaps(&caps);
			if(!hr) return hr;
			
			DWORD flags = D3DXMESHOPT_VERTEXCACHE;
			if(caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)) {
				is_use_sw_ = false;
				flags |= D3DXMESH_MANAGED;
			}
			else {
				is_use_sw_ = true;
				flags |= D3DXMESH_SYSTEMMEM;
			}

			// ���ۂ̃��[�h�ōđ���
			if(is_use_sw_) {
				hr = dev->SetSoftwareVertexProcessing(TRUE);
				if(!hr) return hr;
				hr = dev->GetDeviceCaps(&caps);
				if(!hr) return hr;
			}
			// �c��̒萔�̈�����ׂă{�[���}�g���N�X�Ɋ��蓖�Ă�
//			max_matricies_ = (caps.MaxVertexShaderConst-9)/3;
//			if(max_matricies_ >= caps.MaxVertexShaderConst) return E_FAIL; // ����܈Ӗ��Ȃ��݂���
			
			// Get palette size
			pal_size_ = (min)((ulong)MAX_MATRICIES, (ulong)owner_.boneNum());
//			PRNN("bone num : "<<owner_.boneNum());

			hr = owner_.skin()->ConvertToIndexedBlendedMesh(
				owner_.mesh(),
				flags,
				pal_size_,
				owner_.adjacency(),
				NULL,//pAdjacencyOut
				NULL,//pFaceRemap
				NULL,//ppVertexRemap
				&max_face_infl_,
				&attr_num_,
				&bonecb_,
				&mesh_);
			if(!hr) return hr;

			if((mesh_->GetFVF() & D3DFVF_POSITION_MASK) != D3DFVF_XYZ) max_face_infl_ = ((mesh_->GetFVF() & D3DFVF_POSITION_MASK) - D3DFVF_XYZRHW) / 2;
			else max_face_infl_ = 1;

			// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
			DWORD new_fvf = (mesh_->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
			if(new_fvf != mesh_->GetFVF()) {
				ID3DXMeshPtr temp_mesh;
				hr = mesh_->CloneMeshFVF(mesh_->GetOptions(), new_fvf, dev, &temp_mesh);
				if(hr) mesh_ = temp_mesh;
			}
			
			dx::D3DVERTEXELEMENT pDecl[MAX_FVF_DECL_SIZE];
			dx::D3DVERTEXELEMENT *pDeclCur;
			hr = mesh_->GetDeclaration(pDecl);
			if(!hr) return hr;

			// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
			//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
			//          this is more of a "cast" operation
			pDeclCur = pDecl;
			while(pDeclCur->Stream != 0xff) {
				if((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
					pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
				pDeclCur++;
			}

			hr = mesh_->UpdateSemantics(pDecl);
			if(!hr) return hr;
			return hr;
		}

		/// �`��
		HRslt draw(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/) const
		{
			return drawLow(tree, -1);
		}
		/// �`��
		HRslt draw(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/, int mtrlno) const
		{
			return drawLow(tree, mtrlno);
		}

	protected:
		/// �`��
		HRslt drawLow(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/, int mtrlno) const
		{
			HRslt hr;
			if(mesh_ && vs_) {
				IDirect3DDevicePtr dev;
				hr = mesh_->GetDevice(&dev);
				if(!hr) return hr;

				if(is_use_sw_) dev->SetSoftwareVertexProcessing(TRUE);

				hr = dev->SetVertexShader(vs_->shader_[max_face_infl_-1]);
				if(!hr) return hr;

				// Set Light for vertex shader
				// ���C�g�X�^�b�N��́A�ŏ��̃f�B���N�V���i�����C�g���̗p
				for(uint i = 0; i < lightNum(); i++) {
					DirectionalLight light;
					if(getLight(i, light)) {
						device().impl()->SetVertexShaderConstantF(1, Vector4C(-light.dir, 0), 1);
						break;
					}
				}

				{
					D3DXMATRIXA16 transposed_proj_mat;
					dev->GetTransform(D3DTS_PROJECTION, &transposed_proj_mat);
					D3DXMatrixTranspose(&transposed_proj_mat, &transposed_proj_mat);
					dev->SetVertexShaderConstantF(2, transposed_proj_mat, 4);
				}

				// Use COLOR instead of UBYTE4 since Geforce3 does not support it
				// _const.w should be 3, but due to about hack, mul by 255 and add epsilon
				Vector4 _const = { 1.0f, 0.0f, 0.0f, 765.01f };

				D3DXMATRIXA16 view_mat;
				dev->GetTransform(D3DTS_VIEW, &view_mat);
				D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
				for(ulong i = 0; i < attr_num_; i++) {
					if(mtrlno != -1 && bonecb[i].AttribId != static_cast<uint>(mtrlno)) break;
					for(ulong j = 0; j < pal_size_; j++) {
						DWORD matid = bonecb[i].BoneId[j];
						if(matid != UINT_MAX) {
							Pointer<Skeleton::NodeType> node = tree.get(owner_.bonename(matid));
							if(node) {
								D3DXMATRIXA16 mat;
								D3DXMatrixMultiply(&mat, *owner_.bone(matid), node->val.wtm());
								D3DXMatrixMultiplyTranspose(&mat, &mat, &view_mat);
								dev->SetVertexShaderConstantF(j*3 + 9, mat, 3);
							}
						}
					}

					// Sum of all ambient and emissive contribution
					Color amb_emm = owner_.mtrls[bonecb[i].AttribId].ambient*getAmbient()+owner_.mtrls[bonecb[i].AttribId].emissive;
					dev->SetVertexShaderConstantF(7, amb_emm, 1);
					dev->SetVertexShaderConstantF(8, owner_.mtrls[bonecb[i].AttribId].diffuse, 1);
					_const.y = owner_.mtrls[bonecb[i].AttribId].power;
					dev->SetVertexShaderConstantF(0, _const, 1);

					Pointer<Texture> tex = owner_.mtrls[bonecb[i].AttribId].tex.get();
					if(tex) dev->SetTexture(0, *tex);
					else dev->SetTexture(0, NULL);

					hr = mesh_->DrawSubset(i);
					if(!hr) return hr;
				}

				if(is_use_sw_) dev->SetSoftwareVertexProcessing(FALSE);
			}
			return hr;
		}

		bool					is_use_sw_;
		ulong					pal_size_;
		ulong					max_face_infl_;
		ulong					attr_num_;
		ID3DXBufferPtr			bonecb_; // �{�[���̑g�ݍ��킹�f�[�^
		ID3DXMeshPtr			mesh_;

		static Pointer<SkinningVertexShader> vs_;
	};
	
	Pointer<SkinningVertexShader> VertexShaderSkinModel::vs_;


	/// HLSL���X�L�����f���N���X
	class ShaderSkinModel : public ModelDetail
	{
	public:
		enum { MAX_MATRICIES = 26 }; // ���ۂ͂����Ǝg����񂾂��ǁc
		// asm�^�ɔ�ׂĕs�ւ���

		ShaderSkinModel(Model &owner)
			: ModelDetail(owner), is_use_sw_(false), pal_size_(0), max_face_infl_(0), attr_num_(0)
		{
		}

		HRslt setUp()
		{
			if(owner_.shader()) shader_ = owner_.shader().get();
			else shader_ = graphic::createOnDB<dx::HLSLShader>(_T("skinnedmesh.fx"));

			HRslt hr;
			mesh_ = 0; bonecb_ = 0; pal_size_ = 0; attr_num_ = 0; max_face_infl_ = 0;
			
			IDirect3DDevicePtr dev;
			hr = owner_.mesh()->GetDevice(&dev);
			if(!hr) return hr;

			// Make sure we get HW caps
			hr = dev->SetSoftwareVertexProcessing(FALSE);

			dx::D3DCAPS caps;
			hr = dev->GetDeviceCaps(&caps);
			if(!hr) return hr;

			DWORD flags = D3DXMESHOPT_VERTEXCACHE;
			if(caps.VertexShaderVersion >= D3DVS_VERSION(1, 1)) {
				is_use_sw_ = false;
				flags |= D3DXMESH_MANAGED;
			}
			else {
				is_use_sw_ = true;
				flags |= D3DXMESH_SYSTEMMEM;
			}
			
			// Get palette size
			pal_size_ = (min)((long)MAX_MATRICIES, (long)owner_.boneNum());
//			PRNN("bone num : "<<owner_.boneNum());

			hr = owner_.skin()->ConvertToIndexedBlendedMesh(
				owner_.mesh(),
				flags,
				pal_size_,
				owner_.adjacency(),
				NULL,//pAdjacencyOut
				NULL,//pFaceRemap
				NULL,//ppVertexRemap
				&max_face_infl_,
				&attr_num_,
				&bonecb_,
				&mesh_);
			if(!hr) return hr;

			if((mesh_->GetFVF() & D3DFVF_POSITION_MASK) != D3DFVF_XYZ) max_face_infl_ = ((mesh_->GetFVF() & D3DFVF_POSITION_MASK) - D3DFVF_XYZRHW) / 2;
			else max_face_infl_ = 1;

			// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
			DWORD new_fvf = (mesh_->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
			if(new_fvf != mesh_->GetFVF()) {
				ID3DXMeshPtr temp_mesh;
				hr = mesh_->CloneMeshFVF(mesh_->GetOptions(), new_fvf, dev, &temp_mesh);
				if(hr) mesh_ = temp_mesh;
			}
			
			dx::D3DVERTEXELEMENT pDecl[MAX_FVF_DECL_SIZE];
			dx::D3DVERTEXELEMENT *pDeclCur;
			hr = mesh_->GetDeclaration(pDecl);
			if(!hr) return hr;

			// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
			//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
			//          this is more of a "cast" operation
			pDeclCur = pDecl;
			while(pDeclCur->Stream != 0xff) {
				if((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
					pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
				pDeclCur++;
			}

			/*for(ulong i = 0; i < attr_num_; i++) {
				D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
				for(ulong j = 0; j < pal_size_; j++) {
					DWORD matid = bonecb[i].BoneId[j];
					if(matid != UINT_MAX) {
						const char *name = owner_.bonename(matid);
						if(name) PRNN("bonename "<<i<<","<<j<<","<<matid<<" "<<name);
						else PRNN("bonename "<<i<<","<<j<<","<<matid<<" ???");
					}
				}
			}*/

			hr = mesh_->UpdateSemantics(pDecl);
			if(!hr) return hr;

			bone_matricies_.resize(pal_size_);
			return hr;
		}

		/// �`��
		HRslt draw(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/) const
		{
			return drawLow(tree, -1);
		}
		/// �`��
		HRslt draw(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/, int mtrlno) const
		{
			return drawLow(tree, mtrlno);
		}

	protected:
		/// �`��
		HRslt drawLow(const Skeleton &tree/**< ���[�V�������Z�b�g���ꂽ�X�P���g��*/, int mtrlno) const
		{
			HRslt hr;
			Handle<dx::HLSLShader> shader = shader_;
			if(mesh_ && shader) {
				IDirect3DDevicePtr dev;
				hr = mesh_->GetDevice(&dev);
				if(!hr) return hr;

				if(is_use_sw_) {
					// If hw or pure hw vertex processing is forced, we can't render the
					// mesh, so just exit out.  Typical applications should create
					// a device with appropriate vertex processing capability for this
					// skinning method.
	                //if( g_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING ) return;
					dev->SetSoftwareVertexProcessing(TRUE);
				}

				// Set Light for vertex shader
				// ���C�g�X�^�b�N��́A�ŏ��̃f�B���N�V���i�����C�g���̗p
				for(uint i = 0; i < lightNum(); i++) {
					DirectionalLight light;
					if(getLight(i, light)) {
						hr = (*shader)->SetVector("lhtDir", Vector4C(-light.dir, 0));
						if(!hr) GCTP_TRACE(hr);
						hr = (*shader)->SetVector("lightDiffuse", (D3DXVECTOR4*)&light.diffuse);
						if(!hr) GCTP_TRACE(hr);
						break;
					}
				}

				{
					D3DXMATRIXA16 mat;
					D3DXMATRIXA16 view_proj_mat;
					dev->GetTransform(D3DTS_VIEW, &view_proj_mat);
					dev->GetTransform(D3DTS_PROJECTION, &mat);
					D3DXMatrixMultiply(&view_proj_mat, &view_proj_mat, &mat);
					hr = (*shader)->SetMatrix("mViewProj", &view_proj_mat);
					if(!hr) GCTP_TRACE(hr);
				}
				D3DXBONECOMBINATION *bonecb = reinterpret_cast<D3DXBONECOMBINATION*>(bonecb_->GetBufferPointer());
				D3DXMATRIX *bm_buf = const_cast< D3DXMATRIX * >(&bone_matricies_[0]);

				for(ulong i = 0; i < attr_num_; i++) {
					if(mtrlno != -1 && bonecb[i].AttribId != static_cast<uint>(mtrlno)) break;
					
					if(hr = shader_->begin()) {
						for(ulong j = 0; j < pal_size_; j++)
						{
							DWORD matid = bonecb[i].BoneId[j];
							if(matid != UINT_MAX) {
								Pointer<Skeleton::NodeType> node = tree.get(owner_.bonename(matid));
								if(node) D3DXMatrixMultiply(&bm_buf[j], *owner_.bone(matid), node->val.wtm());
							}
						}
						hr = (*shader)->SetMatrixArray("mWorldMatrixArray", &bone_matricies_[0], pal_size_);
						if(!hr) {
							PRNN(pal_size_);
							GCTP_TRACE(hr);
						}

						// Sum of all ambient and emissive contribution
						Color amb_emm = owner_.mtrls[bonecb[i].AttribId].ambient*getAmbient()+owner_.mtrls[bonecb[i].AttribId].emissive;
						hr = (*shader)->SetVector("MaterialAmbient", (D3DXVECTOR4*)&amb_emm);
						if(!hr) GCTP_TRACE(hr);
						// set material color properties 
						hr = (*shader)->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&owner_.mtrls[bonecb[i].AttribId].diffuse);
						if(!hr) GCTP_TRACE(hr);

						hr = (*shader)->SetInt( "CurNumBones", max_face_infl_-1);
						if(!hr) GCTP_TRACE(hr);

						Pointer<Texture> tex = owner_.mtrls[bonecb[i].AttribId].tex.get();
						if(tex) dev->SetTexture(0, *tex);
						else dev->SetTexture(0, NULL);

						for(uint ipass = 0; ipass < shader_->passnum(); ipass++) {
							hr = shader_->beginPass( ipass );
							if(!hr) GCTP_TRACE(hr);
							hr = mesh_->DrawSubset(i);
							if(!hr) {
								GCTP_TRACE(hr);
								PRNN("i = "<<i<<";"<<"ipass = "<<ipass<<";"<<"passnum = "<<shader_->passnum());
							}
							hr = shader_->endPass();
							if(!hr) GCTP_TRACE(hr);
						}

						hr = shader_->end();
						if(!hr) GCTP_TRACE(hr);
					}
					else GCTP_TRACE(hr);
				}

				if(is_use_sw_) dev->SetSoftwareVertexProcessing(FALSE);
			}
			return hr;
		}

		bool					is_use_sw_;
		ulong					pal_size_;
		ulong					max_face_infl_;
		ulong					attr_num_;
		ID3DXBufferPtr			bonecb_; // �{�[���̑g�ݍ��킹�f�[�^
		ID3DXMeshPtr			mesh_;
		mutable Pointer<Shader>	shader_;
		vector<D3DXMATRIX>		bone_matricies_;
	};

	/** �X�L�j���O����߂�
	 */
	void Model::solidize()
	{
		detail_ = 0;
	}

#define CREATE_DETAIL(_T)	{\
	if(skin_) {\
		if(isSkin()) {\
			_T *w = new _T(*this);\
			if(w) {\
				detail_ = w;\
				return w->setUp();\
			}\
			return E_OUTOFMEMORY;\
		}\
		return D3DERR_INVALIDCALL;\
	}\
	return CO_E_NOTINITIALIZED;\
}

	/** �\�t�g�E�F�A�X�L�j���O���ꂽ���b�V���𐶐�
	 */
	HRslt Model::useSoftware()
	{
		CREATE_DETAIL(SoftwareSkinModel);
	}

	/** ���_�u�����h�X�L���ɕϊ�
	 */
	HRslt Model::useBlended()
	{
		CREATE_DETAIL(BlendedSkinModel);
	}

	/** �C���f�b�N�X�^���_�u�����h�X�L���ɕϊ�
	 */
	HRslt Model::useIndexed()
	{
		CREATE_DETAIL(IndexedSkinModel);
	}

	/** ���_�V�F�[�_�[�ɂ��X�L���ɕϊ�
	 */
	HRslt Model::useVS()
	{
		CREATE_DETAIL(VertexShaderSkinModel);
	}

	/** HLSL�ɂ��X�L���ɕϊ�
	 */
	HRslt Model::useShader()
	{
		CREATE_DETAIL(ShaderSkinModel);
	}

	namespace {

		void setBlend(void *dst, const void *src, Real weight, dx::D3DVERTEXELEMENT decl[MAX_FVF_DECL_SIZE], bool first)
		{
			for(int i = 0; i < MAX_FVF_DECL_SIZE; i++) {
				if(i > 0 && decl[i].Offset == 0) break;
				switch(decl[i].Type) {
				case D3DDECLTYPE_FLOAT1:
					if(first) (*(float *)(((char *)dst)+decl[i].Offset)) = (*(float *)(((char *)dst)+decl[i].Offset))*weight;
					else (*(float *)(((char *)dst)+decl[i].Offset)) += (*(float *)(((char *)dst)+decl[i].Offset))*weight;
					break;
				case D3DDECLTYPE_FLOAT2:
					if(first) (*(Vector2 *)(((char *)dst)+decl[i].Offset)) = (*(Vector2 *)(((char *)dst)+decl[i].Offset))*weight;
					else (*(Vector2 *)(((char *)dst)+decl[i].Offset)) += (*(Vector2 *)(((char *)dst)+decl[i].Offset))*weight;
					break;
				case D3DDECLTYPE_FLOAT3:
					if(first) (*(Vector *)(((char *)dst)+decl[i].Offset)) = (*(Vector *)(((char *)dst)+decl[i].Offset))*weight;
					else (*(Vector *)(((char *)dst)+decl[i].Offset)) += (*(Vector *)(((char *)dst)+decl[i].Offset))*weight;
					break;
				case D3DDECLTYPE_FLOAT4:
					if(first) (*(Vector4 *)(((char *)dst)+decl[i].Offset)) = (*(Vector4 *)(((char *)dst)+decl[i].Offset))*weight;
					else (*(Vector4 *)(((char *)dst)+decl[i].Offset)) += (*(Vector4 *)(((char *)dst)+decl[i].Offset))*weight;
					break;
				case D3DDECLTYPE_D3DCOLOR:
					if(first) (*(Color32 *)(((char *)dst)+decl[i].Offset)) = (*(Color32 *)(((char *)dst)+decl[i].Offset))*weight;
					else (*(Color32 *)(((char *)dst)+decl[i].Offset)) += (*(Color32 *)(((char *)dst)+decl[i].Offset))*weight;
					break;
				case D3DDECLTYPE_UBYTE4:
					if(first) (*(Color32 *)(((char *)dst)+decl[i].Offset)) = (*(Color32 *)(((char *)dst)+decl[i].Offset))*weight;
					else (*(Color32 *)(((char *)dst)+decl[i].Offset)) += (*(Color32 *)(((char *)dst)+decl[i].Offset))*weight;
					break;
				case D3DDECLTYPE_SHORT2:
					break;
				case D3DDECLTYPE_SHORT4:
					break;
				case D3DDECLTYPE_UBYTE4N:
					break;
				case D3DDECLTYPE_SHORT2N:
					break;
				case D3DDECLTYPE_SHORT4N:
					break;
				case D3DDECLTYPE_USHORT2N:
					break;
				case D3DDECLTYPE_USHORT4N:
					break;
				case D3DDECLTYPE_UDEC3:
					break;
				case D3DDECLTYPE_DEC3N:
					break;
				case D3DDECLTYPE_FLOAT16_2:
					break;
				case D3DDECLTYPE_FLOAT16_4:
					break;
				case D3DDECLTYPE_UNUSED:
					break;
				}
			}
		}

	}

	/** �������f���̃u�����h���ʂŒu��������
	 *
	 * ���̃I�u�W�F�N�g����Ƀ��[�t����A�܂�ق��̃��f����
	 * ���̃I�u�W�F�N�g�Ɠ������_���ł���K�v������.
	 */
	void Model::blend(const Model **models, Real *weights, int num)
	{
		MeshVertexLock _dst(mesh_);
		if(_dst) {
			dx::D3DVERTEXELEMENT decl[MAX_FVF_DECL_SIZE];
			D3DXDeclaratorFromFVF(mesh_->GetFVF(), decl);
			int highest = 0; Real highest_weight = Real(0);
			for(int i = 0; i < num; i++) {
				if(weights[i] > highest_weight) {
					highest_weight = weights[i];
					highest = i;
				}
			}
			for(int i = 0; i < num; i++) {
				uint mnum = (std::max)(mesh_->GetNumVertices(), models[i]->mesh_->GetNumVertices());
				if(i == 0) {
					for(uint j = 0; j < mtrls.size(); j++) {
						mtrls[j] = models[i]->mtrls[j]*weights[i];
						//�e�N�X�`�����͍ł����E�F�C�g�̂��̂��̗p
						mtrls[j].blend = models[highest]->mtrls[j].blend;
						mtrls[j].tex = models[highest]->mtrls[j].tex;
						mtrls[j].tex1 = models[highest]->mtrls[j].tex1;
						mtrls[j].tex2 = models[highest]->mtrls[j].tex2;
					}
				}
				else {
					for(uint j = 0; j < mtrls.size(); j++) {
						mtrls[j] += models[i]->mtrls[j]*weights[i];
						//�e�N�X�`�����͍ł����E�F�C�g�̂��̂��̗p
						mtrls[j].blend = models[highest]->mtrls[j].blend;
						mtrls[j].tex = models[highest]->mtrls[j].tex;
						mtrls[j].tex1 = models[highest]->mtrls[j].tex1;
						mtrls[j].tex2 = models[highest]->mtrls[j].tex2;
					}
				}
				MeshVertexLock _src(models[i]->mesh_);
				if(_src) {
					uint vnum = (std::max)(mesh_->GetNumVertices(), models[i]->mesh_->GetNumVertices());
					for(uint j = 0; j < vnum; j++, _dst.step(), _src.step()) {
						setBlend(_dst.get(), _src.get(), weights[i], decl, i == 0);
					}
				}
			}
		}
	}

	///////////////////////
	// WireMesh
	//

	namespace {
		enum {
			FVF_L  = (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1),
			_FORCE_DWORD_ = 0x7FFFFFFF
		};
		struct LVertex {
			D3DXVECTOR3 p; D3DCOLOR color; float tu, tv;
			void set(Vector pos, Color32 col, Point2f uv)
			{
				p = pos; color = col; tu = uv.x; tv = uv.y;
			}
		};

		struct LineIndex {
			ulong start, end;
		};

#if _MSC_VER <= 1400
#pragma warning(push)
#pragma warning(disable:4200)
#endif
		struct Verticies {
			ulong num;
			Vector verticies[];
		};

		struct Indicies {
			ulong num;
			LineIndex indicies[];
		};

		struct XMeshMaterialList {
			DWORD mtrlnum;
			DWORD mtrlnonum;
			DWORD mtrlno[];
		};
#if _MSC_VER <= 1400
#pragma warning(pop)
#endif
	}

	/** ���f������
	 *
	 * ���łɃZ�b�g�A�b�v����Ă�����A���X�g�A
	 */
	HRslt WireMesh::setUp(const void *data, const void *mlist, ID3DXBufferPtr mtrls, ulong mtrl_num)
	{
		HRslt hr;
		if(vbuf_) vbuf_ = 0;
		if(ibuf_) ibuf_ = 0;
		
		const XMeshMaterialList *mtrllist = reinterpret_cast<const XMeshMaterialList *>(mlist);
		const Verticies *vert = reinterpret_cast<const Verticies *>(data);
		const Indicies *idx = reinterpret_cast<const Indicies *>(&vert->verticies[vert->num]);

		vbuf_ = new VertexBuffer;
		if(!vbuf_) return E_OUTOFMEMORY;
		ibuf_ = new IndexBuffer;
		if(!ibuf_) return E_OUTOFMEMORY;
		hr = vbuf_->setUp(VertexBuffer::STATIC, dx::FVF(FVF_L), vert->num);
		if(hr) {
			VertexBuffer::ScopedLock al(*vbuf_);
			//memcpy(al.buf, vert->verticies, vert->num*sizeof(Vector));
			LVertex *vtx = reinterpret_cast<LVertex *>(al.buf);
			D3DXMATERIAL *_mtrls = reinterpret_cast<D3DXMATERIAL*>(mtrls->GetBufferPointer());
			for(uint i = 0; i < vert->num; i++) {
				vtx[i].p = vert->verticies[i];
				if(_mtrls) vtx[i].color = Color32(_mtrls[mtrllist->mtrlno[i]].MatD3D.Diffuse);
				else vtx[i].color = Color32(128, 128, 128);
			}
		}
		hr = ibuf_->setUp(IndexBuffer::STATIC, idx->num*2 > 0xFFFF ? IndexBuffer::LONG : IndexBuffer::SHORT, idx->num*2);
		if(hr) {
			num_ = idx->num;
			IndexBuffer::ScopedLock al(*ibuf_, 0, 0);
			if(0 && mtrl_num > 0) { // �I�~�b�g�����ρA�@�����������C�e�B���O�ł��Ȃ��Ȃ�}�e���A����K�p�ł��Ȃ��݂���
				//�}�e���A�����ɕ��ׂȂ���
				subset_.resize(mtrl_num);
				uint ii = 0;
				for(uint mno = 0; mno < mtrl_num; mno++) {
					subset_[mno].first = ii;
					subset_[mno].second = 0;
					if(idx->num*2 > 0xFFFF) {
						for(uint i = 0; i < idx->num && ii < idx->num; i++) {
							if(mtrllist->mtrlno[i] == mno) {
								al.buf32[2*ii  ] = static_cast<ushort>(idx->indicies[i].start);
								al.buf32[2*ii+1] = static_cast<ushort>(idx->indicies[i].end);
								ii++;
							}
						}
					}
					else {
						for(uint i = 0; i < idx->num && ii < idx->num; i++) {
							if(mtrllist->mtrlno[i] == mno) {
								al.buf16[2*ii  ] = static_cast<ushort>(idx->indicies[i].start);
								al.buf16[2*ii+1] = static_cast<ushort>(idx->indicies[i].end);
								ii++;
							}
						}
					}
					subset_[mno].second = subset_[mno].first-ii;
				}
			}
			else {
				mtrl_num = 0;
				subset_.resize(1);
				subset_[0].first = 0;
				subset_[0].second = num_;
				if(idx->num*2 > 0xFFFF) memcpy(al.buf, idx->indicies, idx->num*2*sizeof(ulong));
				else {
					for(uint i = 0; i < idx->num; i++) {
						al.buf16[2*i  ] = static_cast<ushort>(idx->indicies[i].start);
						al.buf16[2*i+1] = static_cast<ushort>(idx->indicies[i].end);
					}
				}
			}
		}
		return hr;
	}

	/** ���C�����X�g��`��
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/11/21 15:52:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WireMesh::draw() const
	{
		if(ibuf_ && vbuf_) {
			HRslt ret;
			for(uint i = 0; i < subset_.size(); i++) {
				ret = draw(i);
				if(!ret) return ret;
			}
			return ret;
		}
		return E_POINTER;
	}

	/** ���C�����X�g��`��i�T�u�Z�b�g�w��)
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/11/21 15:52:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	HRslt WireMesh::draw(int subset) const
	{
		if(ibuf_ && vbuf_) {
			if(subset_[subset].first) return S_FALSE;
			return ibuf_->draw(*vbuf_, 0, D3DPT_LINELIST, subset_[subset].second, subset_[subset].first);
		}
		return E_POINTER;
	}

	float WireMesh::calcRadius(const Vector &center) const
	{
		if(!vbuf_) return 0;
		float d, ret = 0;
		
		uint vnum = vbuf_->numVerticies();

		VertexBuffer::ScopedLock vbl(*const_cast<Pointer<VertexBuffer>&>(vbuf_));
		if(vbl.buf) {
			for(uint i=0; i < vnum; i++, vbl.step()) {
				Vector *pv = (Vector *)vbl.buf;
				d = distance(*pv, center);
				if(d > ret) ret = d;
			}
		}
		return ret;
	}

	AABox WireMesh::getAABB() const
	{
		AABox ret;
		VertexBuffer::ScopedLock vbl(*const_cast<Pointer<VertexBuffer>&>(vbuf_));
		if(vbl.buf) {
			uint vnum = vbuf_->numVerticies();
			for(uint i = 0; i < vnum; i++, vbl.step()) {
				Vector *pv = (Vector *)vbl.buf;
				if(i == 0) ret.initialize(*pv);
				else ret.add(*pv);
			}
		}
		return ret;
	}

	AABox WireMesh::getAABB(const Matrix &mat) const
	{
		AABox ret;
		VertexBuffer::ScopedLock vbl(*const_cast<Pointer<VertexBuffer>&>(vbuf_));
		if(vbl.buf) {
			Vector vec;
			uint vnum = vbuf_->numVerticies();
			for(uint i = 0; i < vnum; i++, vbl.step()) {
				//Vector *pv = (Vector *)vbl.buf;
				//mat.transformPoint(vec, *pv);
				vec = mat * *(Vector *)vbl.buf;
				if(i == 0) ret.initialize(vec);
				else ret.add(vec);
			}
		}
		return ret;
	}

}} // namespace gctp
