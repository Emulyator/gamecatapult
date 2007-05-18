#ifndef _GCTP_GRAPHIC_HPP_
#define _GCTP_GRAPHIC_HPP_
/** @file
 * GameCatapult DirectX Graphicクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * $Id: graphic.hpp,v 1.4 2005/07/01 05:13:06 takasugi Exp $
 *
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/com_ptr.hpp>
#include <gctp/types.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/db.hpp>
#include <gctp/object.hpp>
#include <gctp/matrix.hpp>
#include <iosfwd>
#include <vector>
#include <stack>

struct IDirect3DDevice9;

namespace gctp {
	struct Color;
	/** グラフィック関連のモジュール
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/12/05 13:08:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	namespace graphic {
		struct DirectionalLight;
		struct PointLight;
		struct SpotLight;
		struct Material;
	}
}

namespace gctp { namespace graphic {

	namespace dx {
		class Device;
		class StateBlock;
		class View;
	}

	/** ビューポート構造体
	 *
	 * コンストラクタでいろいろ簡易設定できる
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/25 6:41:56
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	struct ViewPort {
		uint x;
		uint y;
		uint width;
		uint height;
		float min_z;
		float max_z;
		ViewPort() {}
		ViewPort(uint _x, uint _y, uint _width, uint _height, float _min_z, float _max_z)
			: x(_x), y(_y), width(_width), height(_height), min_z(_min_z), max_z(_max_z) {}
		ViewPort(uint _width, uint _height, float _min_z = 0.0f, float _max_z = 1.0f)
			: x(0), y(0), width(_width), height(_height), min_z(_min_z), max_z(_max_z) {}
		ViewPort(const Point2 &_pos, const Point2 &_size)
			: x(_pos.x), y(_pos.y), width(_size.x), height(_size.y), min_z(0.0f), max_z(1.0f) {}
		Point2 size() const { return Point2C(width, height); }
		float aspectRatio() const { return (float)width/(float)height; }
	};

	class DeviceImpl;
	class Rsrc;
	/** グラフィックＡＰＩクラス
	 *
	 * Direct3Dデバイスをあらわすクラス。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/26 22:52:23
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Device {
	public:
		Device(/** 同じアダプタに対して製作する時、追加スワップチェーンでなく別デバイスにするか？*/bool is_unique = false);
		HRslt open(HWND h_focus_wnd);
		HRslt open(HWND h_focus_wnd, uint adpt, uint mode);
		HRslt open(HWND h_focus_wnd, HWND h_device_wnd);
		HRslt open(HWND h_focus_wnd, HWND h_device_wnd, uint adpt, uint mode);
		HRslt open(HWND h_focus_wnd, HWND h_first_wnd, HWND h_second_wnd, uint adpt, uint mode);
		HRslt open(uint adpt, uint idx);
		HRslt open(IDirect3DDevice9 *device);
		void close();

		/// オープン済みか？
		bool isOpen() const;

		/// 描画開始
		HRslt begin();
		/// 描画終了
		HRslt end();
		/// 描画反映
		HRslt present();
		/// 画面のクリア
		HRslt clear(bool target = true, bool zbuffer = true);

		/// ワールドマトリクス設定
		void setWorld(const Matrix &mat);
		/// ワールドマトリクス設定
		Matrix getWorld();
		/// 投影マトリクス設定
		void setProjection(const Matrix &mat);
		/// 投影マトリクス取得
		Matrix getProjection();
		/// ビューマトリクス設定
		void setView(const Matrix &mat);
		/// ビューマトリクス取得
		Matrix getView();

		/// グローバルアンビエントライトを設定
		HRslt setAmbient(const Color &col);
		/// グローバルアンビエントライトを取得
		const Color &getAmbient() const;
		/// ライトを追加
		HRslt pushLight(const DirectionalLight &lgt);
		/// ライトを追加
		HRslt pushLight(const PointLight &lgt);
		/// ライトを追加
		HRslt pushLight(const SpotLight &lgt);
		/// ライトをポップ
		HRslt popLight();
		/// ライトを全消去
		void clearLight();
		/// 現在アクティブなライトの数を返す
		uint lightNum() const;
		/// ライトを追加
		HRslt getLight(uint no, DirectionalLight &lgt) const;
		/// ライトを追加
		HRslt getLight(uint no, PointLight &lgt) const;
		/// ライトを追加
		HRslt getLight(uint no, SpotLight &lgt) const;

		/// マテリアル適用
		void setMaterial(const Material &mtrl);

		/// 深度テスト比較方法
		enum ZFunc {
			NONE = D3DCMP_NEVER,
			LT   = D3DCMP_LESS,
			EQ   = D3DCMP_EQUAL,
			LTEQ = D3DCMP_LESSEQUAL,
			GT   = D3DCMP_GREATER,
			NTEQ = D3DCMP_NOTEQUAL,
			GTEQ = D3DCMP_GREATEREQUAL,
			ALL  = D3DCMP_ALWAYS
		};
		/** 深度バッファの設定変更
		 * ※実装されていない
		 */
		HRslt setDepthBufferState(bool enable, bool writable, ZFunc zfunc);
		// これ全部ラップするのめんどいな…

		/// デバイス消失時のリソース解放
		void cleanUp();
		/// デバイス消失時のリソース再構築
		void restore();

		/// スクリーンサイズを返す
		Point2 getScreenSize() const;

		/// クリアするときの色を設定
		void setClearColor(const Color &cc);
	
		/// ビューポートを取得
		ViewPort getViewPort() const;
		/// ビューポートを設定
		HRslt setViewPort(const ViewPort &vp);

		/// マウスカーソルの表示･非表示を設定
		void setCursorVisible(bool yes);

		/// ステートブロック記録開始
		HRslt beginRecord();
		/// ステートブロック記録終了
		Pointer<dx::StateBlock> endRecord();
		/// 新規ステートブロック
		Pointer<dx::StateBlock> createState(D3DSTATEBLOCKTYPE type);
		// ステートスタックの仕組みさえあれば十分では？
		// 確かに、ステートの変更は少ないほうが良く、変更するならステートブロックを使って
		// 一括で設定したほうがいいのだろうけど、DirectXに密着した仕様だし、苦労の割に
		// 劇的にパフォーマンスが上がるわけではないと思う。こんなことに苦労するくらいなら
		// 全アーキテクチャで有効なテクスチャ切り替えの回数、プリミティブ描画命令の発行回数
		// を減らすことを考えた方がいい。
		// とおもったが、やるなら昔あったStateBlockRsrcを復活するしかない
		// StateBlockはデバイス消失時の明示再構築の対象だ。めどいんで放置

//		/// 現在のステートを退避
//		HRslt pushState();
//		/// 一個前のステートに復帰
//		HRslt popState();

		/** カレントオーディオデバイスの取得
		 *
		 * カレントデバイスを返す。
		 */
		inline static Device *current() { return current_; }
		/** カレントオーディオデバイスを設定
		 *
		 * カレントデバイスを設定する。
		 */
		HRslt setCurrent() const;

		/** 実装オブジェクトを返す。
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/07/28 3:49:23
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		dx::Device &impl();

		/** 実装オブジェクトを返す。
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/07/28 3:49:23
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		const dx::Device &impl() const;

		/** リソース登録
		 *
		 * newRsrc経由、Realizer経由なら自動で行われるので、これを使う必要なし
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/01/26 0:39:42
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		void registerRsrc(Handle<Rsrc> rsrc);

		/** リソースデータベース
		 *
		 * いつかなくす
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/01/28 15:20:16
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		const DB &db() const;

		/** リソースデータベース
		 *
		 * いつかなくす
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/01/28 15:20:16
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		DB &db();

	private:
		bool is_unique_;	///< 同じアダプタに対して製作する時、追加スワップチェーンでなく別デバイスにするか？
		HRslt preSetUp(uint adpt, HWND hwnd);
		Pointer<DeviceImpl> impl_;
		Pointer<dx::View> view_;
		GCTP_TLS static Device* current_;	///< カレントデバイスインスタンス
	};

	/** グラフィックデバイスの取得
	 *
	 * カレントデバイスのインスタンスを返す。
	 */
	inline Device &device() { return *Device::current(); }

	/** グラフィックシステム全体の初期化。
	 *
	 * これを必ずやっておく必要がある
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/26 1:14:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void initialize(bool is_no_WHQL = false);

	/** @defgroup CurrentGraphicDeviceOperation カレントに対する操作 */
	/* @{ */
	/// 描画開始
	inline HRslt begin()
	{
		return device().begin();
	}
	/// 描画終了
	inline HRslt end()
	{
		return device().end();
	}
	/// 描画反映
	inline HRslt present()
	{
		return device().present();
	}
	/// 画面のクリア
	inline HRslt clear(bool target = true, bool zbuffer = true)
	{
		return device().clear(target, zbuffer);
	}
	/// 画面の消去に使う色を設定
	inline void setClearColor(const Color &cc)
	{
		device().setClearColor(cc);
	}
	/// ワールドマトリクス設定
	inline void setWorld(const Matrix &mat)
	{
		device().setWorld(mat);
	}
	/// ワールドマトリクス取得
	inline Matrix getWorld()
	{
		return device().getWorld();
	}
	/// 投影マトリクス設定
	inline void setProjection(const Matrix &mat)
	{
		device().setProjection(mat);
	}
	/// 投影マトリクス取得
	inline Matrix getProjection()
	{
		return device().getProjection();
	}
	/// ビューマトリクス設定
	inline void setView(const Matrix &mat)
	{
		device().setView(mat);
	}
	/// ビューマトリクス取得
	inline Matrix getView()
	{
		return device().getView();
	}
	/// スクリーンサイズを返す
	inline Point2 getScreenSize()
	{
		return device().getScreenSize();
	}
	
	/// ビューポートを設定
	inline HRslt setViewPort(const ViewPort &src)
	{
		return device().setViewPort(src);
	}

	/// ビューポートを取得
	inline ViewPort getViewPort()
	{
		return device().getViewPort();
	}

	/// マウスカーソルの表示・非表示を設定
	inline void setCursorVisible(bool yes)
	{
		device().setCursorVisible(yes);
	}

	/// 環境光を設定
	inline HRslt setAmbient(const Color &col)
	{
		return device().setAmbient(col);
	}

	/// 環境光を設定
	inline const Color &getAmbient()
	{
		return device().getAmbient();
	}

	/// 平行光源を追加
	inline HRslt pushLight(const DirectionalLight &lgt)
	{
		return device().pushLight(lgt);
	}

	/// 点光源を追加
	inline HRslt pushLight(const PointLight &lgt)
	{
		return device().pushLight(lgt);
	}

	/// スポットライトを追加
	inline HRslt pushLight(const SpotLight &lgt)
	{
		return device().pushLight(lgt);
	}

	/// 一番最近追加したライトを削除
	inline HRslt popLight()
	{
		return device().popLight();
	}

	/// すべてのライトを削除
	inline void clearLight()
	{
		device().clearLight();
	}

	/// 平行光源を追加
	inline HRslt getLight(uint no, DirectionalLight &lgt)
	{
		return device().getLight(no, lgt);
	}

	/// 点光源を追加
	inline HRslt getLight(uint no, PointLight &lgt)
	{
		return device().getLight(no, lgt);
	}

	/// スポットライトを追加
	inline HRslt getLight(uint no, SpotLight &lgt)
	{
		return device().getLight(no, lgt);
	}

	/// 現在アクティブなライト数
	inline uint lightNum()
	{
		return device().lightNum();
	}

	/// マテリアル適用
	inline void setMaterial(const Material &mtrl)
	{
		device().setMaterial(mtrl);
	}

	/// ステートブロック記録開始
	inline HRslt beginRecord()
	{
		return device().beginRecord();
	}

	/// ステートブロック記録終了
	inline Pointer<dx::StateBlock> endRecord()
	{
		return device().endRecord();
	}

	/// ステートブロック作成
	inline Pointer<dx::StateBlock> createState(D3DSTATEBLOCKTYPE type)
	{
		return device().createState(type);
	}
	
//	/// 現在のステートを退避
//	inline HRslt pushState()
//	{
//		return device().pushState();
//	}

//	/// 一個前のステートに復帰
//	inline HRslt popState()
//	{
//		return device().popState();
//	}

	/** データベースに登録しつつ製作
	 *
	 * すでにデータベースにあるならそれを返し、そうで無かったら新規製作しＤＢに登録する、という
	 * 操作のテンプレート
	 *
	 * いつかなくす
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/16 14:03:54
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T>
	Pointer<_T> createOnDB(const _TCHAR *name)
	{
		if(name) {
			Handle<_T> h = device().db()[name];
			if(h) return h.get();
			else {
				Pointer<_T> ret = new _T;
				if(ret) {
					if(ret->setUp(name)) {
						PRNN(_T("リソース'")<<name<<_T("'を製作"));
						h = ret;
						device().db().insert(name, h);
						return ret;
					}
				}
				PRNN(_T("リソース'")<<name<<_T("'の制作に失敗"));
			}
		}
		else PRNN(_T("リソースの製作に失敗。名前を指定してください"));
		return 0;
	}
	/*@}*/

	/** @defgroup GraphicSystemSetting システム設定に対するコントロール */
	/* @{ */

	/// フルシーンアンチエイリアスの種類
	enum FSAAType {
		FSAA_NONE, ///< なし
		FSAA_MSx2, ///< ２サンプルのマルチサンプリング
		FSAA_MSx3, ///< ３サンプルのマルチサンプリング
		FSAA_MSx4, ///< ４サンプルのマルチサンプリング
	};
	/** フルシーンアンチエイリアスをするか？
	 *
	 * デフォルトでFSAA_NONE。
	 * openする前にこれを呼び出す必要がある。
	 * 
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/01/04 20:25:13
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	void allowFSAA(FSAAType type, uint level);

	/** 厳密なマルチスレッド対応モードにするか？
	 *
	 * デフォルトでfalse。
	 * openする前にこれを呼び出す必要がある。
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/01/04 20:25:13
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	void allowStrictMultiThreadSafe(bool yes);

	/** Zバッファを自動で製作するか？
	 *
	 * デフォルトでtrue。
	 * openする前にこれを呼び出す必要がある。
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/01/04 20:24:41
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	void allowAutoDepthBuffer(bool yes);
	
	/** HALが獲得できなかった場合、リファレンスドライバーを使用するか？
	 *
	 * デフォルトでfalse。
	 * openする前にこれを呼び出す必要がある。
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/01/04 20:24:41
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	void allowReferenceDriver(bool yes);
	
	/** ハードウェア頂点処理を使用するか？
	 *
	 * デフォルトでfalse。
	 * openする前にこれを呼び出す必要がある。
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/01/04 20:24:41
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	void allowHardwareVertexProcessing(bool yes);
	
	/** ハードウェア・ソフトウェア混合頂点処理を使用するか？
	 *
	 * デフォルトでtrue。
	 * openする前にこれを呼び出す必要がある。
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/01/04 20:24:41
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	void allowMixedVertexProcessing(bool yes);

	/** （フルスクリーン時の）画面更新間隔設定
	 *
	 * デフォルトで0。(VSyncをとらない)
	 * １でVsyncをとる。2でVsync２回に一回フリップ。3でVsync３回に一回フリップ。
	 * 0～4の値を指定。
	 * openする前にこれを呼び出す必要がある。
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/01/04 20:24:41
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	void setIntervalTime(int time);

	/** スワップエフェクトの設定
	 *
	 * デフォルトでfalse(D3DSWAPEFFECT_DISCARD)
	 *
	 * trueにするとD3DSWAPEFFECT_COPYを使用する。
	 *
	 * openする前にこれを呼び出す必要がある。
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/01/04 20:24:41
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	void setSwapMode(bool copy);
	/* @} */

}} // namespace gctp

#endif //_GCTP_GRAPHIC_HPP_