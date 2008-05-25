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
#include <gctp/graphic/brush.hpp>
#include <gctp/graphic/shader.hpp>
#include <gctp/graphic/light.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic/vertexbuffer.hpp>
#include <gctp/graphic/indexbuffer.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/graphic/dx/hlslshader.hpp>
#include <gctp/graphic/dx/skinbrush.hpp>
#include <gctp/dbgout.hpp>

using namespace std;
using gctp::graphic::dx::IDirect3DDevicePtr;
using gctp::graphic::dx::IDirect3DVertexBufferPtr;
using gctp::graphic::dx::IDirect3DIndexBufferPtr;

namespace gctp { namespace graphic {

	// �{����dx/skinbrush.cpp�ɂ���ׂ������c
	Pointer<SkinningVertexShader> VertexShaderSkinBrush::vs_;

	/** ���f������
	 *
	 * ���łɃZ�b�g�A�b�v����Ă�����A���X�g�A
	 */
	void Model::setUp(CStr name, ID3DXMeshPtr mesh, ID3DXSkinInfoPtr skin, ID3DXBufferPtr adjc)
	{
		type_ = TYPE_POLYGON;
		name_ = name;
		mesh_ = mesh;
		skin_ = skin;
		adjc_ = adjc;

		if(mesh_) {
			DWORD size;
			mesh_->GetAttributeTable(NULL, &size);
			subsets_.resize(size);
			std::vector<D3DXATTRIBUTERANGE> attrinfo;
			attrinfo.resize(size);
			mesh_->GetAttributeTable(&attrinfo[0], &size);
			for(DWORD i = 0; i < size; i++) {
				subsets_[i].index_offset = attrinfo[i].FaceStart;
				subsets_[i].primitive_num = attrinfo[i].FaceCount;
				subsets_[i].vertex_offset = attrinfo[i].VertexStart;
				subsets_[i].vertex_num = attrinfo[i].VertexCount;
			}
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

	/** ���C�����f������
	 *
	 * �����܂ŕ\������Model�A�Ƃ����݌v����邽�߂̎b�菈�u�c
	 * �����ƃX�}�[�g�Ȏ�i���l���܂��傤
	 */
	HRslt Model::setUpWire(CStr name, const void *data, const void *mlist, ID3DXBufferPtr mtrls, ulong mtrl_num)
	{
		type_ = TYPE_LINE;
		name_ = name;
		HRslt hr;
		const XMeshMaterialList *mtrllist = reinterpret_cast<const XMeshMaterialList *>(mlist);
		const Verticies *vert = reinterpret_cast<const Verticies *>(data);
		const Indicies *idx = reinterpret_cast<const Indicies *>(&vert->verticies[vert->num]);

		hr = vb_.setUp(VertexBuffer::STATIC, dx::FVF(FVF_L), vert->num);
		if(hr) {
			VertexBuffer::ScopedLock al(vb_);
			//memcpy(al.buf, vert->verticies, vert->num*sizeof(Vector));
			LVertex *vtx = reinterpret_cast<LVertex *>(al.buf);
			D3DXMATERIAL *_mtrls = reinterpret_cast<D3DXMATERIAL*>(mtrls->GetBufferPointer());
			for(uint i = 0; i < vert->num; i++) {
				vtx[i].p = vert->verticies[i];
				if(_mtrls) vtx[i].color = Color32(_mtrls[mtrllist->mtrlno[i]].MatD3D.Diffuse);
				else vtx[i].color = Color32(128, 128, 128);
			}
		}
		hr = ib_.setUp(IndexBuffer::STATIC, idx->num*2 > 0xFFFF ? IndexBuffer::LONG : IndexBuffer::SHORT, idx->num*2);
		if(hr) {
			IndexBuffer::ScopedLock al(ib_, 0, 0);
			if(0 && mtrl_num > 0) { // �I�~�b�g�����ρA�@�����������C�e�B���O�ł��Ȃ��Ȃ�}�e���A����K�p�ł��Ȃ��݂���
				//�}�e���A�����ɕ��ׂȂ���
				subsets_.resize(mtrl_num);
				uint ii = 0;
				for(uint mno = 0; mno < mtrl_num; mno++) {
					subsets_[mno].index_offset = ii;
					subsets_[mno].primitive_num = 0;
					subsets_[mno].vertex_offset = 0;
					subsets_[mno].vertex_num = 0;
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
					subsets_[mno].primitive_num = subsets_[mno].index_offset-ii;
				}
			}
			else {
				mtrl_num = 0;
				subsets_.resize(1);
				subsets_[0].index_offset = 0;
				subsets_[0].primitive_num = idx->num;
				subsets_[0].vertex_offset = 0;
				subsets_[0].vertex_num = 0;
				if(idx->num*2 > 0xFFFF) memcpy(al.buf, idx->indicies, idx->num*2*sizeof(ulong));
				else {
					for(uint i = 0; i < idx->num; i++) {
						al.buf16[2*i  ] = static_cast<ushort>(idx->indicies[i].start);
						al.buf16[2*i+1] = static_cast<ushort>(idx->indicies[i].end);
					}
				}
			}
		}
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
			brush_ = 0;
			return ret;
		}
		return CO_E_NOTINITIALIZED;
	}

	HRslt Model::update(const Model &src, const Matrix &mat)
	{
		if(!mesh_) return CO_E_NOTINITIALIZED;
		if(!src.mesh_) return CO_E_NOTINITIALIZED;
		uint vnum = (max)(mesh_->GetNumVertices(), src.mesh_->GetNumVertices());

		VertexBuffer::ScopedLock _dst(vb_);
		VertexBuffer::ScopedLock _src(src.vb_);
		if(_dst.buf && _src.buf) {
			for(uint i = 0; i < vnum; i++, _dst.step(), _src.step()) {
				*(Vector *)_dst.buf = mat * *(Vector *)_src.buf;
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
		if(!vb_) return 0;
		float d, ret = 0;
		VertexBuffer::ScopedLock vbl(vb_);
		if(vbl.buf) {
			uint vnum = vb_.numVerticies();
			for(uint i=0; i < vnum; i++, vbl.step()) {
				Vector *pv = (Vector *)vbl.buf;
				d = distance(*pv, center);
				if(d > ret) ret = d;
			}
		}
		return ret;
	}

	AABox Model::getAABB() const
	{
		if(!vb_) return AABox(VectorC(0, 0, 0));
		AABox ret;
		VertexBuffer::ScopedLock vbl(vb_);
		if(vbl.buf) {
			uint vnum = vb_.numVerticies();
			for(uint i = 0; i < vnum; i++, vbl.step()) {
				Vector *pv = (Vector *)vbl.buf;
				if(i == 0) ret.initialize(*pv);
				else ret.add(*pv);
			}
		}
		return ret;
	}

	AABox Model::getAABB(const Matrix &mat) const
	{
		if(!vb_) return AABox(VectorC(0, 0, 0));
		AABox ret;
		VertexBuffer::ScopedLock vbl(vb_);
		if(vbl.buf) {
			Vector vec;
			uint vnum = vb_.numVerticies();
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
		VertexBuffer::ScopedLock vbl(with.vb_);
		IndexBuffer::ScopedLock ibl(with.ib_);
		RayLine ray;
		ulong idx;
		float u, v, dist, sdist;
		for(uint i = 0; i < with.mesh_->GetNumFaces(); i++) {
			for(int j = 0; j < 6; j++) {
				switch(j) {
				case 0: ray.s = vbl.get<Vector>(ibl.get<ushort>(i*3  )); ray.v = vbl.get<Vector>(ibl.get<ushort>(i*3+1)) - vbl.get<Vector>(ibl.get<ushort>(i*3  )); break;
				case 1: ray.s = vbl.get<Vector>(ibl.get<ushort>(i*3+1)); ray.v = vbl.get<Vector>(ibl.get<ushort>(i*3+2)) - vbl.get<Vector>(ibl.get<ushort>(i*3+1)); break;
				case 2: ray.s = vbl.get<Vector>(ibl.get<ushort>(i*3+2)); ray.v = vbl.get<Vector>(ibl.get<ushort>(i*3  )) - vbl.get<Vector>(ibl.get<ushort>(i*3+2)); break;
				case 3: ray.s = vbl.get<Vector>(ibl.get<ushort>(i*3  )); ray.v = vbl.get<Vector>(ibl.get<ushort>(i*3+2)) - vbl.get<Vector>(ibl.get<ushort>(i*3  )); break;
				case 4: ray.s = vbl.get<Vector>(ibl.get<ushort>(i*3+1)); ray.v = vbl.get<Vector>(ibl.get<ushort>(i*3  )) - vbl.get<Vector>(ibl.get<ushort>(i*3+1)); break;
				case 5: ray.s = vbl.get<Vector>(ibl.get<ushort>(i*3+2)); ray.v = vbl.get<Vector>(ibl.get<ushort>(i*3+1)) - vbl.get<Vector>(ibl.get<ushort>(i*3+2)); break;
				}
				sdist = ray.v.length();
				ray.v.normalize();
				if(hasIntersected(ray, idx, u, v, dist)) {
					if(dist < sdist) {
						VertexBuffer::ScopedLock myvbl(vb_);
						IndexBuffer::ScopedLock myibl(ib_);
						mytouch = myvbl.get<Vector>(myibl.get<ushort>(idx*3));
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
		if(type_ == TYPE_LINE) {
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
							if(ib_ && vb_) {
								//if(subsets_[i].first) return S_FALSE; //??
								hr = ib_.draw(vb_, 0, D3DPT_LINELIST, subsets_[i].primitive_num, subsets_[i].index_offset);
								if(!hr) GCTP_TRACE(hr);
							}
							//hr = wire_->draw(i);
							//if(!hr) GCTP_TRACE(hr);
							hr = shader->endPass();
							if(!hr) GCTP_TRACE(hr);
						}
						hr = shader->end();
						if(!hr) GCTP_TRACE(hr);
					}
					else {
						if(ib_ && vb_) {
							//if(subsets_[i].first) return S_FALSE; //??
							hr = ib_.draw(vb_, 0, D3DPT_LINELIST, subsets_[i].primitive_num, subsets_[i].index_offset);
							if(!hr) GCTP_TRACE(hr);
						}
					}
					if(!hr) return hr;
				}
			}
			else {
				Material mtrl;
				mtrl.setUp();
				device().setMaterial(mtrl);
				//hr = wire_->draw();
				if(ib_ && vb_) {
					//if(subsets_[i].first) return S_FALSE; //??
					for(uint i = 0; i < subsets_.size(); i++) {
						hr = ib_.draw(vb_, 0, D3DPT_LINELIST, subsets_[i].primitive_num, subsets_[i].index_offset);
					}
					if(!hr) GCTP_TRACE(hr);
				}
				if(!hr) return hr;
			}
		}
		else if(mesh_) {
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
					hr = dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, subsets_[mtrlno].vertex_offset, 0, subsets_[mtrlno].vertex_num, subsets_[mtrlno].index_offset, subsets_[mtrlno].primitive_num);
					if(!hr) GCTP_TRACE(hr);
					hr = shader->endPass();
					if(!hr) GCTP_TRACE(hr);
				}
			}
			else hr = dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, subsets_[mtrlno].vertex_offset, 0, subsets_[mtrlno].vertex_num, subsets_[mtrlno].index_offset, subsets_[mtrlno].primitive_num);

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
		if(brush_.get()) return brush_->draw(skl);
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
		if(brush_) return brush_->draw(skl, mtrlno);
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
		// ��X����ł͖�肠��
		if(brush_) return true;
		return false;
	}

	/** �X�L�j���O����߂�
	 */
	void Model::solidize()
	{
		brush_ = 0;
	}

#define CREATE_BRUSH(_T)	{\
	if(skin_) {\
		if(isSkin()) {\
			_T *w = new _T(*this);\
			if(w) {\
				brush_ = w;\
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
		CREATE_BRUSH(SoftwareSkinBrush);
	}

	/** ���_�u�����h�X�L���ɕϊ�
	 */
	HRslt Model::useBlended()
	{
		CREATE_BRUSH(BlendedSkinBrush);
	}

	/** �C���f�b�N�X�^���_�u�����h�X�L���ɕϊ�
	 */
	HRslt Model::useIndexed()
	{
		CREATE_BRUSH(IndexedSkinBrush);
	}

	/** ���_�V�F�[�_�[�ɂ��X�L���ɕϊ�
	 */
	HRslt Model::useVS()
	{
		CREATE_BRUSH(VertexShaderSkinBrush);
	}

	/** HLSL�ɂ��X�L���ɕϊ�
	 */
	HRslt Model::useShader()
	{
		CREATE_BRUSH(ShaderSkinBrush);
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
		VertexBuffer::ScopedLock _dst(vb_);
		if(_dst.buf) {
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
				VertexBuffer::ScopedLock _src(models[i]->vb_);
				if(_src.buf) {
					uint vnum = (std::max)(mesh_->GetNumVertices(), models[i]->mesh_->GetNumVertices());
					for(uint j = 0; j < vnum; j++, _dst.step(), _src.step()) {
						setBlend(_dst.buf, _src.buf, weights[i], decl, i == 0);
					}
				}
			}
		}
	}

}} // namespace gctp
