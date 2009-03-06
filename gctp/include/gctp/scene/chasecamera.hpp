#ifndef _GCTP_SCENE_CHASECAMERA_HPP_
#define _GCTP_SCENE_CHASECAMERA_HPP_
/** @file
 * GameCatapult Quake型カメラクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/scene/aspectupdater.hpp>
#include <gctp/strutumtree.hpp>

namespace gctp { namespace scene {

	class Camera;
	/** 追跡カメラ
	 *
	 * シーンカメラノードにアタッチするビヘイビア
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/16 8:10:04
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class ChaseCamera : public AspectUpdater<Object>
	{
	public:
		//bool allow_direction;
		float dumping_factor;
		Vector position_offset;
		Vector posture_offset;

		ChaseCamera();

		Handle<Camera> &target() { return target_; }
		Handle<Camera> target() const { return target_; }
		StrutumTree::NodeHndl &chasee() { return chasee_; }
		StrutumTree::NodeHndl chasee() const { return chasee_; }

	protected:
		virtual bool doUpdate(float delta);

		Handle<Camera> target_;
		StrutumTree::NodeHndl chasee_;

		bool LuaCtor(luapp::Stack &L);
		void setPositionOffset(luapp::Stack &L);
		void setPostureOffset(luapp::Stack &L);
		void setDumpingFactor(luapp::Stack &L);
		void attach(luapp::Stack &L);

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(ChaseCamera);
	};

}} // namespace gctp::scene

#endif // _GCTP_SCENE_CHASECAMERA_HPP_