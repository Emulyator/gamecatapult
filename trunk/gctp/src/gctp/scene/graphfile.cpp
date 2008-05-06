/** @file
 * GameCatapult �V�[���O���t�R���e���g�t�@�C���N���X
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/04 22:56:37
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/pointer.hpp>
#include <gctp/vector.hpp>
#include <gctp/xfile.hpp>
#include <gctp/graphic/model.hpp>
#include <gctp/graphic/brush.hpp>
#include <gctp/graphic/texture.hpp>
#include <gctp/graphic/dx/device.hpp>
#include <gctp/scene/graphfile.hpp>
#include <gctp/scene/flesh.hpp>
#include <gctp/scene/body.hpp>
#include <gctp/scene/motion.hpp>
#include <gctp/extension.hpp>
#include <gctp/context.hpp>
#include <gctp/buffer.hpp>
#include <gctp/dbgout.hpp>
#include <gctp/turi.hpp>
#include <rmxfguid.h>

using namespace std;

template<class E, class T>
std::basic_ostream<E, T> &operator<<(std::basic_ostream<E, T> &os, const GUID &g)
{
	char w[128];
	sprintf(w, "<%04X-%02X-%02x-%01X%01X-%01X%01X%01X%01X%01X%01X>", g.Data1, g.Data2, g.Data3, g.Data4[0], g.Data4[1],
		g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
	return os << w;
}

bool operator < (const GUID &lhs, const GUID &rhs)
{
	return memcmp(&lhs, &rhs, sizeof(GUID)) < 0;
}

namespace gctp { namespace scene {

	///////////////////
	// XFileHandler
	//

	/** �e���v���[�g���̓ǂݍ��݃C�x���g�n���h���[��o�^
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:19:25
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	XFileHandler::XFileHandler(const GUID &type, const XFileHandleMethod f)
	{
		handlers()[type] = f;
		me_ = handlers().find(type);
	}

	/** �o�^�𖕏�
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:19:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	XFileHandler::~XFileHandler()
	{
		handlers().erase(me_);
	}

	/** �`�����N�ɑΉ������n���h�������邩�H
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:19:37
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	bool XFileHandler::hasHandler(const GUID &type)
	{
		HandlerMap::iterator i = handlers().find(type);
		return i != handlers().end();
	}

	/** �n���h�����擾
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/08 11:19:44
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	XFileHandleMethod XFileHandler::get(const GUID &type)
	{
		HandlerMap::iterator i = handlers().find(type);
		if(i != handlers().end()) return i->second;
		return NULL;
	}

	/// HandlerMap�̃V���O���g����Ԃ�
	XFileHandler::HandlerMap &XFileHandler::handlers()
	{
		static HandlerMap _handlers_;
		return _handlers_;
	}

	namespace {
		GCTP_REGISTER_REALIZER2(x, GraphFile, &GraphFile::setUpFromX);
		//Extention extention_x("x", Realizer<XFile>);

		/** ���f���̃}�e���A�����Z�b�g�A�b�v
		 */
		void setUpModelMaterial(graphic::Model &model, ID3DXBufferPtr mtrls, ulong mtrl_num, ID3DXBufferPtr eff)
		{
			// �}�e���A���ݒ�
			if(mtrls) {
				D3DXMATERIAL *_mtrls = reinterpret_cast<D3DXMATERIAL*>(mtrls->GetBufferPointer());
				model.mtrls.resize(mtrl_num);
				for(uint i = 0; i < mtrl_num; i++) {
					model.mtrls[i].diffuse  = _mtrls[i].MatD3D.Diffuse;
					model.mtrls[i].ambient  = _mtrls[i].MatD3D.Ambient;
					model.mtrls[i].specular = _mtrls[i].MatD3D.Specular;
					model.mtrls[i].emissive = _mtrls[i].MatD3D.Emissive;
					model.mtrls[i].power    = _mtrls[i].MatD3D.Power;
					model.mtrls[i].blend    = graphic::Material::ALPHA;
					if(_mtrls[i].pTextureFilename) {
#ifdef UNICODE
						WCStr fname = _mtrls[i].pTextureFilename;
						context().load(fname.c_str());
						model.mtrls[i].tex = context()[fname.c_str()];
#else
						context().load(_mtrls[i].pTextureFilename);
						model.mtrls[i].tex = context()[_mtrls[i].pTextureFilename];
#endif
					}
				}
				if(eff) {
					D3DXEFFECTINSTANCE *effect = reinterpret_cast<D3DXEFFECTINSTANCE*>(eff->GetBufferPointer());
					if(effect->pEffectFilename) {
						PRNN("Effect "<<effect->pEffectFilename);
#ifdef UNICODE
						WCStr fname = effect->pEffectFilename;
						context().load(fname.c_str());
						Handle<graphic::Brush> brush = context()[fname.c_str()];
#else
						context().load(effect->pEffectFilename);
						Handle<graphic::Brush> brush = context()[effect->pEffectFilename];
#endif
						if(brush) {
							model.setBrush(brush);
							for(DWORD i = 0; i < effect->NumDefaults; i++) {
								(*brush)->SetValue(effect->pDefaults[i].pParamName, effect->pDefaults[i].pValue, effect->pDefaults[i].NumBytes);
							}
						}
					}
					else {
						for(DWORD i = 0; i < effect->NumDefaults; i++) {
							switch(effect->pDefaults[i].Type) {
							case D3DXEDT_STRING:
								PRNN(i<<")STRING "<<effect->pDefaults[i].pParamName<<" "<<(const char *)effect->pDefaults[i].pValue);
								break;
							case D3DXEDT_FLOATS:
								PRN(i<<")FLOATS "<<effect->pDefaults[i].pParamName<<" ");
								for(DWORD j = 0; j < effect->pDefaults[i].NumBytes/sizeof(float); j++) {
									if(j > 0) PRN(", ");
									PRN(((float *)effect->pDefaults[i].pValue)[j]);
								}
								PRN(endl);
								break;
							case D3DXEDT_DWORD:
								PRN(i<<")DWORD "<<effect->pDefaults[i].pParamName<<" ");
								for(DWORD j = 0; j < effect->pDefaults[i].NumBytes/sizeof(DWORD); j++) {
									if(j > 0) PRN(", ");
									PRN(((DWORD *)effect->pDefaults[i].pValue)[j]);
								}
								PRN(endl);
								break;
							}
						}
					}
				}
			}
		}

		/** ���f������
		 *
		 * ���łɃZ�b�g�A�b�v����Ă�����A���X�g�A
		 */
		HRslt setUpModel(graphic::Model &model, const XData &data)
		{
			ID3DXMeshPtr mesh;
			ID3DXSkinInfoPtr skin;
			ID3DXBufferPtr adjc, effect, mtrls;
			ulong mtrl_num;
			// �w�肵���`�����N����X�L���i�����m��Ȃ��j���f�������[�h����B
			HRslt hr = D3DXLoadSkinMeshFromXof(data, D3DXMESH_SYSTEMMEM, graphic::device().impl(), &adjc, &mtrls, &effect, &mtrl_num, &skin, &mesh);
			if(!hr) return hr;
			setUpModelMaterial(model, mtrls, mtrl_num, effect);
			model.setUp(data.name(), mesh, skin, adjc);

			{
	//			ID3DXBufferPtr message;
	//			HRslt hr = D3DXValidMesh(mesh_, adjacency(), &message);
	//			if(!hr) PRNN(hr << " : " << reinterpret_cast<char *>(message->GetBufferPointer()));
#if 0
				MeshVertexLock vbl(mesh_);
				MeshIndexLock ibl(mesh_);
				for(uint i = 0; i < mesh_->GetNumFaces(); i++) {
					if(i == 1824 || i == 1755) {
						PRNN("invalid face ("<<i<<")");
						PRNN("index : "<<ibl[i*3]<<","<<ibl[i*3+1]<<","<<ibl[i*3+2]);
						PRNN("vertex : "<<vbl[i*3]<<","<<vbl[i*3+1]<<","<<vbl[i*3+2]);
						PRNN("adjacency : "<<adjacency()[i*3]<<","<<adjacency()[i*3+1]<<","<<adjacency()[i*3+2]);
					}
				}
#endif
			}
			return hr;
		}

		/** ���C�����f������
		 *
		 */
		HRslt setUpWireModel(graphic::Model &model, const XData &data)
		{
			ID3DXBufferPtr mtrls;
			ulong mtrl_num = 0;
			const gctp::XMeshMaterialList *mtrllist = 0;
			// �}�e���A����񂩂��W��
			for(uint i = 0; i < data.size(); i++) {
				XData child = data.getChild(i);
				if(child.type() == TID_D3DRMMeshMaterialList) {
					mtrllist = reinterpret_cast<const gctp::XMeshMaterialList *>(child.data());
					mtrl_num = mtrllist->mtrlnum;
					D3DXCreateBuffer(sizeof(D3DXMATERIAL)*mtrllist->mtrlnum, &mtrls);
					D3DXMATERIAL *_mtrls = reinterpret_cast<D3DXMATERIAL*>(mtrls->GetBufferPointer());
					int mn = 0;
					for(uint i = 0; i < child.size(); i++) {
						XData mlchild = child.getChild(i);
						if(mlchild.type() == TID_D3DRMMaterial) {
							const XMaterial *xmtrl = reinterpret_cast<const XMaterial *>(mlchild.data());
							_mtrls[mn].MatD3D.Diffuse = xmtrl->face_color;
							_mtrls[mn].MatD3D.Ambient = Color(0, 0, 0);
							_mtrls[mn].MatD3D.Power   = xmtrl->power;
							_mtrls[mn].MatD3D.Specular.a = 1.0f;
							_mtrls[mn].MatD3D.Specular.r = xmtrl->specular_r;
							_mtrls[mn].MatD3D.Specular.g = xmtrl->specular_g;
							_mtrls[mn].MatD3D.Specular.b = xmtrl->specular_b;
							_mtrls[mn].MatD3D.Emissive.a = 1.0f;
							_mtrls[mn].MatD3D.Emissive.r = xmtrl->emissive_r;
							_mtrls[mn].MatD3D.Emissive.g = xmtrl->emissive_g;
							_mtrls[mn].MatD3D.Emissive.b = xmtrl->emissive_b;
							_mtrls[mn].pTextureFilename  = 0;
							for(uint i = 0; i < mlchild.size(); i++) {
								XData mchild = mlchild.getChild(i);
								if(mchild.type() == TID_D3DRMTextureFilename) {
									_mtrls[mn].pTextureFilename = const_cast<LPSTR>(reinterpret_cast<LPCSTR>(mchild.data()));
									break;
								}
							}
							mn++;
						}
					}
				}
			}
			setUpModelMaterial(model, mtrls, mtrl_num, ID3DXBufferPtr());
			return model.setUpWire(data.name(), data.data(), mtrllist, mtrls, mtrl_num);
		}

		/** XFile����̓ǂ݂���
		 */
		HRslt setUpMotionChannel(MotionChannel &self, const XData &dat/**< �J�����g�̈ʒu������XData�I�u�W�F�N�g*/)
		{
			HRslt hr;
			const XKeys *keys = dat.keys();
			if(keys) {
				if(keys->type == XKEY_SCALING) {
					const XScalingKeys *anim_keys = dat.sclkeys();
					MotionChannel::ScalingKeys *w = new MotionChannel::ScalingKeys(MotionChannel::Keys::SCALING, anim_keys->num);
					//�����R�s�[
					for(uint i = 0; i < anim_keys->num; i++) {
						(*w)[i].time = anim_keys->keys[i].time;
						(*w)[i].val = VectorC(anim_keys->keys[i].val);
					}
					self.setKeys(w);
					//PRNN("ScalingKey read");
				}
				else if(keys->type == XKEY_POSTURE) {
					const XPostureKeys *anim_keys = dat.posturekeys();
					MotionChannel::PostureKeys *w = new MotionChannel::PostureKeys(MotionChannel::Keys::POSTURE, anim_keys->num);
					//�����R�s�[
					for(uint i = 0; i < anim_keys->num; i++) {
						(*w)[i].time = anim_keys->keys[i].time;
						(*w)[i].val.w = anim_keys->keys[i].val.w;
						// �Ȃ����]���K�v�Ȃ񂾁H
						(*w)[i].val.x = -anim_keys->keys[i].val.x;
						(*w)[i].val.y = -anim_keys->keys[i].val.y;
						(*w)[i].val.z = -anim_keys->keys[i].val.z;
					}
					self.setKeys(w);
					//PRNN("PostureKey read");
				}
				else if(keys->type == XKEY_YPR) {
					const XYPRKeys *anim_keys = dat.yprkeys();
					MotionChannel::YPRKeys *w = new MotionChannel::YPRKeys(MotionChannel::Keys::YPR, anim_keys->num);
					//�����R�s�[
					for(uint i = 0; i < anim_keys->num; i++) {
						(*w)[i].time = anim_keys->keys[i].time;
						(*w)[i].val = VectorC(anim_keys->keys[i].val);
					}
					self.setKeys(w);
					//PRNN("YPRKey read");
				}
				else if(keys->type == XKEY_POSITION) {
					const XPositionKeys *anim_keys = dat.positionkeys();
					MotionChannel::PositionKeys *w = new MotionChannel::PositionKeys(MotionChannel::Keys::POSITION, anim_keys->num);
					//�����R�s�[
					for(uint i = 0; i < anim_keys->num; i++) {
						(*w)[i].time = anim_keys->keys[i].time;
						(*w)[i].val = VectorC(anim_keys->keys[i].val);
					}
					self.setKeys(w);
					//PRNN("PositionKey read");
				}
				else if(keys->type == XKEY_MATRIX) {
					const XMatrixKeys *anim_keys = dat.matkeys();
					MotionChannel::MatrixKeys *w = new MotionChannel::MatrixKeys(MotionChannel::Keys::MATRIX, anim_keys->num);
					//�����R�s�[
					for(uint i = 0; i < anim_keys->num; i++) {
						(*w)[i].time = anim_keys->keys[i].time;
						(*w)[i].val = MatrixC(anim_keys->keys[i].val);
					}
					self.setKeys(w);
					//PRNN("MatrixKey read");
				}
				else {
					PRNN("Motion:"<<keys->type<<":�Ή����Ă��Ȃ��L�[�^�C�v�ł��B");
					hr = E_FAIL;
				}
			}
			else hr = E_FAIL;
			return hr;
		}

		HRslt setUpMotion(
			Motion &self,
			const XData &cur/**< �J�����g�̈ʒu������XData�I�u�W�F�N�g*/
		)
		{
			HRslt hr;
			for(uint i = 0; i < cur.size(); i++) {
				XData setdat = cur[i];
				if(TID_D3DRMAnimation == setdat.type()) {
					bool is_open = true;
					MotionChannel::PosType postype = MotionChannel::LINEAR;
					CStr framename;
					MotionChannelVector channels;
					//PRNN("chunk name : "<<cur.name());
					for(uint i = 0; i < setdat.size(); i++) {
						XData dat = setdat[i];
						if(!dat.isRef()) {
							if( TID_D3DRMFrame == dat.type() ) {
								// �Q�ƂłȂ��A���ڃt���[�����i�[����Ă��邱�Ƃ�����悤���c�c
								// �Ή����Ȃ��Ⴞ�߂��c�c
								/*hr = LoadFrames(pxofobjChild, pde, options, fvf, graphic(), pframeCur);
								if (FAILED(hr))
									goto e_Exit;*/
								PRNN("Motion::�t���[���̎w��͎Q�ƈȊO�Ή����Ă��܂���B");
							}
							else if( TID_D3DRMAnimationOptions == dat.type() ) {
								const XAnimOption *opt = dat.animoption();
								if(opt) {
									is_open = (opt->openclosed==1)? true : false;
									postype = (opt->positionquality == 1)? MotionChannel::SPLINE : MotionChannel::LINEAR;
								}
							}
							else if( TID_D3DRMAnimationKey == dat.type() ) {
								MotionChannel *channel = new MotionChannel;
								if(channel) {
									hr = setUpMotionChannel(*channel, dat);
									if(!hr) delete channel;
									else channels.push_back(Pointer<MotionChannel>(channel));
								}
								else hr = E_FAIL;
							}
						}
						else {
							if(TID_D3DRMFrame == dat.type()) {
								framename = dat.name();
							}
							else {
								PRNN("AnimationRsrc::�Ή����Ă��Ȃ��Q�ƃf�[�^�ł��B");
								hr = E_FAIL;
							}
						}
					}

					for(MotionChannelVector::iterator i = channels.begin(); i != channels.end(); ++i) {
						(*i)->setIsOpen(is_open);
						(*i)->setPosType(postype);
					}
					if(!framename.empty()) self.set(framename, channels);
				}
			}
			return hr;
		}

		/** XFile�̓ǂ݂���
		 */
		HRslt loadX(
			GraphFile &self, /**< �Ώۂ�Xfile�I�u�W�F�N�g*/
			XFileReadingWork &work,
			const XData &cur, /**< �J�����g�̈ʒu������XData�I�u�W�F�N�g */
			Body *body=NULL, /**< �J�����g�̊K�w�c���[ */
			Body::NodePtr cnode=NULL /**< �J�����g�̃m�[�h */
		) {
			//PRNN("chunk name : "<<cur.name());
			if(TID_D3DRMMesh == cur.type()) {
				//PRNN("TID_D3DRMMesh found");
				Pointer<graphic::Model> w = new graphic::Model;
				if(w) {
					HRslt hr = setUpModel(*w, cur);
					if(hr) {
						self.push_back(w);
						if(body) {
							Pointer<Flesh> ww = new Flesh;
							if(ww) {
								ww->setUp(w, body, cnode);
								body->fleshies().push_back(ww);
							}
						}
					}
					else GCTP_TRACE(hr);
				}
				//PRNN("TID_D3DRMMesh end");
			}
			else if(xext::TID_Wire == cur.type()) {
				//PRNN("xext::TID_Wire found");
				Pointer<graphic::Model> w = new graphic::Model;
				if(w) {
					HRslt hr = setUpWireModel(*w, cur);
					if(hr) {
						self.push_back(w);
						if(body) {
							Pointer<Flesh> ww = new Flesh;
							if(ww) {
								ww->setUp(w, body, cnode);
								body->fleshies().push_back(ww);
							}
						}
					}
					else GCTP_TRACE(hr);
				}
				//PRNN("xext::TID_Wire end");
			}
			else if(TID_D3DRMFrameTransformMatrix == cur.type()) {
				//PRNN("TID_D3DRMFrameTransformMatrix found");
				if(cnode) cnode->val.getLCM() = MatrixC(*(D3DXMATRIX *)cur.data());
				else PRN("cnode is NULL!"<<endl);
			}
			else if(DXFILEOBJ_AnimTicksPerSecond == cur.type()) {
				//PRNN("DXFILEOBJ_AnimTicksPerSecond found");
				work.ticks_per_sec = *(DWORD *)cur.data();
			}
			else if(TID_D3DRMAnimationSet == cur.type()) {
				//PRNN("TID_D3DRMAnimationSet found");
				Pointer<Motion> w = new Motion;
				if(w) {
					GCTP_TRACE(w);
					setUpMotion(*w, cur);
					w->setTicksPerSec(static_cast<float>(work.ticks_per_sec));
					self.push_back(w);
				}
				//PRNN("TID_D3DRMAnimationSet end");
			}
			else if(TID_D3DRMFrame == cur.type()) {
				//PRNN("TID_D3DRMFrame found");
				if(body) cnode = body->add(*cnode, cur.name().c_str());
				else {
					body = new Body;
					if(body) {
						self.push_back(body);
						body->setUp(cur.name().c_str());
						cnode = *body;
					}
				}
				
				for(uint i = 0; i < cur.size(); i++) loadX(self, work, cur[i], body, cnode);
				//PRNN("TID_D3DRMFrame end");
			}
			else {
				PRNN(cur.type() << " : Parse Error or Unknown chunk");
#define UNKNOWN_CHUNK(_guid) if(_guid == cur.type()) PRNN(#_guid << " : Unknown chunk")
				UNKNOWN_CHUNK(TID_DXFILEHeader);
				UNKNOWN_CHUNK(TID_D3DRMInfo);
				UNKNOWN_CHUNK(TID_D3DRMMesh);
				UNKNOWN_CHUNK(TID_D3DRMVector);
				UNKNOWN_CHUNK(TID_D3DRMMeshFace);
				UNKNOWN_CHUNK(TID_D3DRMMaterial);
				UNKNOWN_CHUNK(TID_D3DRMMaterialArray);
				UNKNOWN_CHUNK(TID_D3DRMFrame);
				UNKNOWN_CHUNK(TID_D3DRMFrameTransformMatrix);
				UNKNOWN_CHUNK(TID_D3DRMMeshMaterialList);
				UNKNOWN_CHUNK(TID_D3DRMMeshTextureCoords);
				UNKNOWN_CHUNK(TID_D3DRMMeshNormals);
				UNKNOWN_CHUNK(TID_D3DRMCoords2d);
				UNKNOWN_CHUNK(TID_D3DRMMatrix4x4);
				UNKNOWN_CHUNK(TID_D3DRMAnimation);
				UNKNOWN_CHUNK(TID_D3DRMAnimationSet);
				UNKNOWN_CHUNK(TID_D3DRMAnimationKey);
				UNKNOWN_CHUNK(TID_D3DRMFloatKeys);
				UNKNOWN_CHUNK(TID_D3DRMMaterialAmbientColor);
				UNKNOWN_CHUNK(TID_D3DRMMaterialDiffuseColor);
				UNKNOWN_CHUNK(TID_D3DRMMaterialSpecularColor);
				UNKNOWN_CHUNK(TID_D3DRMMaterialEmissiveColor);
				UNKNOWN_CHUNK(TID_D3DRMMaterialPower);
				UNKNOWN_CHUNK(TID_D3DRMColorRGBA);
				UNKNOWN_CHUNK(TID_D3DRMColorRGB);
				UNKNOWN_CHUNK(TID_D3DRMGuid);
				UNKNOWN_CHUNK(TID_D3DRMTextureFilename);
				UNKNOWN_CHUNK(TID_D3DRMTextureReference);
				UNKNOWN_CHUNK(TID_D3DRMIndexedColor);
				UNKNOWN_CHUNK(TID_D3DRMMeshVertexColors);
				UNKNOWN_CHUNK(TID_D3DRMMaterialWrap);
				UNKNOWN_CHUNK(TID_D3DRMBoolean);
				UNKNOWN_CHUNK(TID_D3DRMMeshFaceWraps);
				UNKNOWN_CHUNK(TID_D3DRMBoolean2d);
				UNKNOWN_CHUNK(TID_D3DRMTimedFloatKeys);
				UNKNOWN_CHUNK(TID_D3DRMAnimationOptions);
				UNKNOWN_CHUNK(TID_D3DRMFramePosition);
				UNKNOWN_CHUNK(TID_D3DRMFrameVelocity);
				UNKNOWN_CHUNK(TID_D3DRMFrameRotation);
				UNKNOWN_CHUNK(TID_D3DRMLight);
				UNKNOWN_CHUNK(TID_D3DRMCamera);
				UNKNOWN_CHUNK(TID_D3DRMAppData);
				UNKNOWN_CHUNK(TID_D3DRMLightUmbra);
				UNKNOWN_CHUNK(TID_D3DRMLightRange);
				UNKNOWN_CHUNK(TID_D3DRMLightPenumbra);
				UNKNOWN_CHUNK(TID_D3DRMLightAttenuation);
				UNKNOWN_CHUNK(TID_D3DRMInlineData);
				UNKNOWN_CHUNK(TID_D3DRMUrl);
				UNKNOWN_CHUNK(TID_D3DRMProgressiveMesh);
				UNKNOWN_CHUNK(TID_D3DRMExternalVisual);
				UNKNOWN_CHUNK(TID_D3DRMStringProperty);
				UNKNOWN_CHUNK(TID_D3DRMPropertyBag);
				UNKNOWN_CHUNK(TID_D3DRMRightHanded);
				UNKNOWN_CHUNK(DXFILEOBJ_XSkinMeshHeader);
				UNKNOWN_CHUNK(DXFILEOBJ_VertexDuplicationIndices);
				UNKNOWN_CHUNK(DXFILEOBJ_SkinWeights);
#undef UNKNOWN_CHUNK
			}
			return S_OK;
		}

	} // namespace anonymous

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, GraphFile, Object);

	/** X�t�@�C������V�[���t�@�C�����\�[�X�𐶐�����B
	 *
	 * @return �G���[�l
	 */
	bool GraphFile::setUpFromX(const _TCHAR *fn)
	{
		HRslt hr;
		XFileReader file;
		hr = file.open(fn);
		// ���ꂪ�A�Ȃ���E_OUTOFMEMORY��Ԃ����Ƃ�����B
		// ��̂ǂ��Ń��b�N����΂����񂾁H
		if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
			::Sleep(1);
			hr = file.open(fn);
		}
		if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
			::Sleep(1);
			hr = file.open(fn);
		}
		if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
			::Sleep(1);
			hr = file.open(fn);
		}
		if(hr) {
			PRNN(_T("Begin read Xfile : ")<<fn);
			TURI _fn = fn;
			XFileReadingWork work;
			work.ticks_per_sec = 30;
			_TCHAR cur_dir[MAX_PATH];
			::GetCurrentDirectory(MAX_PATH, cur_dir);
			::SetCurrentDirectory(_fn.path().c_str());
			for(uint i = 0; i < file.size(); i++) {
				hr = loadX(*this, work, file[i]);
				if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
					::Sleep(1);
					hr = loadX(*this, work, file[i]);
				}
				if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
					::Sleep(1);
					hr = loadX(*this, work, file[i]);
				}
				if(hr == E_OUTOFMEMORY) { // �c�߂�ǂ���łR�񎎍s���邱�Ƃɂ���c
					::Sleep(1);
					hr = loadX(*this, work, file[i]);
				}
				if(!hr) break;
			}
			::SetCurrentDirectory(cur_dir);
			PRNN(_T("End read Xfile : ")<<fn);
			if(!hr) GCTP_TRACE(hr);
		}
		else PRNN("Xfile "<<fn<<" : "<<hr);
		return hr;
	}

	/** ���������X�t�@�C������V�[���t�@�C�����\�[�X�𐶐�����B
	 *
	 * @return �G���[�l
	 */
	bool GraphFile::setUpFromX(BufferPtr buffer)
	{
		HRslt hr;
		XFileReader file;
		hr = file.open(buffer->buf(), buffer->size());
		if(hr) {
			PRNN(_T("Begin read Xfile"));
			XFileReadingWork work;
			work.ticks_per_sec = 30;
			for(uint i = 0; i < file.size(); i++) {
				hr = loadX(*this, work, file[i]);
				if(!hr) break;
			}
			PRNN(_T("End read Xfile"));
			if(!hr) GCTP_TRACE(hr);
		}
		else PRNN("Xfile error : "<<hr);
		return hr;
	}

}} // namespace gctp::scene
