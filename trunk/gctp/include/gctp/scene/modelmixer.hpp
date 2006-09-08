#ifndef _GCTP_SCENE_MODELMIXER_HPP_
#define _GCTP_SCENE_MODELMIXER_HPP_
/** @file
 * GameCatapult モーション制御クラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 18:43:01
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/pointer.hpp>
#include <gctp/scene/motion.hpp>

namespace gctp {
	class Skeleton;
	namespace graphic {
		class Model;
	}
}

namespace gctp { namespace scene {

	/** モデルトラッククラス
	 *
	 * モデルのトランジションを行う
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 21:52:43
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class ModelTrack {
	public:
		ModelTrack() : weight_(0) {}
		ModelTrack(Handle<graphic::Model> model) : weight_(0) {}

		void set(Handle<graphic::Model> model)
		{
			model_ = model;
		}
		
		float weight() const { return weight_; }
		float setWeight(float weight) { std::swap(weight_, weight); return weight; }

		Handle<graphic::Model> model() const { return model_; }

	private:
		Handle<graphic::Model> model_;
		float weight_;
	};

	/// MotionTrackのvector(そのまんま)
	typedef std::vector<ModelTrack> ModelTrackVector;

	/** モデルミキサークラス
	 *
	 * モーフして最終的な結果を描画する
	 *
	 * CPU使わずにシェーダーだけでもやれるようにするには、どうしたらいいのだろう。。。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 21:52:43
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class ModelMixer : public Object {
	public:
		ModelMixer(int track_num);

		void add(Handle<graphic::Model> model);
		ModelTrackVector &tracks() { return tracks_; }
		const ModelTrackVector &tracks() const { return tracks_; }
		bool isExist(ModelTrackVector::size_type idx) const { return (0 <= idx && idx < tracks_.size()); }

		void update();

		graphic::Model &getModel();
		const graphic::Model &getModel() const;

	private:
		ModelTrackVector tracks_;
		mutable std::vector<graphic::Model *> model_argument_;
		mutable std::vector<Real> weight_argument_;
		Pointer<graphic::Model> blended_;
	};

}} // namespace gctp

#endif //_GCTP_SCENE_MODELMIXER_HPP_