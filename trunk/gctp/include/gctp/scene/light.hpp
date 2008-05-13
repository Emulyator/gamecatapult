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
#include <gctp/scene/renderer.hpp>
#include <gctp/strutumnode.hpp>
#include <gctp/graphic/light.hpp>

namespace gctp { namespace scene {

	class World;
	/** 環境光オブジェクト
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 8:44:22
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class AmbientLight : public Renderer
	{
	public:
		AmbientLight();
		AmbientLight(const Color &color);
		
		void set(const Color &color);
		virtual bool onReach() const;

		void enter(World &world);

	GCTP_DECLARE_CLASS

	protected:
		Color color_;
	};

	/** 平行光源オブジェクト
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 8:44:22
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class ParallelLight : public Renderer
	{
	public:
		ParallelLight();
		ParallelLight(const graphic::DirectionalLight &light);
		
		void set(const graphic::DirectionalLight &light);

		virtual bool onReach() const;

		void enter(World &world);

	GCTP_DECLARE_CLASS

	protected:
		graphic::DirectionalLight light_;
	};

	/** スポット、点光源など、位置情報をもつライト
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 2:11:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Light : public Renderer
	{
	public:
		enum Type {
			POINT,
			SPOT
		};
		Type type_;

		Light();
		Light(const graphic::PointLight &point);
		Light(const graphic::SpotLight &spot);
		
		void set(const graphic::PointLight &light);
		void set(const graphic::SpotLight &light);

		void setUp();

		void enter(World &world);

		/// アタッチしているノード
		Handle<StrutumNode> node() const { return node_; }
		
		virtual bool onReach() const;

	GCTP_DECLARE_CLASS

	protected:
		Color  ambient_;
		Color  diffuse_;
		Color  specular_;

		float  range_;
		float  attenuation_[3];
		float  falloff_;
		float  theta_;
		float  phi_;

		Pointer<StrutumNode> node_;
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_LIGHT_HPP_