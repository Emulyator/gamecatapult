#ifndef _GCTP_GRAPHIC_DX_DEVICE_HPP_
#define _GCTP_GRAPHIC_DX_DEVICE_HPP_
/** @file
 * GameCatapult DirectX Graphic�N���X�w�b�_�t�@�C��
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

	/** DirextGraphic������
	 *
	 * ��̓I�ɂ́AIDirect3D9�𐻍삵�āA�f�o�C�X�̗񋓁B
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/26 1:12:32
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void initialize(bool with_WHQL = false /**< WHQL����Ńf�o�C�X�񋓂��邩�H�i�f�t�H���gfalse�j */);

#ifdef GCTP_LITE
	typedef D3DADAPTER_IDENTIFIER8 D3DADAPTER_IDENTIFIER;
#else
	typedef D3DADAPTER_IDENTIFIER9 D3DADAPTER_IDENTIFIER;
#endif

	/// �O���t�B�b�N�A�_�v�^���N���X
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
		static AdapterList adapters_;	///< �f�o�C�X�񋓏��
	};

	/// �f�o�C�X���
	typedef std::vector<Adapter> AdapterList;
	inline const AdapterList &adapters() { return Adapter::adapters(); }

	/** �O���t�B�b�N�`�o�h�N���X
	 *
	 * Direct3D�f�o�C�X������킷�N���X�B
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/24 17:23:09
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Device : public Object {
	public:
		/// �t���V�[���A���`�G�C���A�X�̎��
		enum FSAAType {
			FSAA_NONE, ///< �Ȃ�
			FSAA_MSx2, ///< �Q�T���v���̃}���`�T���v�����O
			FSAA_MSx3, ///< �R�T���v���̃}���`�T���v�����O
			FSAA_MSx4, ///< �S�T���v���̃}���`�T���v�����O
		};
		/** �t���V�[���A���`�G�C���A�X�����邩�H
		 *
		 * �f�t�H���g��FSAA_NONE�B
		 * open����O�ɂ�����Ăяo���K�v������B
		 * 
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/01/04 20:25:13
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		static void allowFSAA(FSAAType type, uint level);

		/** �����ȃ}���`�X���b�h�Ή����[�h�ɂ��邩�H
		 *
		 * �f�t�H���g��false�B
		 * open����O�ɂ�����Ăяo���K�v������B
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/01/04 20:25:13
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		static void allowStrictMultiThreadSafe(bool yes);

		/** Z�o�b�t�@�������Ő��삷�邩�H
		 *
		 * �f�t�H���g��true�B
		 * open����O�ɂ�����Ăяo���K�v������B
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/01/04 20:24:41
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		static void allowAutoDepthBuffer(bool yes);
		
		/** HAL���l���ł��Ȃ������ꍇ�A���t�@�����X�h���C�o�[���g�p���邩�H
		 *
		 * �f�t�H���g��false�B
		 * open����O�ɂ�����Ăяo���K�v������B
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/01/04 20:24:41
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		static void allowReferenceDriver(bool yes);
		
		/** �n�[�h�E�F�A���_�������g�p���邩�H
		 *
		 * �f�t�H���g��false�B
		 * open����O�ɂ�����Ăяo���K�v������B
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/01/04 20:24:41
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		static void allowHardwareVertexProcessing(bool yes);
		
		/** �n�[�h�E�F�A�E�\�t�g�E�F�A�������_�������g�p���邩�H
		 *
		 * �f�t�H���g��true�B
		 * open����O�ɂ�����Ăяo���K�v������B
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/01/04 20:24:41
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		static void allowMixedVertexProcessing(bool yes);

		/** �i�t���X�N���[�����́j��ʍX�V�Ԋu�ݒ�
		 *
		 * �f�t�H���g��0�B(VSync���Ƃ�Ȃ�)
		 * �P��Vsync���Ƃ�B2��Vsync�Q��Ɉ��t���b�v�B3�ł�Vsync�R��Ɉ��t���b�v�B
		 * 0�`4�̒l���w��B
		 * open����O�ɂ�����Ăяo���K�v������B
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/01/04 20:24:41
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		static void setIntervalTime(int time);

		/** �X���b�v�G�t�F�N�g�̐ݒ�
		 *
		 * �f�t�H���g��false(D3DSWAPEFFECT_DISCARD)
		 *
		 * true�ɂ����D3DSWAPEFFECT_COPY���g�p����B
		 *
		 * open����O�ɂ�����Ăяo���K�v������B
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/01/04 20:24:41
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		static void setSwapMode(bool copy);

		Device();
		~Device();

		/// DirectGraphic���w��̃E�B���h�E�Ƀo�C���h
		HRslt open(HWND h_focus_wnd, HWND h_device_wnd, bool is_fs, uint adpt, uint mode, D3DDEVTYPE type = D3DDEVTYPE_HAL);
		/// DirectGraphic���w��̃E�B���h�E�Ƀo�C���h
		HRslt open(HWND h_focus_wnd, HWND h_first_wnd, HWND h_second_wnd, uint adpt, uint mode, D3DDEVTYPE type = D3DDEVTYPE_HAL);

		/// �E�B���h�E���[�h��
		HRslt open(HWND h_focus_wnd)
		{
			return open(h_focus_wnd, h_focus_wnd, false, 0, 0);
		}
		/// �E�B���h�E���[�h��
		HRslt open(HWND h_focus_wnd, HWND h_device_wnd)
		{
			return open(h_focus_wnd, h_device_wnd, false, 0, 0);
		}
		/// �t���X�N���[����
		HRslt open(HWND h_focus_wnd, uint adpt, uint mode)
		{
			return open(h_focus_wnd, h_focus_wnd, true, adpt, mode);
		}
		/// �t���X�N���[����
		HRslt open(HWND h_focus_wnd, HWND h_device_wnd, uint adpt, uint mode)
		{
			return open(h_focus_wnd, h_device_wnd, true, adpt, mode);
		}

		/// �����̃f�o�C�X�ɃA�^�b�`
		HRslt open(IDirect3DDevice9 *device);

		void close();

		/** �o�C���h����Ă���E�B���h�E��Ԃ��B
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

		/// �J���[�t�H�[�}�b�g����
		HRslt checkFormat(DWORD usage, D3DRESOURCETYPE rtype, D3DFORMAT format);

	private:
		IDirect3DDevicePtr ptr_;				///< �f�o�C�X�̃n���h��

		static uint fsaa_level_;
		static FSAAType fsaa_;
		static bool is_multihead_;				///< �}���`�w�b�h���[�h���H
		static bool is_threaded_;				///< �}���`�X���b�h�Ή��ŏ��������邩�H
		static bool is_autodepthbuffer_;		///< �[�x�o�b�t�@�t���ŏ��������邩�H
		static bool is_allow_REF_;				///< ���t�@�����X�h���C�o���������H
		static bool is_allow_HVP_;				///< �n�[�h�E�F�A���_�������������H
		static bool is_allow_MVP_;				///< �n�[�h�E�F�A�E�\�t�g�E�F�A�������_�������������H
		static bool copy_when_swap_;			///< �X�V���[�h
		static int interval_time_;				///< �X�V���[�g

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

		/// �`��J�n
		HRslt begin()
		{
			if(ptr_) return ptr_->BeginScene();
			return E_POINTER;
		}
		/// �`��I��
		HRslt end()
		{
			if(ptr_) return ptr_->EndScene();
			return E_POINTER;
		}
		/// �`�攽�f
		HRslt present();
		/// ��ʂ̃N���A
		HRslt clear(DWORD flag = D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);

		/// ���[���h�}�g���N�X�ݒ�
		inline void setWorld(const Matrix &mat) {
			ptr_->SetTransform( D3DTS_WORLD, mat );
		}
		/// ���[���h�}�g���N�X�ݒ�
		inline Matrix getWorld() {
			Matrix ret;
			ptr_->GetTransform( D3DTS_WORLD, ret );
			return ret;
		}
		/// ���e�}�g���N�X�ݒ�
		inline void setProjection(const Matrix &mat) {
			ptr_->SetTransform( D3DTS_PROJECTION, mat );
		}
		/// ���e�}�g���N�X�ݒ�
		inline Matrix getProjection() {
			Matrix ret;
			ptr_->GetTransform( D3DTS_PROJECTION, ret );
			return ret;
		}
		/// �r���[�}�g���N�X�ݒ�
		inline void setView(const Matrix &mat) {
			ptr_->SetTransform( D3DTS_VIEW, mat );
		}
		/// �r���[�}�g���N�X�ݒ�
		inline Matrix getView() {
			Matrix ret;
			ptr_->GetTransform( D3DTS_VIEW, ret );
			return ret;
		}

		/// �f�o�C�X�������̃��\�[�X���
		void cleanUp();
		/// �f�o�C�X�������̃��\�[�X�č\�z
		void restore();

		/// ���\�[�X�o�^
		void registerRsrc(Handle<Rsrc> rsrc);

		/// �X�N���[���T�C�Y��Ԃ�
		Point2 getScreenSize() const;

		/// �r���[�|�[�g���擾
		D3DVIEWPORT getViewPort() const;

		/// �r���[�|�[�g��ݒ�
		HRslt setViewPort(const D3DVIEWPORT &vp);

		/// �N���A����Ƃ��̐F��ݒ�
		void setClearColor(const Color &cc);
		
		/// �}�E�X�J�[�\���̕\�����\����ݒ�
		void setCursorVisible(bool yes);
		/// �}�E�X�J�[�\���̈ʒu��ݒ�
		void setCursorPosition(const Point2 &pos);

		/// ������ő�̃��C�g�����擾
		uint maxLightNum() const;
		/// �O���[�o���A���r�G���g���C�g��ݒ�
		HRslt setAmbient(const Color &col);
		/// �O���[�o���A���r�G���g���C�g���擾
		const Color &getAmbient() const { return ambient_light_; }
		/// ���C�g��ǉ�
		HRslt pushLight(const D3DLIGHT &lgt);
		/// ���C�g��ǉ�
		HRslt pushLight(const DirectionalLight &lgt);
		/// ���C�g��ǉ�
		HRslt pushLight(const PointLight &lgt);
		/// ���C�g��ǉ�
		HRslt pushLight(const SpotLight &lgt);
		/// �Ō�ɒǉ����ꂽ���C�g������
		HRslt popLight();
		/// ���C�g��S����
		void clearLight();
		/// ���C�g���擾
		HRslt getLight(uint no, D3DLIGHT &lgt) const;
		/// ���C�g���擾
		HRslt getLight(uint no, DirectionalLight &lgt) const;
		/// ���C�g���擾
		HRslt getLight(uint no, PointLight &lgt) const;
		/// ���C�g���擾
		HRslt getLight(uint no, SpotLight &lgt) const;
		/// ���݂̃��C�g�����擾
		uint lightNum() const { return light_num_; }

		/// �}�e���A���K�p
		void setMaterial(const Material &mtrl);

		/// �X�e�[�g�u���b�N�L�^�J�n
		HRslt beginRecord();
		/// �X�e�[�g�u���b�N�L�^�I��
		Pointer<StateBlock> endRecord();
		/// �V�K�X�e�[�g�u���b�N
		Pointer<StateBlock> createState(D3DSTATEBLOCKTYPE type = D3DSBT_ALL);
//		/// ���݂̃X�e�[�g��ޔ�
//		HRslt pushState(D3DSTATEBLOCKTYPE type = D3DSBT_ALL);
//		/// ��O�̃X�e�[�g�ɕ��A
//		HRslt popState();

		/// �V�K�r���[�i�ǉ��X���b�v�`�F�[���j
		Pointer<View> createView(uint adpt, HWND hwnd);
		/// �V�K�r���[�i�񖾎��X���b�v�`�F�[���j
		Pointer<View> createView(uint adpt, uint idx);

	private:
		Color clear_color_;
		HCURSOR cursor_backup_; // �}�E�X�J�[�\���̃o�b�N�A�b�v
		Color ambient_light_;
		uint light_num_;
		typedef HandleList<Rsrc> RsrcList;
		RsrcList rsrcs_;
#if 0
		// ����A���ɗ����˂�
		// �g���ȁI
		class StateStack : public std::vector< StateBlock > {
		public:
			StateStack() {}
			HRslt push(IDirect3DDevicePtr dev, D3DSTATEBLOCKTYPE type = D3DSBT_ALL) {
				resize(size()+1);
				if(!back() && !back().setUp(dev, type)) return E_FAIL;
				return back().capture();
			}
			// 1�ȉ��ɂ͂Ȃ�Ȃ��B�ŏ��̈�̓f�t�H���g�ݒ�p
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

	/// �J���[�t�H�[�}�b�g�f�o�b�O�o��
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

	/// �A�_�v�^���f�o�b�O�o��
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