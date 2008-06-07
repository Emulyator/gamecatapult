#ifndef _GCTP_GRAPHIC_DX_DEVICE_HPP_
#define _GCTP_GRAPHIC_DX_DEVICE_HPP_
/** @file
 * GameCatapult DirectX Graphicクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/16 15:28:55
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <d3d9.h>
#include <gctp/types.hpp>
#include <gctp/hrslt.hpp>
#include <gctp/com_ptr.hpp>
#include <gctp/object.hpp>
#include <gctp/matrix.hpp>
#include <gctp/color.hpp>
#include <gctp/handlelist.hpp>
#include <gctp/graphic/dx/stateblock.hpp>
#include <gctp/graphic/dx/view.hpp>
#include <iosfwd>
#include <vector>
//#include <stack>

namespace gctp {
	namespace graphic {
		struct DirectionalLight;
		struct PointLight;
		struct SpotLight;
		struct Material;
	}
}

namespace gctp { namespace graphic { namespace dx {

	/** DirextGraphic初期化
	 *
	 * 具体的には、IDirect3D9を製作して、デバイスの列挙。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/26 1:12:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void initialize(bool with_WHQL = false /**< WHQLありでデバイス列挙するか？（デフォルトfalse） */);

#ifdef GCTP_LITE
	typedef D3DADAPTER_IDENTIFIER8 D3DADAPTER_IDENTIFIER;
#else
	typedef D3DADAPTER_IDENTIFIER9 D3DADAPTER_IDENTIFIER;
#endif

	/// グラフィックアダプタ情報クラス
	class Adapter : public D3DADAPTER_IDENTIFIER {
	public:
		typedef std::vector<Adapter> AdapterList;
		std::vector<D3DDISPLAYMODE>	modes;
		static const AdapterList &adapters() { return adapters_;}
		static void initialize(bool with_WHQL);
	private:
		friend class Device;
		void set(UINT adpt, bool with_WHQL=false);
		static IDirect3DPtr api_;
		static AdapterList adapters_;	///< デバイス列挙情報
	};

	/// デバイス情報
	typedef std::vector<Adapter> AdapterList;
	inline const AdapterList &adapters() { return Adapter::adapters(); }

	/** グラフィックＡＰＩクラス
	 *
	 * Direct3Dデバイスをあらわすクラス。
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/24 17:23:09
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Device : public Object {
	public:
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
		static void allowFSAA(FSAAType type, uint level);

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
		static void allowStrictMultiThreadSafe(bool yes);

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
		static void allowAutoDepthBuffer(bool yes);
		
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
		static void allowReferenceDriver(bool yes);
		
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
		static void allowHardwareVertexProcessing(bool yes);
		
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
		static void allowMixedVertexProcessing(bool yes);

		/** （フルスクリーン時の）画面更新間隔設定
		 *
		 * デフォルトで0。(VSyncをとらない)
		 * １でVsyncをとる。2でVsync２回に一回フリップ。3ででVsync３回に一回フリップ。
		 * 0～4の値を指定。
		 * openする前にこれを呼び出す必要がある。
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/01/04 20:24:41
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		static void setIntervalTime(int time);

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
		static void setSwapMode(bool copy);

		Device();
		~Device();

		/// DirectGraphicを指定のウィンドウにバインド
		HRslt open(HWND h_focus_wnd, HWND h_device_wnd, bool is_fs, uint adpt, uint mode, D3DDEVTYPE type = D3DDEVTYPE_HAL);
		/// DirectGraphicを指定のウィンドウにバインド
		HRslt open(HWND h_focus_wnd, HWND h_first_wnd, HWND h_second_wnd, uint adpt, uint mode, D3DDEVTYPE type = D3DDEVTYPE_HAL);

		/// ウィンドウモードで
		HRslt open(HWND h_focus_wnd)
		{
			return open(h_focus_wnd, h_focus_wnd, false, 0, 0);
		}
		/// ウィンドウモードで
		HRslt open(HWND h_focus_wnd, HWND h_device_wnd)
		{
			return open(h_focus_wnd, h_device_wnd, false, 0, 0);
		}
		/// フルスクリーンで
		HRslt open(HWND h_focus_wnd, uint adpt, uint mode)
		{
			return open(h_focus_wnd, h_focus_wnd, true, adpt, mode);
		}
		/// フルスクリーンで
		HRslt open(HWND h_focus_wnd, HWND h_device_wnd, uint adpt, uint mode)
		{
			return open(h_focus_wnd, h_device_wnd, true, adpt, mode);
		}

		/// 既存のデバイスにアタッチ
		HRslt open(IDirect3DDevice9 *device);

		void close();

		/** バインドされているウィンドウを返す。
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/07/28 3:49:23
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		HWND hwnd()
		{
			D3DDEVICE_CREATION_PARAMETERS parameters;
			ptr_->GetCreationParameters(&parameters);
			return parameters.hFocusWindow;
		}

		IDirect3DDevicePtr ptr() const { return ptr_; }

		/// カラーフォーマット検証
		HRslt checkFormat(DWORD usage, D3DRESOURCETYPE rtype, D3DFORMAT format);

	private:
		IDirect3DDevicePtr ptr_;				///< デバイスのハンドル

		static uint fsaa_level_;
		static FSAAType fsaa_;
		static bool is_multihead_;				///< マルチヘッドモードか？
		static bool is_threaded_;				///< マルチスレッド対応で初期化するか？
		static bool is_autodepthbuffer_;		///< 深度バッファ付きで初期化するか？
		static bool is_allow_REF_;				///< リファレンスドライバも試すか？
		static bool is_allow_HVP_;				///< ハードウェア頂点処理を試すか？
		static bool is_allow_MVP_;				///< ハードウェア・ソフトウェア混合頂点処理を試すか？
		static bool copy_when_swap_;			///< 更新モード
		static int interval_time_;				///< 更新レート

	public:
		IDirect3DDevice *operator->() const {
			if(this) return ptr_;
			return NULL;
		}
		operator IDirect3DDevice *() const {
			if(this) return ptr_;
			return NULL;
		}

		HRslt setCurrent() const;

		/// 描画開始
		HRslt begin()
		{
			if(ptr_) return ptr_->BeginScene();
			return E_POINTER;
		}
		/// 描画終了
		HRslt end()
		{
			if(ptr_) return ptr_->EndScene();
			return E_POINTER;
		}
		/// 描画反映
		HRslt present();
		/// 画面のクリア
		HRslt clear(DWORD flag = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);

		/// ワールドマトリクス設定
		inline void setWorld(const Matrix &mat) {
			ptr_->SetTransform( D3DTS_WORLD, mat );
		}
		/// ワールドマトリクス設定
		inline Matrix getWorld() {
			Matrix ret;
			ptr_->GetTransform( D3DTS_WORLD, ret );
			return ret;
		}
		/// 投影マトリクス設定
		inline void setProjection(const Matrix &mat) {
			ptr_->SetTransform( D3DTS_PROJECTION, mat );
		}
		/// 投影マトリクス設定
		inline Matrix getProjection() {
			Matrix ret;
			ptr_->GetTransform( D3DTS_PROJECTION, ret );
			return ret;
		}
		/// ビューマトリクス設定
		inline void setView(const Matrix &mat) {
			ptr_->SetTransform( D3DTS_VIEW, mat );
		}
		/// ビューマトリクス設定
		inline Matrix getView() {
			Matrix ret;
			ptr_->GetTransform( D3DTS_VIEW, ret );
			return ret;
		}

		/// デバイス消失時のリソース解放
		void cleanUp();
		/// デバイス消失時のリソース再構築
		void restore();

		/// リソース登録
		void registerRsrc(Handle<Rsrc> rsrc);

		/// スクリーンサイズを返す
		Point2 getScreenSize() const;

		/// ビューポートを取得
		D3DVIEWPORT getViewPort() const;

		/// ビューポートを設定
		HRslt setViewPort(const D3DVIEWPORT &vp);

		/// クリアするときの色を設定
		void setClearColor(const Color &cc);
		
		/// マウスカーソルの表示･非表示を設定
		void setCursorVisible(bool yes);
		/// マウスカーソルの位置を設定
		void setCursorPosition(const Point2 &pos);

		/// 扱える最大のライト数を取得
		uint maxLightNum() const;
		/// グローバルアンビエントライトを設定
		HRslt setAmbient(const Color &col);
		/// グローバルアンビエントライトを取得
		const Color &getAmbient() const { return ambient_light_; }
		/// ライトを追加
		HRslt pushLight(const D3DLIGHT &lgt);
		/// ライトを追加
		HRslt pushLight(const DirectionalLight &lgt);
		/// ライトを追加
		HRslt pushLight(const PointLight &lgt);
		/// ライトを追加
		HRslt pushLight(const SpotLight &lgt);
		/// 最後に追加されたライトを消去
		HRslt popLight();
		/// ライトを全消去
		void clearLight();
		/// ライトを取得
		HRslt getLight(uint no, D3DLIGHT &lgt) const;
		/// ライトを取得
		HRslt getLight(uint no, DirectionalLight &lgt) const;
		/// ライトを取得
		HRslt getLight(uint no, PointLight &lgt) const;
		/// ライトを取得
		HRslt getLight(uint no, SpotLight &lgt) const;
		/// 現在のライト数を取得
		uint lightNum() const { return light_num_; }

		/// マテリアル適用
		void setMaterial(const Material &mtrl);

		/// ステートブロック記録開始
		HRslt beginRecord();
		/// ステートブロック記録終了
		Pointer<StateBlock> endRecord();
		/// 新規ステートブロック
		Pointer<StateBlock> createState(D3DSTATEBLOCKTYPE type = D3DSBT_ALL);
//		/// 現在のステートを退避
//		HRslt pushState(D3DSTATEBLOCKTYPE type = D3DSBT_ALL);
//		/// 一個前のステートに復帰
//		HRslt popState();

		/// 新規ビュー（追加スワップチェーン）
		Pointer<View> createView(uint adpt, HWND hwnd);
		/// 新規ビュー（非明示スワップチェーン）
		Pointer<View> createView(uint adpt, uint idx);

	private:
		Color clear_color_;
		HCURSOR cursor_backup_; // マウスカーソルのバックアップ
		Color ambient_light_;
		uint light_num_;
		typedef HandleList<Rsrc> RsrcList;
		RsrcList rsrcs_;
#if 0
		// これ、役に立たねえ
		// 使うな！
		class StateStack : public std::vector< StateBlock > {
		public:
			StateStack() {}
			HRslt push(IDirect3DDevicePtr dev, D3DSTATEBLOCKTYPE type = D3DSBT_ALL) {
				resize(size()+1);
				if(!back() && !back().setUp(dev, type)) return E_FAIL;
				return back().capture();
			}
			// 1以下にはならない。最初の一個はデフォルト設定用
			HRslt pop() {
				if(size()>0) {
					HRslt hr = back().apply();
					if(size()>1) resize(size()-1);
					return hr;
				}
				return S_FALSE;
			}
		};
		StateStack states_;
#endif
	};

	/// カラーフォーマットデバッグ出力
	template<class _T, class _E>
	std::basic_ostream<_T, _E> &operator<<(std::basic_ostream<_T, _E> &os, const D3DFORMAT fmt)
	{
		switch(fmt) {
#define case_n_PRINT_FORMAT(_f) case _f: os<<#_f; break
		case_n_PRINT_FORMAT(D3DFMT_UNKNOWN              );
		case_n_PRINT_FORMAT(D3DFMT_R8G8B8               );
		case_n_PRINT_FORMAT(D3DFMT_A8R8G8B8             );
		case_n_PRINT_FORMAT(D3DFMT_X8R8G8B8             );
		case_n_PRINT_FORMAT(D3DFMT_R5G6B5               );
		case_n_PRINT_FORMAT(D3DFMT_X1R5G5B5             );
		case_n_PRINT_FORMAT(D3DFMT_A1R5G5B5             );
		case_n_PRINT_FORMAT(D3DFMT_A4R4G4B4             );
		case_n_PRINT_FORMAT(D3DFMT_R3G3B2               );
		case_n_PRINT_FORMAT(D3DFMT_A8                   );
		case_n_PRINT_FORMAT(D3DFMT_A8R3G3B2             );
		case_n_PRINT_FORMAT(D3DFMT_X4R4G4B4             );
		case_n_PRINT_FORMAT(D3DFMT_A2B10G10R10          );
		case_n_PRINT_FORMAT(D3DFMT_A8B8G8R8             );
		case_n_PRINT_FORMAT(D3DFMT_X8B8G8R8             );
		case_n_PRINT_FORMAT(D3DFMT_G16R16               );
		case_n_PRINT_FORMAT(D3DFMT_A2R10G10B10          );
		case_n_PRINT_FORMAT(D3DFMT_A16B16G16R16         );
		case_n_PRINT_FORMAT(D3DFMT_A8P8                 );
		case_n_PRINT_FORMAT(D3DFMT_P8                   );
		case_n_PRINT_FORMAT(D3DFMT_L8                   );
		case_n_PRINT_FORMAT(D3DFMT_A8L8                 );
		case_n_PRINT_FORMAT(D3DFMT_A4L4                 );
		case_n_PRINT_FORMAT(D3DFMT_V8U8                 );
		case_n_PRINT_FORMAT(D3DFMT_L6V5U5               );
		case_n_PRINT_FORMAT(D3DFMT_X8L8V8U8             );
		case_n_PRINT_FORMAT(D3DFMT_Q8W8V8U8             );
		case_n_PRINT_FORMAT(D3DFMT_V16U16               );
		case_n_PRINT_FORMAT(D3DFMT_A2W10V10U10          );
		case_n_PRINT_FORMAT(D3DFMT_UYVY                 );
		case_n_PRINT_FORMAT(D3DFMT_R8G8_B8G8            );
		case_n_PRINT_FORMAT(D3DFMT_YUY2                 );
		case_n_PRINT_FORMAT(D3DFMT_G8R8_G8B8            );
		case_n_PRINT_FORMAT(D3DFMT_DXT1                 );
		case_n_PRINT_FORMAT(D3DFMT_DXT2                 );
		case_n_PRINT_FORMAT(D3DFMT_DXT3                 );
		case_n_PRINT_FORMAT(D3DFMT_DXT4                 );
		case_n_PRINT_FORMAT(D3DFMT_DXT5                 );
		case_n_PRINT_FORMAT(D3DFMT_D16_LOCKABLE         );
		case_n_PRINT_FORMAT(D3DFMT_D32                  );
		case_n_PRINT_FORMAT(D3DFMT_D15S1                );
		case_n_PRINT_FORMAT(D3DFMT_D24S8                );
		case_n_PRINT_FORMAT(D3DFMT_D24X8                );
		case_n_PRINT_FORMAT(D3DFMT_D24X4S4              );
		case_n_PRINT_FORMAT(D3DFMT_D16                  );
		case_n_PRINT_FORMAT(D3DFMT_D32F_LOCKABLE        );
		case_n_PRINT_FORMAT(D3DFMT_D24FS8               );
		case_n_PRINT_FORMAT(D3DFMT_VERTEXDATA           );
		case_n_PRINT_FORMAT(D3DFMT_INDEX16              );
		case_n_PRINT_FORMAT(D3DFMT_INDEX32              );
		case_n_PRINT_FORMAT(D3DFMT_Q16W16V16U16         );
		case_n_PRINT_FORMAT(D3DFMT_MULTI2_ARGB8         );
		case_n_PRINT_FORMAT(D3DFMT_R16F                 );
		case_n_PRINT_FORMAT(D3DFMT_G16R16F              );
		case_n_PRINT_FORMAT(D3DFMT_A16B16G16R16F        );
		case_n_PRINT_FORMAT(D3DFMT_R32F                 );
		case_n_PRINT_FORMAT(D3DFMT_G32R32F              );
		case_n_PRINT_FORMAT(D3DFMT_A32B32G32R32F        );
		case_n_PRINT_FORMAT(D3DFMT_CxV8U8               );
#undef case_n_PRINT_FORMAT
		default: os << "D3DFMT?:" << (int)fmt;
		}
		return os;
	}

	/// アダプタ情報デバッグ出力
	template<class _T, class _E>
	std::basic_ostream<_T, _E> &operator<<(std::basic_ostream<_T, _E> &os, const Adapter &adapter)
	{
		os<<"driver : "<<adapter.Driver<<endl;
		os<<"caption : "<<adapter.Description<<endl;
		os<<"screen descs : ";
		for(uint i = 0; i < adapter.modes.size(); i++) {
			os<<i<<": "<<adapter.modes[i].Width<<"x"<<adapter.modes[i].Height<<" "<<adapter.modes[i].Format;
			if(adapter.modes[i].RefreshRate==D3DPRESENT_RATE_DEFAULT) os<<":DEFAULT  ";
			else os<<":"<<adapter.modes[i].RefreshRate<<"Hz  ";
		}
		os<<endl;
		return os;
	}

}}} // namespace gctp

#endif //_GCTP_GRAPHIC_DX_DEVICE_HPP_