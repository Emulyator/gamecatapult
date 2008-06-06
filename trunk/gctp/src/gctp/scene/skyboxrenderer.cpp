/** @file
 * GameCatapult スカイボックスレンダラークラス
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 6:08:56
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
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

	GCTP_IMPLEMENT_CLASS_NS2(gctp, scene, SkyBoxRenderer, Renderer);
	TUKI_IMPLEMENT_BEGIN_NS2(gctp, scene, SkyBoxRenderer)
		TUKI_METHOD(SkyBoxRenderer, load)
	TUKI_IMPLEMENT_END(SkyBoxRenderer)

	namespace {

		struct Vertex {
			enum {
				FVF  = (D3DFVF_XYZ|D3DFVF_DIFFUSE),
			};
			D3DXVECTOR3 p; D3DCOLOR color;
			void set(float x, float y, float z)
			{
				p.x = x; p.y = y; p.z = z;
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
			"    MipFilter = LINEAR;\n"
			"	AddressU = CLAMP;\n"
			"	AddressV = CLAMP;\n"
			"};\n"
			"\n"
			"float4x4 InvWorldViewProj;\n"
			"\n"
			"//////////////////////////\n"
			"\n"
			"struct CubeVertexOutput\n"
			"{\n"
			"   	float4 Position	: POSITION;\n"
			"    float3 UV		: TEXCOORD0;\n"
			"};\n"
			"\n"
			"CubeVertexOutput CubeVS(\n"
			"		float4 Position : POSITION,\n"
			"		uniform float4x4 ivp : ViewProjInverse\n"
			") {\n"
			"    CubeVertexOutput OUT;\n"
			"	OUT.Position = Position;\n"
			"    OUT.UV = normalize(mul(Position, ivp));\n"
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

	SkyBoxRenderer::SkyBoxRenderer()
	{
		if(vb_.setUp(graphic::VertexBuffer::STATIC, graphic::dx::FVF(Vertex::FVF), 4)) {
			graphic::VertexBuffer::ScopedLock al(vb_);
			al.get<Vertex>(0).set(-1,  1,  1);
			al.get<Vertex>(1).set( 1,  1,  1);
			al.get<Vertex>(2).set(-1, -1,  1);
			al.get<Vertex>(3).set( 1, -1,  1);
		}
		shader_ = graphic::device().db()[_T("skybox.fx")];
		if(!shader_) {
			default_shader_ = graphic::device().db()[_T("HLSL_SKYBOX")].lock();
			if(!default_shader_) {
				default_shader_ = new graphic::dx::HLSLShader;
				default_shader_->setUp(skybox_fx, sizeof(skybox_fx));
				graphic::device().db().insert(_T("HLSL_SKYBOX"), default_shader_);
			}
			shader_ = default_shader_;
		}
	}

	/** ワールド描画
	 *
	 * レンダリングツリーに描画メッセージを送る
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2005/01/11 2:34:02
	 */	
	bool SkyBoxRenderer::onReach(float delta) const
	{
		Handle<graphic::dx::HLSLShader> shader = shader_;
		if(shader) {
			Matrix proj;
			Size2f screen = Camera::current().screen();
			Rectf subwindow = Camera::current().subwindow();
			proj.setFOV(Camera::current().fov(), screen.x/screen.y, subwindow.left, subwindow.top, subwindow.right, subwindow.bottom, 1, 1000);
			(*shader)->SetMatrix("InvWorldViewProj", (Camera::current().view() * proj).inverse());
			(*shader)->SetTexture("BgTexture", *texture_);
			shader->begin();
			vb_.draw(0, D3DPT_TRIANGLESTRIP, 2, 0);
		}
		return false;
	}

	bool SkyBoxRenderer::onLeave(float delta) const
	{
		Handle<graphic::dx::HLSLShader> shader = shader_;
		if(shader) {
			shader->end();
		}
		return true;
	}

	bool SkyBoxRenderer::setUp(luapp::Stack &L)
	{
		// Context:createで製作する
		return false;
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
		}
	}

}} // namespace gctp::scene
