#ifndef _GCTP_SCENE_LIGHT_HPP_
#define _GCTP_SCENE_LIGHT_HPP_
/** @file
 * GameCatapult ライトオブジェクトクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/tuki.hpp>
#include <gctp/scene/renderer.hpp>
#include <gctp/strutumnode.hpp>
#include <gctp/graphic/light.hpp>

namespace gctp { namespace scene {

	class World;

	/** シーン内のライトオブジェクト
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 2:11:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Light : public Object
	{
	public:
		/// ライト種
		enum Type {
			AREA,	///< エリアライト（平行光源。位置情報から優先順位を決める）
			POINT,	///< 点光源
			SPOT	///< スポットライト
		};

		Type   type;
		Color  ambient;
		Color  diffuse;
		Color  specular;
		float  range;
		float  attenuation[3];
		float  falloff;
		float  theta;
		float  phi;

		Light();
		explicit Light(const graphic::DirectionalLight &light);
		explicit Light(const graphic::PointLight &light);
		explicit Light(const graphic::SpotLight &light);
		
		void set(const graphic::DirectionalLight &light);
		void set(const graphic::PointLight &light);
		void set(const graphic::SpotLight &light);

		/// 新規にノードを持つ
		void newNode();
		/// 既存ノードにアタッチ
		void attach(Handle<StrutumNode> node);

		void enter(World &world);
		void exit(World &world);

		/// アタッチしているノード
		Handle<StrutumNode> node() const { return node_; }
		/// 境界球
		const Sphere &bs() const { return bs_; }
		
		/// グラフィックデバイスにライト情報をセット
		void apply() const;
		/// 境界球の更新
		void update();
		
	protected:
		Handle<StrutumNode> node_;
		Pointer<StrutumNode> own_node_;
		Sphere bs_;
		
		bool setUp(luapp::Stack &L);
		void newNode(luapp::Stack &L);
		void attach(luapp::Stack &L);
		void enter(luapp::Stack &L);
		void exit(luapp::Stack &L);
		void setPosition(luapp::Stack &L);
		int getPosition(luapp::Stack &L);
		void setPosture(luapp::Stack &L);
		int getPosture(luapp::Stack &L);
		void setType(luapp::Stack &L);
		int getType(luapp::Stack &L);
		void setAmbient(luapp::Stack &L);
		int getAmbient(luapp::Stack &L);
		void setDiffuse(luapp::Stack &L);
		int getDiffuse(luapp::Stack &L);
		void setSpecular(luapp::Stack &L);
		int getSpecular(luapp::Stack &L);
		void setRange(luapp::Stack &L);
		int getRange(luapp::Stack &L);
		void setAttenuation(luapp::Stack &L);
		int getAttenuation(luapp::Stack &L);
		void setFalloff(luapp::Stack &L);
		int getFalloff(luapp::Stack &L);
		void setTheta(luapp::Stack &L);
		int getTheta(luapp::Stack &L);
		void setPhi(luapp::Stack &L);
		int getPhi(luapp::Stack &L);

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(Light);
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_LIGHT_HPP_