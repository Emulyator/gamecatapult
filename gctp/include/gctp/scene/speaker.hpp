#ifndef _GCTP_SCENE_SPEAKER_HPP_
#define _GCTP_SCENE_SPEAKER_HPP_
/** @file
 * GameCatapult スピーカー（３Ｄサウンド音源）オブジェクトクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/08 11:18:38
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/tuki.hpp>
#include <gctp/strutumnode.hpp>
#include <gctp/audio/speaker.hpp>
#include <gctp/audio/player.hpp>

namespace gctp { namespace scene {

	class World;

	/** シーン内の音源オブジェクト
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 2:11:01
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Speaker : public Object
	{
	public:
		/// 音源種
		enum Type {
			AMBIENT,	///< 無指向の環境音
			OMNI,		///< 無指向性音源
			SPOT		///< スポット音源
		};

		float  range;
		float  min_distance;
		float  theta;
		float  phi;
		
		Speaker();

		void set(const audio::Player &ambient);
		void set(const audio::Speaker &omni_or_spot);

		/// 新規にノードを持つ
		void newNode();
		/// 既存ノードにアタッチ
		void attach(Handle<StrutumNode> node);

		void enter(World &world);
		void exit(World &world);

		/// 音源種
		Type type() const { return type_; }

		/// アタッチしているノード
		Handle<StrutumNode> node() const { return node_; }
		/// 境界球
		const Sphere &bs() const { return bs_; }
		
		bool play(int times = 1);
		bool isPlaying();
		void stop();
		void release();
		void reset();

		/// オーディオデバイスに音源情報をセット
		void apply() const;
		/// 境界球の更新
		void update();
		
	protected:
		audio::Speaker speaker_;
		audio::Player player_;
		Handle<StrutumNode> node_;
		Pointer<StrutumNode> own_node_;
		Sphere bs_;
		
		bool setUp(luapp::Stack &L);
		void newNode(luapp::Stack &L);
		void attach(luapp::Stack &L);
		void enter(luapp::Stack &L);
		void exit(luapp::Stack &L);
		int isPlaying(luapp::Stack &L);
		int play(luapp::Stack &L);
		int stop(luapp::Stack &L);
		void setSource(luapp::Stack &L);
		int getType(luapp::Stack &L);
		void setPosition(luapp::Stack &L);
		int getPosition(luapp::Stack &L);
		void setPosture(luapp::Stack &L);
		int getPosture(luapp::Stack &L);
		void setRange(luapp::Stack &L);

	private:
		Type   type_;

		GCTP_DECLARE_CLASS;
		TUKI_DECLARE(Speaker);
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_SPEAKER_HPP_