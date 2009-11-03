#ifndef _GCTP_GRAPHIC_WIREBUFFER_HPP_
#define _GCTP_GRAPHIC_WIREBUFFER_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult ラインプリミティブバッファクラスヘッダファイル
 *
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

	namespace dx {
		class StateBlockRsrc;
	}

	class Texture;
	/** 線表示定義情報
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 5:46:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct WireDesc {
		Vector  s, e;
		Point2f uv[2];
		Color32 color[2];
		Color32 hilight[2];

		/// ４頂点すべてに同一の色指定
		WireDesc &setColor(Color32 col) { color[0] = color[1] = col; return *this; }
		/// ４頂点すべてに同一のハイライト指定
		WireDesc &setHilight(Color32 col) { hilight[0] = hilight[1] = col; return *this; }
	};

	/** 線描画バッファクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 4:02:46
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class WireBuffer : public VertexBuffer {
	public:
		struct LVertex;

		/// ブレンドモード
		enum BlendMode {
			OPEQUE,
			TRANSLUCENT,
			ADD,
			SUB,
			MUL,
		};

		HRslt setUp(uint maxnum = default_maxnum_);

		WireBuffer &begin(bool do_filter = true);
		WireBuffer &begin(const Texture &tex, bool do_filter = true);
		WireBuffer &draw(const WireDesc &desc);
		WireBuffer &draw(const Texture &tex, const WireDesc &desc);
		WireBuffer &draw(const WireDesc *desc, uint num);
		WireBuffer &draw(const Texture &tex, const WireDesc *desc, uint num);
		WireBuffer &set(const Texture &tex);
		WireBuffer &set(BlendMode blend);
		WireBuffer &end();

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

		//Handle<dx::StateBlockRsrc> sb_;
		Pointer<dx::StateBlockRsrc> sb_;
	private:
		void *locked_;
	};

}}

#endif //_GCTP_GRAPHIC_WIREBUFFER_HPP_
