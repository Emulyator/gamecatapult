#ifndef _GCTP_GRAPHIC_SPRITE_HPP_
#define _GCTP_GRAPHIC_SPRITE_HPP_
/** @file
 * GameCatapult スプライトクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/18 21:54:52
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/class.hpp>
#include <gctp/pointer.hpp>
#include <gctp/color.hpp>
#include <gctp/graphic/rsrc.hpp>
#include <gctp/graphic/vertexbuffer.hpp>
#include <gctp/matrix2.hpp>
#include <gctp/types.hpp>
#include <vector>

namespace gctp { namespace graphic {

	namespace dx {
		class StateBlockRsrc;
	}

	/** スプライト定義情報
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 5:46:30
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct SpriteDesc {
		Point2f pos[4];
		Point2f uv[4];
		Color32 color[4];
		Color32 hilight[4];
		/// テクスチャを実寸表示する設定
		SpriteDesc &setUp(const class Texture &tex);

		/// 均等拡縮
		SpriteDesc &scale(const Point2f &center, float scl)
		{
			Matrix2 m; m.scale(scl, scl);
			pos[0] -= center; pos[0] = m*pos[0]; pos[0] += center;
			pos[1] -= center; pos[1] = m*pos[1]; pos[1] += center;
			pos[2] -= center; pos[2] = m*pos[2]; pos[2] += center;
			pos[3] -= center; pos[3] = m*pos[3]; pos[3] += center;
			return *this;
		}
		/// 拡縮
		SpriteDesc &scale(const Point2f &center, const Vector2 &scl)
		{
			Matrix2 m; m.scale(scl);
			pos[0] -= center; pos[0] = m*pos[0]; pos[0] += center;
			pos[1] -= center; pos[1] = m*pos[1]; pos[1] += center;
			pos[2] -= center; pos[2] = m*pos[2]; pos[2] += center;
			pos[3] -= center; pos[3] = m*pos[3]; pos[3] += center;
			return *this;
		}
		/// 回転
		SpriteDesc &rot(const Point2f &center, float theta)
		{
			Matrix2 m; m.rot(theta);
			pos[0] -= center; pos[0] = m*pos[0]; pos[0] += center;
			pos[1] -= center; pos[1] = m*pos[1]; pos[1] += center;
			pos[2] -= center; pos[2] = m*pos[2]; pos[2] += center;
			pos[3] -= center; pos[3] = m*pos[3]; pos[3] += center;
			return *this;
		}
		/// 行列を適用
		SpriteDesc &mult(const Point2f &center, const Matrix2 &m)
		{
			pos[0] -= center; pos[0] = m*pos[0]; pos[0] += center;
			pos[1] -= center; pos[1] = m*pos[1]; pos[1] += center;
			pos[2] -= center; pos[2] = m*pos[2]; pos[2] += center;
			pos[3] -= center; pos[3] = m*pos[3]; pos[3] += center;
			return *this;
		}
		/// オフセットを加える
		SpriteDesc &addOffset(Point2f ofs) {
			pos[0] += ofs;
			pos[1] += ofs;
			pos[2] += ofs;
			pos[3] += ofs;
			return *this;
		}
		/// 整数レクトから４頂点を設定
		SpriteDesc &setPos(Rect rc) {
			pos[0].x = (float)rc.left;  pos[0].y = (float)rc.top;
			pos[1].x = (float)rc.right; pos[1].y = (float)rc.top;
			pos[2].x = (float)rc.left;  pos[2].y = (float)rc.bottom;
			pos[3].x = (float)rc.right; pos[3].y = (float)rc.bottom;
			return *this;
		}
		/// レクトから４頂点を設定
		SpriteDesc &setPos(Rectf rc) {
			pos[0].x = rc.left;  pos[0].y = rc.top;
			pos[1].x = rc.right; pos[1].y = rc.top;
			pos[2].x = rc.left;  pos[2].y = rc.bottom;
			pos[3].x = rc.right; pos[3].y = rc.bottom;
			return *this;
		}
		/// レクトから４頂点分のＵＶを設定
		SpriteDesc &setUV(Rectf rc) {
			uv[0].x = rc.left;  uv[0].y = rc.top;
			uv[1].x = rc.right; uv[1].y = rc.top;
			uv[2].x = rc.left;  uv[2].y = rc.bottom;
			uv[3].x = rc.right; uv[3].y = rc.bottom;
			return *this;
		}
		/// ４頂点すべてに同一の色指定
		SpriteDesc &setColor(Color32 col) { color[0] = color[1] = color[2] = color[3] = col; return *this; }
		/// ４頂点すべてに同一のハイライト指定
		SpriteDesc &setHilight(Color32 col) { hilight[0] = hilight[1] = hilight[2] = hilight[3] = col; return *this; }
	};

	/// マルチテクスチャ版（２テクスチャ）
	struct SpriteDesc2 : SpriteDesc {
		Point2f uv2[4];
		/// レクトから４頂点分のＵＶを設定
		SpriteDesc2 &setUV2(Rectf rc) {
			uv2[0].x = rc.left;  uv2[0].y = rc.top;
			uv2[1].x = rc.right; uv2[1].y = rc.top;
			uv2[2].x = rc.left;  uv2[2].y = rc.bottom;
			uv2[3].x = rc.right; uv2[3].y = rc.bottom;
			return *this;
		}
	};

	/// Textはこれに文字スプライトを格納して返すことが出来る
	class SpriteDescVector {
	public:
		typedef std::vector<SpriteDesc> VectorType;
		VectorType descs;
	};

	/** スプライトバッファクラス
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 4:02:46
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class SpriteBuffer : public VertexBuffer {
	public:
		struct LVertex;
		struct TLVertex;
		struct L2Vertex;
		struct TL2Vertex;

		enum VertexType {
			VT_L,
			VT_TL,
			VT_L2,
			VT_TL2,
		};

		enum AddressMode {
			ADDRESS_NONE,
			ADDRESS_WRAP,
			ADDRESS_CLAMP,
		};

		HRslt setUp(const _TCHAR *name);
		HRslt setUp(uint maxnum = default_maxnum_, VertexType type = VT_TL);

		SpriteBuffer &begin(bool do_filter = true);
		SpriteBuffer &begin(const class Texture &tex, bool do_filter = true, AddressMode addressmode = ADDRESS_NONE);
		SpriteBuffer &begin(const Size2 &screen/**< 仮想スクリーンサイズ */, bool do_filter = true);
		SpriteBuffer &begin(const Size2 &screen/**< 仮想スクリーンサイズ */, const class Texture &tex, bool do_filter = true);
		SpriteBuffer &draw(const SpriteDesc &desc);
		SpriteBuffer &draw(const SpriteDesc2 &desc);
		SpriteBuffer &draw(const Texture &tex, const SpriteDesc &desc);
		SpriteBuffer &draw(const Texture &tex, const SpriteDesc2 &desc);
		SpriteBuffer &set(const Texture &tex);
		SpriteBuffer &end();

		bool isFull();

		uint maxnum();

		static void setDefaultMaxnum(uint maxnum) { default_maxnum_ = maxnum; }

	GCTP_DECLARE_CLASS

	protected:
		static uint default_maxnum_;
		HRslt draw();

		uint cur_;
		uint maxnum_;
		uint lastfreenum_;

		Pointer<dx::StateBlockRsrc>	sb_;
	private:
		void *locked_;
		VertexType type_;
	};

}}

#endif //_GCTP_GRAPHIC_SPRITE_HPP_