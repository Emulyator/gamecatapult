#ifndef _GCTP_GRAPHIC_HPP_
#define _GCTP_GRAPHIC_HPP_
/** @file
 * GameCatapult DirectX Graphic�N���X�w�b�_�t�@�C��
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
	/** �O���t�B�b�N�֘A�̃��W���[��
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

	/** �r���[�|�[�g�\����
	 *
	 * �R���X�g���N�^�ł��낢��ȈՐݒ�ł���
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
	/** �O���t�B�b�N�`�o�h�N���X
	 *
	 * Direct3D�f�o�C�X������킷�N���X�B
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/26 22:52:23
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Device {
	public:
		Device(/** �����A�_�v�^�ɑ΂��Đ��삷�鎞�A�ǉ��X���b�v�`�F�[���łȂ��ʃf�o�C�X�ɂ��邩�H*/bool is_unique = false);
		HRslt open(HWND h_focus_wnd);
		HRslt open(HWND h_focus_wnd, uint adpt, uint mode);
		HRslt open(HWND h_focus_wnd, HWND h_device_wnd);
		HRslt open(HWND h_focus_wnd, HWND h_device_wnd, uint adpt, uint mode);
		HRslt open(HWND h_focus_wnd, HWND h_first_wnd, HWND h_second_wnd, uint adpt, uint mode);
		HRslt open(uint adpt, uint idx);
		HRslt open(IDirect3DDevice9 *device);
		void close();

		/// �I�[�v���ς݂��H
		bool isOpen() const;

		/// �`��J�n
		HRslt begin();
		/// �`��I��
		HRslt end();
		/// �`�攽�f
		HRslt present();
		/// ��ʂ̃N���A
		HRslt clear(bool target = true, bool zbuffer = true);

		/// ���[���h�}�g���N�X�ݒ�
		void setWorld(const Matrix &mat);
		/// ���[���h�}�g���N�X�ݒ�
		Matrix getWorld();
		/// ���e�}�g���N�X�ݒ�
		void setProjection(const Matrix &mat);
		/// ���e�}�g���N�X�擾
		Matrix getProjection();
		/// �r���[�}�g���N�X�ݒ�
		void setView(const Matrix &mat);
		/// �r���[�}�g���N�X�擾
		Matrix getView();

		/// �O���[�o���A���r�G���g���C�g��ݒ�
		HRslt setAmbient(const Color &col);
		/// �O���[�o���A���r�G���g���C�g���擾
		const Color &getAmbient() const;
		/// ���C�g��ǉ�
		HRslt pushLight(const DirectionalLight &lgt);
		/// ���C�g��ǉ�
		HRslt pushLight(const PointLight &lgt);
		/// ���C�g��ǉ�
		HRslt pushLight(const SpotLight &lgt);
		/// ���C�g���|�b�v
		HRslt popLight();
		/// ���C�g��S����
		void clearLight();
		/// ���݃A�N�e�B�u�ȃ��C�g�̐���Ԃ�
		uint lightNum() const;
		/// ���C�g��ǉ�
		HRslt getLight(uint no, DirectionalLight &lgt) const;
		/// ���C�g��ǉ�
		HRslt getLight(uint no, PointLight &lgt) const;
		/// ���C�g��ǉ�
		HRslt getLight(uint no, SpotLight &lgt) const;

		/// �}�e���A���K�p
		void setMaterial(const Material &mtrl);

		/// �[�x�e�X�g��r���@
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
		/** �[�x�o�b�t�@�̐ݒ�ύX
		 * ����������Ă��Ȃ�
		 */
		HRslt setDepthBufferState(bool enable, bool writable, ZFunc zfunc);
		// ����S�����b�v����̂߂�ǂ��ȁc

		/// �f�o�C�X�������̃��\�[�X���
		void cleanUp();
		/// �f�o�C�X�������̃��\�[�X�č\�z
		void restore();

		/// �X�N���[���T�C�Y��Ԃ�
		Point2 getScreenSize() const;

		/// �N���A����Ƃ��̐F��ݒ�
		void setClearColor(const Color &cc);
	
		/// �r���[�|�[�g���擾
		ViewPort getViewPort() const;
		/// �r���[�|�[�g��ݒ�
		HRslt setViewPort(const ViewPort &vp);

		/// �}�E�X�J�[�\���̕\�����\����ݒ�
		void setCursorVisible(bool yes);

		/// �X�e�[�g�u���b�N�L�^�J�n
		HRslt beginRecord();
		/// �X�e�[�g�u���b�N�L�^�I��
		Pointer<dx::StateBlock> endRecord();
		/// �V�K�X�e�[�g�u���b�N
		Pointer<dx::StateBlock> createState(D3DSTATEBLOCKTYPE type);
		// �X�e�[�g�X�^�b�N�̎d�g�݂�������Ώ\���ł́H
		// �m���ɁA�X�e�[�g�̕ύX�͏��Ȃ��ق����ǂ��A�ύX����Ȃ�X�e�[�g�u���b�N���g����
		// �ꊇ�Őݒ肵���ق��������̂��낤���ǁADirectX�ɖ��������d�l�����A��J�̊���
		// ���I�Ƀp�t�H�[�}���X���オ��킯�ł͂Ȃ��Ǝv���B����Ȃ��Ƃɋ�J���邭�炢�Ȃ�
		// �S�A�[�L�e�N�`���ŗL���ȃe�N�X�`���؂�ւ��̉񐔁A�v���~�e�B�u�`�施�߂̔��s��
		// �����炷���Ƃ��l�������������B
		// �Ƃ����������A���Ȃ�̂�����StateBlockRsrc�𕜊����邵���Ȃ�
		// StateBlock�̓f�o�C�X�������̖����č\�z�̑Ώۂ��B�߂ǂ���ŕ��u

//		/// ���݂̃X�e�[�g��ޔ�
//		HRslt pushState();
//		/// ��O�̃X�e�[�g�ɕ��A
//		HRslt popState();

		/** �J�����g�I�[�f�B�I�f�o�C�X�̎擾
		 *
		 * �J�����g�f�o�C�X��Ԃ��B
		 */
		inline static Device *current() { return current_; }
		/** �J�����g�I�[�f�B�I�f�o�C�X��ݒ�
		 *
		 * �J�����g�f�o�C�X��ݒ肷��B
		 */
		HRslt setCurrent() const;

		/** �����I�u�W�F�N�g��Ԃ��B
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/07/28 3:49:23
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		dx::Device &impl();

		/** �����I�u�W�F�N�g��Ԃ��B
		 *
		 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
		 * @date 2003/07/28 3:49:23
		 *
		 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
		 */
		const dx::Device &impl() const;

		/** ���\�[�X�o�^
		 *
		 * newRsrc�o�R�ARealizer�o�R�Ȃ玩���ōs����̂ŁA������g���K�v�Ȃ�
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/01/26 0:39:42
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		void registerRsrc(Handle<Rsrc> rsrc);

		/** ���\�[�X�f�[�^�x�[�X
		 *
		 * �����Ȃ���
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/01/28 15:20:16
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		const DB &db() const;

		/** ���\�[�X�f�[�^�x�[�X
		 *
		 * �����Ȃ���
		 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
		 * @date 2004/01/28 15:20:16
		 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
		 */
		DB &db();

	private:
		bool is_unique_;	///< �����A�_�v�^�ɑ΂��Đ��삷�鎞�A�ǉ��X���b�v�`�F�[���łȂ��ʃf�o�C�X�ɂ��邩�H
		HRslt preSetUp(uint adpt, HWND hwnd);
		Pointer<DeviceImpl> impl_;
		Pointer<dx::View> view_;
		GCTP_TLS static Device* current_;	///< �J�����g�f�o�C�X�C���X�^���X
	};

	/** �O���t�B�b�N�f�o�C�X�̎擾
	 *
	 * �J�����g�f�o�C�X�̃C���X�^���X��Ԃ��B
	 */
	inline Device &device() { return *Device::current(); }

	/** �O���t�B�b�N�V�X�e���S�̂̏������B
	 *
	 * �����K������Ă����K�v������
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/26 1:14:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	void initialize(bool is_no_WHQL = false);

	/** @defgroup CurrentGraphicDeviceOperation �J�����g�ɑ΂��鑀�� */
	/* @{ */
	/// �`��J�n
	inline HRslt begin()
	{
		return device().begin();
	}
	/// �`��I��
	inline HRslt end()
	{
		return device().end();
	}
	/// �`�攽�f
	inline HRslt present()
	{
		return device().present();
	}
	/// ��ʂ̃N���A
	inline HRslt clear(bool target = true, bool zbuffer = true)
	{
		return device().clear(target, zbuffer);
	}
	/// ��ʂ̏����Ɏg���F��ݒ�
	inline void setClearColor(const Color &cc)
	{
		device().setClearColor(cc);
	}
	/// ���[���h�}�g���N�X�ݒ�
	inline void setWorld(const Matrix &mat)
	{
		device().setWorld(mat);
	}
	/// ���[���h�}�g���N�X�擾
	inline Matrix getWorld()
	{
		return device().getWorld();
	}
	/// ���e�}�g���N�X�ݒ�
	inline void setProjection(const Matrix &mat)
	{
		device().setProjection(mat);
	}
	/// ���e�}�g���N�X�擾
	inline Matrix getProjection()
	{
		return device().getProjection();
	}
	/// �r���[�}�g���N�X�ݒ�
	inline void setView(const Matrix &mat)
	{
		device().setView(mat);
	}
	/// �r���[�}�g���N�X�擾
	inline Matrix getView()
	{
		return device().getView();
	}
	/// �X�N���[���T�C�Y��Ԃ�
	inline Point2 getScreenSize()
	{
		return device().getScreenSize();
	}
	
	/// �r���[�|�[�g��ݒ�
	inline HRslt setViewPort(const ViewPort &src)
	{
		return device().setViewPort(src);
	}

	/// �r���[�|�[�g���擾
	inline ViewPort getViewPort()
	{
		return device().getViewPort();
	}

	/// �}�E�X�J�[�\���̕\���E��\����ݒ�
	inline void setCursorVisible(bool yes)
	{
		device().setCursorVisible(yes);
	}

	/// ������ݒ�
	inline HRslt setAmbient(const Color &col)
	{
		return device().setAmbient(col);
	}

	/// ������ݒ�
	inline const Color &getAmbient()
	{
		return device().getAmbient();
	}

	/// ���s������ǉ�
	inline HRslt pushLight(const DirectionalLight &lgt)
	{
		return device().pushLight(lgt);
	}

	/// �_������ǉ�
	inline HRslt pushLight(const PointLight &lgt)
	{
		return device().pushLight(lgt);
	}

	/// �X�|�b�g���C�g��ǉ�
	inline HRslt pushLight(const SpotLight &lgt)
	{
		return device().pushLight(lgt);
	}

	/// ��ԍŋߒǉ��������C�g���폜
	inline HRslt popLight()
	{
		return device().popLight();
	}

	/// ���ׂẴ��C�g���폜
	inline void clearLight()
	{
		device().clearLight();
	}

	/// ���s������ǉ�
	inline HRslt getLight(uint no, DirectionalLight &lgt)
	{
		return device().getLight(no, lgt);
	}

	/// �_������ǉ�
	inline HRslt getLight(uint no, PointLight &lgt)
	{
		return device().getLight(no, lgt);
	}

	/// �X�|�b�g���C�g��ǉ�
	inline HRslt getLight(uint no, SpotLight &lgt)
	{
		return device().getLight(no, lgt);
	}

	/// ���݃A�N�e�B�u�ȃ��C�g��
	inline uint lightNum()
	{
		return device().lightNum();
	}

	/// �}�e���A���K�p
	inline void setMaterial(const Material &mtrl)
	{
		device().setMaterial(mtrl);
	}

	/// �X�e�[�g�u���b�N�L�^�J�n
	inline HRslt beginRecord()
	{
		return device().beginRecord();
	}

	/// �X�e�[�g�u���b�N�L�^�I��
	inline Pointer<dx::StateBlock> endRecord()
	{
		return device().endRecord();
	}

	/// �X�e�[�g�u���b�N�쐬
	inline Pointer<dx::StateBlock> createState(D3DSTATEBLOCKTYPE type)
	{
		return device().createState(type);
	}
	
//	/// ���݂̃X�e�[�g��ޔ�
//	inline HRslt pushState()
//	{
//		return device().pushState();
//	}

//	/// ��O�̃X�e�[�g�ɕ��A
//	inline HRslt popState()
//	{
//		return device().popState();
//	}

	/** �f�[�^�x�[�X�ɓo�^������
	 *
	 * ���łɃf�[�^�x�[�X�ɂ���Ȃ炻���Ԃ��A�����Ŗ���������V�K���삵�c�a�ɓo�^����A�Ƃ���
	 * ����̃e���v���[�g
	 *
	 * �����Ȃ���
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
						PRNN(_T("���\�[�X'")<<name<<_T("'�𐻍�"));
						h = ret;
						device().db().insert(name, h);
						return ret;
					}
				}
				PRNN(_T("���\�[�X'")<<name<<_T("'�̐���Ɏ��s"));
			}
		}
		else PRNN(_T("���\�[�X�̐���Ɏ��s�B���O���w�肵�Ă�������"));
		return 0;
	}
	/*@}*/

	/** @defgroup GraphicSystemSetting �V�X�e���ݒ�ɑ΂���R���g���[�� */
	/* @{ */

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
	void allowFSAA(FSAAType type, uint level);

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
	void allowStrictMultiThreadSafe(bool yes);

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
	void allowAutoDepthBuffer(bool yes);
	
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
	void allowReferenceDriver(bool yes);
	
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
	void allowHardwareVertexProcessing(bool yes);
	
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
	void allowMixedVertexProcessing(bool yes);

	/** �i�t���X�N���[�����́j��ʍX�V�Ԋu�ݒ�
	 *
	 * �f�t�H���g��0�B(VSync���Ƃ�Ȃ�)
	 * �P��Vsync���Ƃ�B2��Vsync�Q��Ɉ��t���b�v�B3��Vsync�R��Ɉ��t���b�v�B
	 * 0�`4�̒l���w��B
	 * open����O�ɂ�����Ăяo���K�v������B
	 *
	 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
	 * @date 2003/01/04 20:24:41
	 *
	 * Copyright (C) 2001,2002 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
	 */
	void setIntervalTime(int time);

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
	void setSwapMode(bool copy);
	/* @} */

}} // namespace gctp

#endif //_GCTP_GRAPHIC_HPP_