/** @file
 * GameCatapult モデルミキサー
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 21:36:50
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include "common.h"
#include <gctp/scene/modelmixer.hpp>

using namespace std;

namespace gctp { namespace scene {
	
	ModelMixer::ModelMixer(int track_num)
	{
		tracks_.resize(track_num);
		if(track_num>1) {
			model_argument_.resize(track_num);
			weight_argument_.resize(track_num);
		}
	}

	/** トラックを追加。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/08 0:42:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void ModelMixer::add(Handle<graphic::Model> model)
	{
		tracks_.push_back(ModelTrack(model));
		if(tracks_.size()>0) {
			model_argument_.resize(tracks_.size());
			weight_argument_.resize(tracks_.size());
		}
	}

	/** 更新
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2006/09/06 2:06:01
	 * Copyright (C) 2006 SAM (T&GG, Org.). All rights reserved.
	 */
	void ModelMixer::update()
	{
	}

	/** 最終的な表示のためのモデルを帰す
	 *
	 * 多分こうじゃなくてModel互換のdrawメソッドを用意した方がいいな。\n
	 * そうすればシェーダーでブレンドするのがやりやすい、かも
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/08 0:42:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	graphic::Model &ModelMixer::getModel()
	{
		return *blended_;
	}

	/** 最終的な表示のためのモデルを帰す(const版)
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/10/08 0:42:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	const graphic::Model &ModelMixer::getModel() const
	{
		return *blended_;
	}


}} // namespace gctp
