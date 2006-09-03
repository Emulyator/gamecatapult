#ifndef _GCTP_GRAPHIC_PARTICLEBUFFER_HPP_
#define _GCTP_GRAPHIC_PARTICLEBUFFER_HPP_
/** @file
 * GameCatapult パーティクルクラスヘッダファイル
 *
 * パーティクルというより、エフェクト用動的メッシュ全般を扱う
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/18 21:54:52
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/types.hpp>
#include <gctp/class.hpp>
#include <gctp/pointer.hpp>
#include <gctp/color.hpp>
#include <gctp/vector.hpp>
#include <gctp/graphic/vertexbuffer.hpp>

namespace gctp { namespace graphic {

	class Texture;
	/** 点パーティクル定義情報
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 5:46:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct ParticleDesc {
		Vector  pos;
		Size2f  size;
		Point2f uv[4];
		Color32 color[4];
		Color32 hilight[4];

		/// レクトから４頂点分のＵＶを設定
		ParticleDesc &setUV(const Rectf &rc) {
			uv[0].x = rc.left;  uv[0].y = rc.top;
			uv[1].x = rc.right; uv[1].y = rc.top;
			uv[2].x = rc.left;  uv[2].y = rc.bottom;
			uv[3].x = rc.right; uv[3].y = rc.bottom;
			return *this;
		}
		/// ４頂点すべてに同一の色指定
		ParticleDesc &setColor(Color32 col) { color[0] = color[1] = color[2] = color[3] = col; return *this; }
		/// ４頂点すべてに同一のハイライト指定
		ParticleDesc &setHilight(Color32 col) { hilight[0] = hilight[1] = hilight[2] = hilight[3] = col; return *this; }
	};

	/** 線パーティクル定義情報
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 5:46:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct LineParticleDesc {
		uint          num;
		const Vector *pos;
		Size2f        size;
		Point2f       uv[4];
		Color32       color[4];
		Color32       hilight[4];

		/// レクトから４頂点分のＵＶを設定
		LineParticleDesc &setUV(Rectf rc) {
			uv[0].x = rc.left;  uv[0].y = rc.top;
			uv[1].x = rc.right; uv[1].y = rc.top;
			uv[2].x = rc.left;  uv[2].y = rc.bottom;
			uv[3].x = rc.right; uv[3].y = rc.bottom;
			return *this;
		}
		/// ４頂点すべてに同一の色指定
		LineParticleDesc &setColor(Color32 col) { color[0] = color[1] = color[2] = color[3] = col; return *this; }
		/// ４頂点すべてに同一のハイライト指定
		LineParticleDesc &setHilight(Color32 col) { hilight[0] = hilight[1] = hilight[2] = hilight[3] = col; return *this; }
	};

	/** パーティクルバッファクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 4:02:46
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class ParticleBuffer : public VertexBuffer {
	public:
		struct LVertex;

		/// ブレンドモード
		enum BlendMode {
			NONE,
			ADD,
			SUB,
//			MUL,
			ALPHA,
		};

		HRslt setUp(const char *name) { return setUp(); } // createOnDB用
		HRslt setUp(uint maxnum = default_maxnum_);

		ParticleBuffer &begin(bool do_filter = true);
		ParticleBuffer &begin(const Texture &tex, bool do_filter = true);
		ParticleBuffer &draw(const ParticleDesc &desc);
		ParticleBuffer &draw(const Texture &tex, const ParticleDesc &desc);
		ParticleBuffer &draw(const ParticleDesc *desc, uint num);
		ParticleBuffer &draw(const Texture &tex, const ParticleDesc *desc, uint num);
		ParticleBuffer &draw(const LineParticleDesc &desc);
		ParticleBuffer &draw(const Texture &tex, const LineParticleDesc &desc);
		ParticleBuffer &set(const Texture &tex);
		ParticleBuffer &set(BlendMode blend);
		ParticleBuffer &end();

		bool isFull();
		bool hasSpace(uint num);

		uint maxnum();

		static void setDefaultMaxnum(uint maxnum) { default_maxnum_ = maxnum; }

	GCTP_DECLARE_CLASS

	protected:
		static uint default_maxnum_;
		HRslt draw();

		uint cur_;
		uint maxnum_;

		Pointer<class ParticleSB>	sb_;
	private:
		void *locked_;
		Vector up_;
		Vector right_;
		Vector vpos_;
	};

}}

#endif //_GCTP_GRAPHIC_PARTICLEBUFFER_HPP_