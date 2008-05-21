/** @file
 * GameCatapult ライトオブジェクトクラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/light.hpp>
#include <gctp/scene/world.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/light.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	AmbientLight::AmbientLight() {}

	AmbientLight::AmbientLight(const Color &color) : color_(color) {}
	
	void AmbientLight::set(const Color &color)
	{
		color_ = color;
	}

	bool AmbientLight::onReach() const
	{
		graphic::setAmbient(color_);
		return true;
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, AmbientLight, Object);


	ParallelLight::ParallelLight() {}

	ParallelLight::ParallelLight(const graphic::DirectionalLight &light) : light_(light) {}
	
	void ParallelLight::set(const graphic::DirectionalLight &light)
	{
		light_ = light;
	}

	bool ParallelLight::onReach() const
	{
		graphic::pushLight(light_);
		return true;
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, ParallelLight, Object);


	Light::Light() {}
	Light::Light(const graphic::PointLight &point) { set(point); }
	Light::Light(const graphic::SpotLight &spot) { set(spot); }
	
	void Light::set(const graphic::PointLight &light)
	{
		type_ = POINT;
		ambient_ = light.ambient;
		diffuse_ = light.diffuse;
		specular_ = light.specular;
		range_ = light.range;
		attenuation_[0] = light.attenuation[0];
		attenuation_[1] = light.attenuation[1];
		attenuation_[2] = light.attenuation[2];
		//stance_.pos = light.pos;
		//stance_.ort = Quat::identity();
	}

	void Light::set(const graphic::SpotLight &light)
	{
		type_ = SPOT;
		ambient_ = light.ambient;
		diffuse_ = light.diffuse;
		specular_ = light.specular;
		range_ = light.range;
		attenuation_[0] = light.attenuation[0];
		attenuation_[1] = light.attenuation[1];
		attenuation_[2] = light.attenuation[2];
		falloff_ = light.falloff;
		theta_ = light.theta;
		phi_ = light.phi;
		//stance_.pos = light.pos;
		//stance_.ort.setSmallestArc(Vector(0.0f, 0.0f, 1.0f), light.dir);
	}

	void Light::setUp()
	{
		//stance_.setDefault();
		node_ = StrutumNode::create();
	}

	void Light::enter(World &world)
	{
		world.strutum_tree.root()->push(node_.get());
		//world.rendering_tree->push(this);
	}

	bool Light::onReach() const
	{
		switch(type_) {
		case POINT:	{
			graphic::PointLight light;
			light.ambient = ambient_;
			light.diffuse = diffuse_;
			light.specular = specular_;
			light.pos = node_->val.wtm().position();
			light.range = range_;
			light.attenuation[0] = attenuation_[0];
			light.attenuation[1] = attenuation_[1];
			light.attenuation[2] = attenuation_[2];
			graphic::pushLight(light);
					}
			break;
		case SPOT:	{
			graphic::SpotLight light;
			light.ambient = ambient_;
			light.diffuse = diffuse_;
			light.specular = specular_;
			light.pos = node_->val.wtm().position();
			light.range = range_;
			light.attenuation[0] = attenuation_[0];
			light.attenuation[1] = attenuation_[1];
			light.attenuation[2] = attenuation_[2];
			light.dir = node_->val.wtm().transformVector(VectorC(0.0f, 0.0f, 1.0f));
			light.falloff = falloff_;
			light.theta = theta_;
			light.phi = phi_;
			graphic::pushLight(light);
					}
			break;
		}
		return true;
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, Light, Object);

}} // namespace gctp::scene
