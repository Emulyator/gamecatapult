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
#include <limits>

using namespace std;

namespace gctp { namespace scene {

	Light::Light() {
		ambient = Color(0, 0, 0);
		diffuse = Color(1, 1, 1);
		specular = Color(0, 0, 0);
		range = 1;
		attenuation[0] = 0;
		attenuation[1] = 1;
		attenuation[0] = 0;
	}

	Light::Light(const graphic::DirectionalLight &light)
	{
		attenuation[0] = 0;
		attenuation[1] = 1;
		attenuation[0] = 0;
		set(light);
	}

	Light::Light(const graphic::PointLight &light)
	{
		set(light);
	}

	Light::Light(const graphic::SpotLight &light)
	{
		set(light);
	}

	void Light::set(const graphic::DirectionalLight &light)
	{
		type = AREA;
		ambient = light.ambient;
		diffuse = light.diffuse;
		specular = light.specular;
		range = numeric_limits<float>::infinity();
	}
	
	void Light::set(const graphic::PointLight &light)
	{
		type = POINT;
		ambient = light.ambient;
		diffuse = light.diffuse;
		specular = light.specular;
		range = light.range;
		attenuation[0] = light.attenuation[0];
		attenuation[1] = light.attenuation[1];
		attenuation[2] = light.attenuation[2];
		if(node()) {
		}
	}

	void Light::set(const graphic::SpotLight &light)
	{
		type = SPOT;
		ambient = light.ambient;
		diffuse = light.diffuse;
		specular = light.specular;
		range = light.range;
		attenuation[0] = light.attenuation[0];
		attenuation[1] = light.attenuation[1];
		attenuation[2] = light.attenuation[2];
		falloff = light.falloff;
		theta = light.theta;
		phi = light.phi;
	}

	void Light::enter(World &world)
	{
		world.light_list.push_back(this);
		world.light_list.unique();
		AspectStrutumNode<Object>::enter(world);
	}

	void Light::exit(World &world)
	{
		world.light_list.remove(this);
		AspectStrutumNode<Object>::exit(world);
	}

	void Light::apply() const
	{
		switch(type) {
		case AREA:	{
			graphic::DirectionalLight light;
			light.ambient = ambient;
			light.diffuse = diffuse;
			light.specular = specular;
			light.dir = node()->val.wtm().forward();
			graphic::device().pushLight(light);
					}
			break;
		case POINT:	{
			graphic::PointLight light;
			light.ambient = ambient;
			light.diffuse = diffuse;
			light.specular = specular;
			light.pos = node()->val.wtm().position();
			light.range = range;
			light.attenuation[0] = attenuation[0];
			light.attenuation[1] = attenuation[1];
			light.attenuation[2] = attenuation[2];
			graphic::device().pushLight(light);
					}
			break;
		case SPOT:	{
			graphic::SpotLight light;
			light.ambient = ambient;
			light.diffuse = diffuse;
			light.specular = specular;
			light.pos = node()->val.wtm().position();
			light.range = range;
			light.attenuation[0] = attenuation[0];
			light.attenuation[1] = attenuation[1];
			light.attenuation[2] = attenuation[2];
			light.dir = node()->val.wtm().forward();
			light.falloff = falloff;
			light.theta = theta;
			light.phi = phi;
			graphic::device().pushLight(light);
					}
			break;
		}
	}

	void Light::update()
	{
		switch(type) {
		case AREA:
		case POINT:
			bs_.r = range;
			bs_.c = node()->val.wtm().position();
			break;
		case SPOT:
			// 円錐が内接する球を求める
			bs_.r = range*tanf(0.5f*phi);
			if(bs_.r < range) {
				float cos_phi_half = cosf(0.5f*phi);
				bs_.r = range/(cos_phi_half*cos_phi_half*2);
				bs_.c = node()->val.wtm().position()+node()->val.wtm().forward().normal()*bs_.r;
			}
			else {
				bs_.c = node()->val.wtm().position()+node()->val.wtm().forward().normal()*range;
			}
			break;
		}
	}

	bool Light::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void Light::enter(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) enter(*world);
		}
	}

	void Light::exit(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			Pointer<World> world = tuki_cast<World>(L[1]);
			if(world) exit(*world);
		}
	}

	void Light::setType(luapp::Stack &L)
	{
		if(L.top() >= 1) {
			if(L[1].isNumber()) {
				type = (Type)L[1].toInteger();
			}
			else {
				const char *s = L[1].toCStr();
				if(s) {
					switch(*s) {
					case 'a':
					case 'A':
					case 'd':
					case 'D':
						type = AREA;
						break;
					case 'p':
					case 'P':
						type = POINT;
						break;
					case 's':
					case 'S':
						type = SPOT;
						break;
					}
				}
			}
		}
	}

	int Light::getType(luapp::Stack &L)
	{
		switch(type) {
		case AREA:
			L << "AREA";
			return 1;
		case POINT:
			L << "POINT";
			return 1;
		case SPOT:
			L << "SPOT";
			return 1;
		}
		return 0;
	}

	void Light::setAmbient(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			ambient.r = (float)L[1].toNumber();
			ambient.g = (float)L[2].toNumber();
			ambient.b = (float)L[3].toNumber();
			if(L.top() >= 4) ambient.a = (float)L[4].toNumber();
		}
	}

	int Light::getAmbient(luapp::Stack &L)
	{
		L << ambient.r << ambient.g << ambient.b << ambient.a;
		return 4;
	}

	void Light::setDiffuse(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			diffuse.r = (float)L[1].toNumber();
			diffuse.g = (float)L[2].toNumber();
			diffuse.b = (float)L[3].toNumber();
			if(L.top() >= 4) diffuse.a = (float)L[4].toNumber();
		}
	}

	int Light::getDiffuse(luapp::Stack &L)
	{
		L << diffuse.r << diffuse.g << diffuse.b << diffuse.a;
		return 4;
	}

	void Light::setSpecular(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			specular.r = (float)L[1].toNumber();
			specular.g = (float)L[2].toNumber();
			specular.b = (float)L[3].toNumber();
			if(L.top() >= 4) specular.a = (float)L[4].toNumber();
		}
	}

	int Light::getSpecular(luapp::Stack &L)
	{
		L << specular.r << specular.g << specular.b << specular.a;
		return 4;
	}

	void Light::setRange(luapp::Stack &L)
	{
		if(L.top() >= 1) range = (float)L[1].toNumber();
	}

	int Light::getRange(luapp::Stack &L)
	{
		L << range;
		return 1;
	}

	void Light::setAttenuation(luapp::Stack &L)
	{
		if(L.top() >= 3) {
			attenuation[0] = (float)L[1].toNumber();
			attenuation[1] = (float)L[2].toNumber();
			attenuation[2] = (float)L[3].toNumber();
		}
	}

	int Light::getAttenuation(luapp::Stack &L)
	{
		L << attenuation[0] << attenuation[1] << attenuation[2];
		return 3;
	}

	void Light::setFalloff(luapp::Stack &L)
	{
		if(L.top() >= 1) falloff = (float)L[1].toNumber();
	}

	int Light::getFalloff(luapp::Stack &L)
	{
		L << falloff;
		return 1;
	}

	void Light::setTheta(luapp::Stack &L)
	{
		if(L.top() >= 1) theta = (float)L[1].toNumber();
	}

	int Light::getTheta(luapp::Stack &L)
	{
		L << theta;
		return 1;
	}

	void Light::setPhi(luapp::Stack &L)
	{
		if(L.top() >= 1) phi = (float)L[1].toNumber();
	}

	int Light::getPhi(luapp::Stack &L)
	{
		L << phi;
		return 1;
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, Light, Object);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, Light)
		GCTP_SCENE_ASPECTSTRUTUMNODE_TUKI_METHODS(Light)
		TUKI_METHOD(Light, setType)
		TUKI_METHOD(Light, getType)
		TUKI_METHOD(Light, setAmbient)
		TUKI_METHOD(Light, getAmbient)
		TUKI_METHOD(Light, setDiffuse)
		TUKI_METHOD(Light, getDiffuse)
		TUKI_METHOD(Light, setSpecular)
		TUKI_METHOD(Light, getSpecular)
		TUKI_METHOD(Light, setRange)
		TUKI_METHOD(Light, getRange)
		TUKI_METHOD(Light, setAttenuation)
		TUKI_METHOD(Light, getAttenuation)
		TUKI_METHOD(Light, setFalloff)
		TUKI_METHOD(Light, getFalloff)
		TUKI_METHOD(Light, setTheta)
		TUKI_METHOD(Light, getTheta)
		TUKI_METHOD(Light, setPhi)
		TUKI_METHOD(Light, getPhi)
	TUKI_IMPLEMENT_END(Light)

}} // namespace gctp::scene
