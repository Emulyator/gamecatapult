#ifndef _GCTP_GRAPHIC_PARTICLEBUFFER_HPP_
#define _GCTP_GRAPHIC_PARTICLEBUFFER_HPP_
/** @file
 * GameCatapult �p�[�e�B�N���N���X�w�b�_�t�@�C��
 *
 * �p�[�e�B�N���Ƃ������A�G�t�F�N�g�p���I���b�V���S�ʂ�����
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
	/** �_�p�[�e�B�N����`���
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

		/// ���N�g����S���_���̂t�u��ݒ�
		ParticleDesc &setUV(const Rectf &rc) {
			uv[0].x = rc.left;  uv[0].y = rc.top;
			uv[1].x = rc.right; uv[1].y = rc.top;
			uv[2].x = rc.left;  uv[2].y = rc.bottom;
			uv[3].x = rc.right; uv[3].y = rc.bottom;
			return *this;
		}
		/// �S���_���ׂĂɓ���̐F�w��
		ParticleDesc &setColor(Color32 col) { color[0] = color[1] = color[2] = color[3] = col; return *this; }
		/// �S���_���ׂĂɓ���̃n�C���C�g�w��
		ParticleDesc &setHilight(Color32 col) { hilight[0] = hilight[1] = hilight[2] = hilight[3] = col; return *this; }
	};

	/** ���p�[�e�B�N����`���
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

		/// ���N�g����S���_���̂t�u��ݒ�
		LineParticleDesc &setUV(Rectf rc) {
			uv[0].x = rc.left;  uv[0].y = rc.top;
			uv[1].x = rc.right; uv[1].y = rc.top;
			uv[2].x = rc.left;  uv[2].y = rc.bottom;
			uv[3].x = rc.right; uv[3].y = rc.bottom;
			return *this;
		}
		/// �S���_���ׂĂɓ���̐F�w��
		LineParticleDesc &setColor(Color32 col) { color[0] = color[1] = color[2] = color[3] = col; return *this; }
		/// �S���_���ׂĂɓ���̃n�C���C�g�w��
		LineParticleDesc &setHilight(Color32 col) { hilight[0] = hilight[1] = hilight[2] = hilight[3] = col; return *this; }
	};

	/** �p�[�e�B�N���o�b�t�@�N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/07/15 4:02:46
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class ParticleBuffer : public VertexBuffer {
	public:
		struct LVertex;

		/// �u�����h���[�h
		enum BlendMode {
			NONE,
			ADD,
			SUB,
//			MUL,
			ALPHA,
		};

		HRslt setUp(const char *name) { return setUp(); } // createOnDB�p
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