#ifndef _GCTP_GRAPHIC_FONTTEXTURE_HPP_
#define _GCTP_GRAPHIC_FONTTEXTURE_HPP_
/**@file
 * GameCatapult フォントテクスチャリソースクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/26 1:42:52
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/graphic/texture.hpp>
#include <gctp/types.hpp>
#include <gctp/vector.hpp>
#include <boost/scoped_ptr.hpp>

namespace gctp {
	
	class Font;

	namespace graphic {
		
		class SpriteBuffer;

		namespace detail {
			struct AsciiAttr;
			class DrawContext;
			class FontTextureDetail;
			class FontTextureSetDetail;
		}
	}
}

namespace gctp { namespace graphic {

	/** 文字に対応するUV情報
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 2:54:10
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct FontGlyph {
		Rectf	uv;
		Size2f	size;
		// @{
		/** 有効か？
		 *
		 * FontTeture::findで、「まだキャッシュされていない」を示す特異値を持っていないかどうかチェック
		 * すでにキャッシュされているなら、trueが返る
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/07/23 4:51:52
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		operator bool() const { return size.x >= 0 && size.y >= 0; }
		bool operator !() const { return size.x < 0 || size.y < 0; }
		// @}
	};

	/** 使用する文字のみをレンダリングしてキャッシュするフォントテクスチャ
	 *
	 * 最大テクスチャサイズが1024以下であれば最大テクスチャサイズを、そうでなければ
	 * 1024*1024で制作し、テクスチャを縦横64分割し、1bitフラグで空きを領域管理する。
	 * 1024では16*16が最少領域、512*512では8*8、256では4*4になる。一枚で最大4096文字種を格納できる計算。
	 * アドレスは１次元の序数で表され、幅優先トラバースしたときのquadtreeでの領域を表す。（これがalloc_の値）
	 * 0はトップノード、1,2,3,4がそれぞれ４分割された領域に…と言う具合に対応する。
	 * 最大64分割は、レベル数で言うと6レベルまで、と言うこと。
	 * テクスチャのサイズまではレンダできていいわけだが、それだと１文字で１テククチャ消費されてしまうので、
	 * 上限レベルを設定できる。（現在は固定で3。1024のとき、192ドット)。
	 *
	 * @todo 領域を使いまわせるようにする（今は溜め込む一方）
	 * @todo 実寸でレンダできない場合は、拡大表示するようにする
	 * @todo 文字サイズの上限を可変に
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/18 2:46:24
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class FontTexture : public Texture {
	public:
		enum { LEVEL = 6 };

		/// コンストラクタ
		FontTexture();

		/// 最少領域のサイズ
		uint cellSize() const
		{
			return size().x>>LEVEL;
		}
		/// 最大領域のサイズ
		uint maxCellSize() const
		{
			return size().x>>(LEVEL/2);
		}

		/// ピクセル単位をブロック単位に
		uint toBlockSize(uint pixelsize)
		{
			return (pixelsize+cellSize()-1)/cellSize();
		}

		/// テクスチャ領域のセットアップ（createOnDB等用）
		HRslt setUp(const _TCHAR *name) { return setUp(); }

		/// テクスチャ領域のセットアップ
		HRslt setUp();

		bool isCached(const Handle<Font> font, int c) const;

		FontGlyph find(const Handle<Font> font, int c) const;

		/// Ascii文字を一度にキャッシュ
		void cacheAscii(const Handle<Font> font);

		/// 文字キャッシュ開始
		void begin();

		/// 文字をキャッシュ
		void cache(const Handle<Font> font, int c);

		/// 文字キャッシュ終了
		void end();

		/// すべて開放
		void clear();

		/// アスキー文字が収まる最少の幅を計算
		static uint getSmallestWidth(const Font &font, uint unit);

		/// 領域のガベージ
		void gabage();

	GCTP_DECLARE_CLASS

	private:
		HRslt setUpAscii(detail::AsciiAttr &attr, const Font &font, ulong width, ulong height, int ofsx = 0, int ofsy = 0);
		int alloc(int level);
		std::auto_ptr<detail::DrawContext> cntx_;
		boost::scoped_ptr<detail::FontTextureDetail> detail_;
		//float scale_;
	};

	/// FontTextureを使った文字表示
	void drawText(SpriteBuffer &spr, FontTexture &font, const Handle<Font> cfont, const Point2f &pos, Color32 color, const _TCHAR *_text);

	/// タブ境界に整列
	inline float toTabbed(float x, int tabwidth) { return static_cast<float>(static_cast<int>(x)/tabwidth*tabwidth+tabwidth); }

	/// 文字列中の指定文字の位置を取得
	Point2f getTextPos(const FontTexture &font, const Handle<Font> cfont, const _TCHAR *text, uint idx);

	/** 複数のフォントテクスチャをまとめて管理する
	 *
	 * 現在使われていない(できてないから)
	 * @todo これを動作させる
	 * @todo と言うか、これ必要か？
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/20 11:20:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class FontTextureSet : public Object {
	public:
		/// コンストラクタ
		FontTextureSet();
		/// フォントを追加
		HRslt add(const Handle<Font> font);
		/// フォントテクスチャを取得
		Handle<FontTexture> get(const Handle<Font> font);
	GCTP_DECLARE_FACTORY;
	private:
		boost::scoped_ptr<detail::FontTextureDetail> detail_;
	};

}} //namespace gctp

#endif //_GCTP_GRAPHIC_FONTTEXTURE_HPP_