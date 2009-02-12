/** @file
 * GameCatapult 属性情報モデルクラス
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: modeltag.cpp,v 1.3 2005/02/21 02:21:12 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include "common.h"
#include <gctp/scene/attrmodel.hpp>
#include <gctp/strutumnode.hpp>
#include <gctp/wcstr.hpp>
#include <btBulletDynamicsCommon.h>

using namespace std;

namespace gctp { namespace scene {

	GCTP_IMPLEMENT_TYPEINFO(AttrModel, Object);

	AttrModel::AttrModel() : vertices_(0), indices_(0), array_(0), shape_(0)
	{
	}

	AttrModel::~AttrModel()
	{
		tearDown();
	}

	namespace {

#if _MSC_VER <= 1400
#pragma warning(push)
#pragma warning(disable:4200)
#endif
		struct Vertices {
			ulong num;
			Vector vertices[];
		};

		struct Face {
			ulong num;
			ulong indices[];
			size_t size() const
			{
				return sizeof(num)+sizeof(ulong)*num;
			}
		};

		struct Faces {
			ulong num;
			Face first_face;
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

	HRslt AttrModel::setUp(CStr name, const void *data)
	{
		tearDown();

		name_ = name;

		const Vertices *vert = reinterpret_cast<const Vertices *>(data);
		const Faces *faces = reinterpret_cast<const Faces *>(&vert->vertices[vert->num]);

		uint tri_num = 0;
		const char *cur_pface = reinterpret_cast<const char *>(&faces->first_face);
		for(ulong i = 0; i < faces->num; i++) {
			const Face *face = reinterpret_cast<const Face *>(cur_pface);
			tri_num += face->num-2;
			cur_pface += face->size();
		}
		
		vertices_ = new Vector[vert->num];
		memcpy(vertices_, vert->vertices, sizeof(Vector)*vert->num);

		indices_ = new int[tri_num*3];
		int tri_idx = 0;
		cur_pface = reinterpret_cast<const char *>(&faces->first_face);
		for(ulong i = 0; i < faces->num; i++) {
			const Face *face = reinterpret_cast<const Face *>(cur_pface);
			for(ulong j = 0; j < face->num-2; j++) {
				indices_[tri_idx++] = face->indices[0];
				indices_[tri_idx++] = face->indices[j+1];
				indices_[tri_idx++] = face->indices[j+2];
				// トライアングルファンと解釈。凹面の場合、うまくいかない場合がある
				// 凹面かどうかチェックすべきか？
			}
			cur_pface += face->size();
		}

		array_ = new btTriangleIndexVertexArray(tri_num, indices_, sizeof(int)*3, vert->num, (Real *)vertices_, sizeof(Real)*3);

		shape_ = new btBvhTriangleMeshShape(array_, true);
		//shape_ = new btGImpactMeshShape(array_);
		//shape_->updateBound();

		return S_OK;
	}

	void AttrModel::tearDown()
	{
		name_ = CStr();
		if(shape_) {
			delete shape_;
			shape_ = 0;
		}
		if(array_) {
			delete array_;
			array_ = 0;
		}
		if(vertices_) {
			delete vertices_;
			vertices_ = 0;
		}
		if(indices_) {
			delete indices_;
			indices_ = 0;
		}
	}


	GCTP_IMPLEMENT_TYPEINFO(AttrFlesh, Object);

	HRslt AttrFlesh::setUp(Handle<AttrModel> model, Handle<StrutumNode> node)
	{
		model_ = model;
		node_ = node;
		return S_OK;
	}

	/*Sphere AttrFlesh::bc()
	{
		Sphere ret;
		if(model_ && model_->shape()) {
			AABox aabb;
			aabb.upper = *(Vector *)&model_->shape()->getLocalBox().m_max;
			aabb.lower = *(Vector *)&model_->shape()->getLocalBox().m_min;
			ret.c = node_->val.wtm()*aabb.center();
			ret.r = 
			model_->shape()->getAabb(*(btTransform *)(&node_->val.wtm()), *(btVector3 *)&ret.lower, *(btVector3 *)&ret.upper);
		}
		return ret;
	}*/

	AABox AttrFlesh::getAABB()
	{
		AABox ret;
		if(model_ && model_->shape()) {
			btVector3 lower;
			btVector3 upper;
			model_->shape()->getAabb(*(btTransform *)(&node_->val.wtm()), lower, upper);
			ret.lower = *(Vector *)&lower;
			ret.upper = *(Vector *)&upper;
		}
		return ret;
	}

}} // namespace gctp::scene
