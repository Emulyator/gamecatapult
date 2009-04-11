/** @file
 * GameCatapult スカイボックスレンダラークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2008 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/skyboxrenderer.hpp>
#include <gctp/scene/camera.hpp>
#include <gctp/graphic.hpp>
#include <gctp/graphic/shader.hpp>
#include <gctp/graphic/cubetexture.hpp>
#include <gctp/graphic/dx/hlslshader.hpp>
#include <gctp/context.hpp>
#include <gctp/dbgout.hpp>

using namespace std;

namespace gctp { namespace scene {

	namespace {

		struct Vertex {
			enum {
				FVF  = (D3DFVF_XYZ|D3DFVF_DIFFUSE),
			};
			D3DXVECTOR3 p; D3DCOLOR color;
			void set(float x, float y)
			{
				p.x = x; p.y = y; p.z = 1.0f;
				color = 0xFFFFFFFF;
			}
		};

		const char skybox_fx[] = 
			"texture BgTexture;\n"
			"samplerCUBE BgSampler = sampler_state\n"
			"{\n"
			"    Texture = <BgTexture>;\n"
			"    MinFilter = LINEAR;\n"
			"    MagFilter = LINEAR;\n"
			"    MipFilter = NONE;\n"
			"	AddressU = CLAMP;\n"
			"	AddressV = CLAMP;\n"
			"};\n"
			"\n"
			"float4x4 InvWorldViewProj;\n"
			"float2 ScreenOffset;\n"
			"\n"
			"//////////////////////////\n"
			"\n"
			"struct CubeVertexOutput\n"
			"{\n"
			"    float4 Position	: POSITION;\n"
			"    float3 UV		: TEXCOORD0;\n"
			"};\n"
			"\n"
			"CubeVertexOutput CubeVS(\n"
			"		float4 Position : POSITION,\n"
			"		uniform float4x4 ivp : ViewProjInverse\n"
			") {\n"
			"   CubeVertexOutput OUT;\n"
			"	OUT.Position = Position;\n"
			"	OUT.Position.x += ScreenOffset.x;\n"
			"	OUT.Position.y += ScreenOffset.y;\n"
			"   OUT.UV = normalize(mul(Position, ivp));\n"
			"	return OUT;\n"
			"}\n"
			"\n"
			"float4 CubePS(CubeVertexOutput IN) : COLOR\n"
			"{\n"
			"	return texCUBE(BgSampler, IN.UV);\n"
			"}\n"
			"\n"
			"//------------------------------------------------------------------------------\n"
			"// Techniques\n"
			"//------------------------------------------------------------------------------\n"
			"technique Bg\n"
			"{\n"
			"	pass p0\n"
			"	{\n"
			"		VertexShader = compile vs_1_1 CubeVS(InvWorldViewProj);\n"
			"		ALPHABLENDENABLE = FALSE;\n"
			"		ALPHATESTENABLE = FALSE;\n"
			"		CullMode = none;\n"
			"		ZEnable = true;\n"
			"		ZWriteEnable = false;\n"
			"		PixelShader  = compile ps_1_1 CubePS();\n"
			"	}\n"
			"}\n";

	}

	SkyBoxRenderer::SkyBoxRenderer() : radius(1000)
	{
	}

	void SkyBoxRenderer::setUp(int div_num)
	{
		if(vb_.setUp(graphic::VertexBuffer::STATIC, graphic::dx::FVF(Vertex::FVF), (div_num+1)*(div_num+1))) {
			graphic::VertexBuffer::ScopedLock al(vb_);
			float step = 2.0f/div_num;
			for(int y = 0; y < div_num+1; y++) {
				for(int x = 0; x < div_num+1; x++) {
					al.get<Vertex>(y*(div_num+1)+x).set(-1+step*x, 1-step*y);
					//dbgout << y*(div_num+1)+x << "(" << -1+step*x <<"," << 1-step*y << ") ";
				}
				//dbgout << endl;
			}
		}
		if(ib_.setUp(graphic::IndexBuffer::STATIC, graphic::IndexBuffer::SHORT, div_num*div_num*6)) {
			graphic::IndexBuffer::ScopedLock al(ib_);
			for(int y = 0; y < div_num; y++) {
				for(int x = 0; x < div_num; x++) {
					al.get<ushort>((y*div_num+x)*6+0) = y*(div_num+1)+x;
					al.get<ushort>((y*div_num+x)*6+1) = y*(div_num+1)+x+1;
					al.get<ushort>((y*div_num+x)*6+2) = (y+1)*(div_num+1)+x;
					al.get<ushort>((y*div_num+x)*6+3) = y*(div_num+1)+x+1;
					al.get<ushort>((y*div_num+x)*6+4) = (y+1)*(div_num+1)+x+1;
					al.get<ushort>((y*div_num+x)*6+5) = (y+1)*(div_num+1)+x;
					/*dbgout << y*(div_num+1)+x << ", "
						<< y*(div_num+1)+x+1 << ", "
						<< (y+1)*(div_num+1)+x << ", "
						<< y*(div_num+1)+x+1 << ", "
						<< (y+1)*(div_num+1)+x+1 << ", "
						<< (y+1)*(div_num+1)+x << endl;*/
				}
			}
		}
		shader_ = context().load(_T("skybox.fx"));
		if(!shader_) {
			shader_ = context()[_T("HLSL_SKYBOX")];
			if(!shader_) {
				Pointer<graphic::dx::HLSLShader> shader = new graphic::dx::HLSLShader;
				shader->setUp(skybox_fx, sizeof(skybox_fx));
				context().insert(shader, _T("HLSL_SKYBOX"));
				shader_ = shader;
			}
		}
		div_num_ = div_num;
	}

	/** スカイボックス描画
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2008/01/11 2:34:02
	 */	
	bool SkyBoxRenderer::onReach(float delta) const
	{
		Handle<graphic::dx::HLSLShader> shader = shader_;
		if(shader && *shader) {
			Matrix proj;
			Matrix view;
			float aspect_ratio = Camera::current().aspectRatio();
			float height = (float)graphic::device().getViewPort().size().y;
			Rectf renderrect = Camera::current().renderRect();
			proj.setFOV(Camera::current().fov(), aspect_ratio, renderrect.left, renderrect.top, renderrect.right, renderrect.bottom, radius/2, radius);
			view = Camera::current().view();
			view.position() = VectorC(0,0,0); // 方向のみ採用
			// おかげで、Radiusに意味がなくなった。それより分割数を可変にしたほうがいい
			(*shader)->SetMatrix("InvWorldViewProj", (view * proj).inverse());
			Vector2 screen_offset;
			screen_offset.x = -1.0f/(height*aspect_ratio);
			screen_offset.y = 1.0f/height;
			HRslt hr = (*shader)->SetFloatArray("ScreenOffset", &screen_offset.x, 2);
			if(!hr) GCTP_ERRORINFO(hr);
			if(texture_) (*shader)->SetTexture("BgTexture", *texture_);
			shader->begin();
			ib_.draw(vb_, 0, D3DPT_TRIANGLELIST, div_num_*div_num_*2, 0);
		}
		return false;
	}

	bool SkyBoxRenderer::onLeave(float delta) const
	{
		Handle<graphic::dx::HLSLShader> shader = shader_;
		if(shader && *shader) {
			shader->end();
		}
		return true;
	}

	bool SkyBoxRenderer::LuaCtor(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
	}

	void SkyBoxRenderer::setRadius(luapp::Stack &L)
	{
		if(L.top() >=1) radius = (float)L[1].toNumber();
	}

	void SkyBoxRenderer::load(luapp::Stack &L)
	{
		if(L.top() >=1) {
			if(L[1].isString()) {
				const _TCHAR *filename;
#ifdef UNICODE
				WCStr str(L[1].toCStr());
				filename = str.c_str();
#else
				filename = L[1].toCStr();
#endif
				Pointer<graphic::CubeTexture> texture = context()[filename].lock();
				if(texture) texture_ = texture;
			}
			if(L.top() >=2) setUp(L[2].toInteger());
			else setUp(7);
		}
	}

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, SkyBoxRenderer, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, SkyBoxRenderer)
		TUKI_METHOD(SkyBoxRenderer, setRadius)
		TUKI_METHOD(SkyBoxRenderer, load)
	TUKI_IMPLEMENT_END(SkyBoxRenderer)

}} // namespace gctp::scene
