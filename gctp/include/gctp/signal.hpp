#ifndef _GCTP_SIGNAL_HPP_
#define _GCTP_SIGNAL_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult Signal/Slot�N���X�w�b�_�t�@�C��
 *
 * gctp::Signal / gctp::Slot�Ƃ́A�ȈՔł�Signal/Slot�@�\�ł���B
 * Boost.Signals�̂ق������_��ŕ֗������Aboost::shared_ptr��boost::any�Ɉˑ����Ă���A�����gctp::Pointer / gctp::Handle��
 * �������ɒu���������������BSlot��gctp::Handle�ŕ⑫���邱�Ƃɂ���΁Ascoped_connection�Ȃǂ�p���Ȃ��Ă��A
 * disconnect�̑��삪���ȕւɂȂ�B
 * boost::any��Object�h���Ɍ��肷��΁Agctp::Handle�ő�p�\�ł���B
 * �܂��ASlot�֐����g�ɁA���̌Ăяo���R���e�L�X�g�ł̒E�ނ̑�����������������B
 * (�����Boost.Signals�ɂ͕W���ŗp�ӂ���Ă��Ȃ�)
 * �p�r����̓C�x���g�ւ̉�����Q�[���I�u�W�F�N�g��Mixin�Ɍ��肵�A���͈̔͂ł͕֗��Ɏg����悤�ɂ��Ă���B
 * ���̂��ߎ�ނ͂��ׂČĂяo���A�����Ȃ��B
 *
 * void (int i, int j)�̂悤��preferred �`�����̗p���Ȃ������̂́A�֐��`���� bool (...)�ɌŒ肵�A�Ԃ�l����������s���邩�A
 * �܂�false�Ȃ�disconnect����A�Ƃ����t���O�Ɏg�����߁B�������ň����̐��ɐ������ł��Ă��܂������B
 * �܂�boost::function��Slot���󂯂�̂ł͂Ȃ��AObject�h���̊֐��I�u�W�F�N�g����ɗv������A�܂蕁�ʂ̊֐����󂯕t���Ȃ��A
 * �̂́Aboost::any&boost:shared_ptr�̔r���AHandle�ɂ��connect/disconnect����̎����A�̂��߁B���ʂ̊֐����󂯕t����悤��
 * ���悤�Ƃ���ƁA���ǂ�Boost.Signals���l��boost::any&boost:shared_ptr���g���������ɂȂ炴��𓾂Ȃ��B
 *
 * �c���[�\���̌`���́Agctp::Scene�̂悤��gctp::Signal�����I�u�W�F�N�g�ɂ���gctp::Signal�𔭉΂��郁���o�֐����`���A
 * ����gctp::Slot��񋟂��邱�ƂŎ�������gctp::Signal�́A�X���b�g�̃f�[�^�x�[�X�@�\��񋟂��Ȃ�
 *
 * (�}�E�X�Ȃǂ̓��̓��b�Z�[�W�Łj�������̂��Ƃ肽���ꍇ�A�܂�A����ʂ̃X���b�g�����s���ꂽ���_�ŉ��ʂ̃X���b�g�����s���Ȃ��A
 * �Ƃ�����������������ɂ́ASlot�̃I�[�o�[���C�h�Ŏ�������
 *
 * ����ς��ꂢ�낢��ւ񂾂Ȃ��B�ς������񂾂��ǁA���܂���B�B�B�v���C�I���e�B�[���ς��₷���Ƃ��������b�g�͂��邯��
 *
 * VC6�ł�\n
 * class Foo {\n
 *     bool bar();\n
 *     MemberSlot0<Foo, bar> slot;\n
 *     Foo() : slot(this) {}\n
 * };\n
 * �Ƃ����\�����g���Ȃ��B�i�N���X��`�̍Œ��Ƀ����o�֐�bar���V���{���Ƃ��ĔF���ł��Ȃ��j\n
 * �ւ���\n
 * class Foo {\n
 *     bool bar();\n
 *     Pointer<Slot> slot;\n
 *     Foo();\n
 * };\n
 * inline Foo::Foo() : slot(new MemberSlot0<Foo, &Foo::bar>(this)) {}\n
 * �Ȃǂ���B\n
 * ���̕\�������Ă�signal << slot;�Ƃ����\�����ʗp����悤�ɁASignal��
 * connect(Pointer<Slot> slot)�����B���A���̂��߂Ɉ������X�g���Ⴄ�X���b�g��
 * �����Ă��G���[�ɂȂ炸�ڑ��ł��Ă��܂��B�������X�g������Ȃ��X���b�g�́A�Ăяo������
 * �`�F�b�N������S�ɍ폜����邪�A���̃G���[���Ԃ��Ȃ��B
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * @date 2001/8/6
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 */
#include <gctp/handlelist.hpp>

namespace gctp {

	/** �X���b�g���N���X
	 *
	 * �X���b�g���g���v���C�I���e�B�[�l�������߁A�����̃V�O�i���ɐڑ������Ƃ��A���ꂼ��ɕʌ̗D��x��
	 * �^�����Ȃ����Ƃɒ��ӁB�}�X�N�l�����l�B
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/12/06 0:09:31
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Slot : public Object
	{
	public:
		enum {
// ���[��B�B�B
#ifdef MAX_PRIORITY
#define BACKUP_MAX_PRIORITY MAX_PRIORITY
#undef MAX_PRIORITY
#endif
#ifdef MIN_PRIORITY
#define BACKUP_MIN_PRIORITY MIN_PRIORITY
#undef MIN_PRIORITY
#endif
			MAX_PRIORITY = 0x7FFF,
			MIN_PRIORITY = 0x8000
#ifdef BACKUP_MAX_PRIORITY
#define MAX_PRIORITY BACKUP_MAX_PRIORITY
#undef BACKUP_MAX_PRIORITY
#endif
#ifdef BACKUP_MIN_PRIORITY
#define MIN_PRIORITY BACKUP_MIN_PRIORITY
#undef BACKUP_MIN_PRIORITY
#endif
		};
		/// �v���C�I���e�B�[���w�肵�ď�����
		Slot(int16_t pri) : pri_(pri), mask_(0xFFFF) {}
		/// �v���C�I���e�B�[��Ԃ�
		int16_t priority() const { return pri_; }
		/// �v���C�I���e�B�[��ݒ�
		int16_t setPriority(int16_t pri) { std::swap(pri_, pri); return pri; }
		/// �}�X�N�l��Ԃ�
		uint16_t mask() const { return mask_; }
		/// �}�X�N�l��ݒ�
		uint16_t setMask(uint16_t mask) { std::swap(mask_, mask); return mask; }
	protected:
		int16_t pri_;
		uint16_t mask_;
	};

	/** �V�O�i���N���X
	 *
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Signal : public Object
	{
	public:
		/// ����ύX�t���O
		enum Flag {
			SORT = 1,			///< �v���C�I���e�B�[�ɂ��\�[�g���s��
			MASK = 1<<1,		///< �}�X�L���O���s��
			_FORCE_WORD_ = 0x7FFF
		};

		/// �R���X�g���N�^
		Signal(uint16_t flags = SORT|MASK) : mask_(0xFFFF), flags_(flags) {}

		/// ���łɓo�^����Ă��Ȃ����H
		bool isUnique(const Slot &slot);
		/// �X���b�g�ڑ�
		void connect(const Slot &slot);
		/// �X���b�g�ڑ�(�d�����`�F�b�N)
		void connectOnce(const Slot &slot)
		{
			if(isUnique(slot)) connect(slot);
		}
		/// �X���b�g�ؒf
		void disconnect(const Slot &slot);
		/// �ڑ�����Ă���X���b�g�̐���Ԃ�
		std::size_t num() const;
		/// �}�X�N�l��Ԃ�
		uint16_t mask() const { return mask_; }
		/// �}�X�N�l��ݒ�
		uint16_t setMask(uint16_t mask) { std::swap(mask_, mask); return mask; }
		/// �t���O��Ԃ�
		uint16_t flags() const { return flags_; }
		/// �t���O��ݒ�
		uint16_t setFlags(uint16_t flags) { std::swap(flags_, flags); return flags; }
	protected:
		/// �v���C�I���e�B���ɃX���b�g���\�[�g
		void sort();
		/// ���s���邩�ǂ����e�X�g
		bool canInvoke(const Slot &slot);

		typedef HandleList<Slot> SlotList;
		SlotList slots_;
		uint16_t mask_;
		uint16_t flags_;
	};

	/** �P�V�O�i���N���X
	 *
	 * �ЂƂ̃X���b�g�������Ȃ�
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class MonoSignal : public Object
	{
	public:
		/// �X���b�g�ڑ�
		void connect(const Slot &slot)
		{
			slot_ = Handle<Slot>(const_cast<Slot *>(&slot));
		}
		/// �X���b�g�ؒf
		void disconnect(const Slot &slot)
		{
			if(slot_ == Handle<Slot>(const_cast<Slot *>(&slot))) slot_ = 0;
		}
		/// �X���b�g���ڑ�����Ă��邩�H
		bool empty() const { return !slot_; }
	protected:
		Handle<Slot> slot_;
	};

	/** �����Ȃ��̃X���b�g�N���X
	 *
	 * virtual bool operator()(int &pri)���I�[�o�[���C�h����B
	 *
	 * �w�肵�����\�b�h��false��Ԃ��ƁA���s�セ��Signal����͂������B
	 *
	 * �h�L�������g�ɂ͋L�ڂ���Ȃ����ASlot1�`Slot10����`����Ă��āA�ő�P�O�̈�����������Slot���`�ł���B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:52:09
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Slot0 : public Slot
	{
	public:
		/// �v���C�I���e�B�[���w�肵�ď�����
		Slot0(int16_t pri) : Slot(pri) {}
		/** �Ăяo���̎���
		 * @return �p�����邩�H(false��Ԃ���disconnect�����)�܂��́A�Ăяo���𒆒f����i�ڑ���̃V�O�i��_Thunk��true�̂Ƃ��j
		 */
		virtual bool operator()() = 0;
	};

	/** �����Ȃ��̃V�O�i���N���X
	 *
	 * �v���C�I���e�B�[���ɑS�X���b�g���Ăяo���B
	 *
	 * �h�L�������g�ɂ͋L�ڂ���Ȃ����ASignal1�`Signal10����`����Ă��āA�ő�P�O�̈�����������Signal���`�ł���B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<bool _Thunk>
	class Signal0 : public Signal
	{
	public:
		typedef Slot0 SlotType;
		/// �֌W�t�����Ă���֐��̌Ăяo��
		void operator()()
		{
			sort();
			for(SlotList::iterator it = slots_.begin(); it != slots_.end();) {
				Handle<SlotType> slot = (*it);
				if(slot) {
					if(canInvoke(*slot)) {
						if(_Thunk) {
							if((*slot)()) break;
							else ++it;
						}
						else {
							if((*slot)()) ++it;
							else it = slots_.erase(it);
						}
					}
					else ++it;
				}
				else it = slots_.erase(it);
			}
		}

		/// �X���b�g�ڑ�
		Signal0 &operator<<(const SlotType &slot) { connect(slot); return *this; }
		/// �X���b�g�ؒf
		Signal0 &operator>>(const SlotType &slot) { disconnect(slot); return *this; }
	};

	/** �����Ȃ��̒P�V�O�i���N���X
	 *
	 * �h�L�������g�ɂ͋L�ڂ���Ȃ����ASignal1�`Signal10����`����Ă��āA�ő�P�O�̈�����������Signal���`�ł���B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class MonoSignal0 : public MonoSignal
	{
	public:
		typedef Slot0 SlotType;
		/// �֌W�t�����Ă���֐��̌Ăяo��
		bool operator()()
		{
			Handle<SlotType> slot = slot_;
			if(slot) return (*slot)();
			return false;
		}

		/// �X���b�g�ڑ�
		MonoSignal0 &operator<<(const SlotType &slot) { connect(slot); return *this; }
		/// �X���b�g�ؒf
		MonoSignal0 &operator>>(const SlotType &slot) { disconnect(slot); return *this; }
	};

	/** �V�O�i���𔭍s����X���b�g�N���X
	 *
	 * �v���C�I���e�B�[���ɑS�X���b�g���Ăяo���B
	 *
	 * �c���[�\�����`�����邽�߂Ɏg����B�Œ�̈����Ƃ��āAhandled���n����顂���handled�����l��true�ŁA
	 * true���ݒ肳�ꂽ�܂ܕԂ�Ƃ��̃I�u�W�F�N�g���ێ�����Signal�𔭉΂��Ȃ��B���ʂ̃X���b�g�ɃV�O�i����n���ꍇ��
	 * false��ݒ肷��B
	 *
	 * �h�L�������g�ɂ͋L�ڂ���Ȃ����ASignalSlot1�`SignalSlot10����`����Ă��āA(bool handled �ȊO�Ɂj
	 * �ő�P�O�̈�����������SignalSlot���`�ł���B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:11
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<bool _Thunk>
	class SignalSlot0 : public Slot0
	{
	public:
		typedef Slot0 SlotType;
		/// �֌W�t�����Ă���֐��̌Ăяo��
		virtual bool operator()()
		{
			bool handled = true, ret;
			ret = (*this)(handled);
			if(!handled) signal_();
			return ret;
		}
		/// �X���b�g�ڑ�
		SignalSlot0 &operator<<(const SlotType &slot) { signal_.connect(slot); return *this; }
		/// �X���b�g�ؒf
		SignalSlot0 &operator>>(const SlotType &slot) { signal_.disconnect(slot); return *this; }
		
		/// �V�O�i���I�u�W�F�N�g��Ԃ�
		Signal0<_Thunk> &signal() { return signal_; }
		/// �V�O�i���I�u�W�F�N�g��Ԃ�
		const Signal0<_Thunk> &signal() const { return signal_; }

		/** �Ăяo���̎���
		 * @param pri �v���C�I���e�B �Ăяo����ł��̃X���b�g�̃v���C�I���e�B�[��ύX�\�ɂ��邽��
		 * @param handled �f�t�H���g��true false������Ǝq�V�O�i���𔭉΂��Ȃ�
		 * @return �p�����邩�H(false��Ԃ���disconnect�����)�܂��́A�Ăяo���𒆒f����i�ڑ���̃V�O�i��_Thunk��true�̂Ƃ��j
		 */
		virtual bool operator()(bool &handled) = 0;

	private:
		Signal0<_Thunk> signal_;
	};

	/** �����Ȃ��̃X���b�g�N���X
	 *
	 * fn_�ɑ���B
	 *
	 * �w�肵�����\�b�h��false��Ԃ��ƁA���s�セ��Signal����͂������B
	 *
	 * �h�L�������g�ɂ͋L�ڂ���Ȃ����AFnSlot1�`FnSlot10����`����Ă��āA�ő�P�O�̈�����������FnSlot���`�ł���B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:52:09
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<bool (*Method)()>
	class FnSlot0 : public Slot0
	{
	public:
		/// �v���C�I���e�B�[���w�肵�ď�����
		FnSlot0(int16_t pri = 0) : Slot0(pri) {}
		/** �Ăяo���̎���
		 * @return �p�����邩�H(false��Ԃ���disconnect�����)
		 */
		virtual bool operator()() { return (*Method)(); }
	};

	/** �����o�֐����w�肷��Slot�N���X�e���v���[�g
	 *
	 * �w�肵�����\�b�h��false��Ԃ��ƁA���s�セ��Signal����͂����B
	 *
	 * ���s����Â������Atrue��Ԃ����ƁB
	 *
	 * @param _T �I�u�W�F�N�g�̌^
	 * @param Method ���s����郁���o�֐��̎w��
	 *
	 * @code
	 *  class Foo : public Object {
	 *  public:
	 *      bool update(int &pri);
	 *      ConstMemberSlot0<Foo, Foo::update> slot;
	 *  };
	 * 
	 *  Pointer<Foo> foo;
	 *  update_signal.connect(foo->slot);
	 * @endcode
	 * 
	 * �����o�֐��|�C���^�[�Ŏ��̂Ƃǂ����������̂��Ȃ��c
	 *
	 * �h�L�������g�ɂ͋L�ڂ���Ȃ����AMemberSlot1�`MemberSlot10����`����Ă��āA�ő�P�O�̈�����������
	 * MemberSlot���`�ł���B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:52
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, bool (_T::*Method)()>
	class MemberSlot0 : public Slot0
	{
	public:
		/// �R���X�g���N�^
		MemberSlot0(int16_t pri = 0) : Slot0(pri) {}
		/// �R���X�g���N�^ �v���C�I���e�B�[�Ɗ֘A�t����C���X�^���X���w�肷��
		MemberSlot0(_T *_this, int16_t pri = 0) : Slot0(pri), this_(_this) {}
		/// �C���X�^���X���w�肷��
		void bind(_T *_this) { this_ = _this; }
		virtual bool operator()()
		{
			if(this_) return (this_.get()->*Method)();
			return false;
		}
	private:
		Handle<_T> this_;
	};

	/** �����o�֐����w�肷��Slot�N���X�e���v���[�g
	 *
	 * MemberSlot��const�������o�֐��p�B
	 *
	 * �ꕔ�R���p�C���ŕK�v�B�R���p�C���\�Ȃ�MemberSlot<const T, ...>�ł��悢�B
	 *
	 * �w�肵�����\�b�h��false��Ԃ��ƁA���s�セ��Signal����͂����B\n
	 * ���s����Â������Atrue��Ԃ����ƁB
	 *
	 * @param _T �I�u�W�F�N�g�̌^
	 * @param Method ���s����郁���o�֐��̎w��
	 *
	 * @code
	 *  class Foo : public Object {
	 *  public:
	 *      bool draw(int &pri) const;
	 *      ConstMemberSlot0<Foo, Foo::draw> slot;
	 *  };
	 * 
	 *  Pointer<Foo> foo;
	 *  draw_invoker.connect(foo->slot);
	 * @endcode
	 *
	 * �h�L�������g�ɂ͋L�ڂ���Ȃ����AConstMemberSlot1�`ConstMemberSlot10����`����Ă��āA�ő�P�O�̈�����������
	 * ConstMemberSlot���`�ł���B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:52
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<class _T, bool (_T::*Method)() const>
	class ConstMemberSlot0 : public Slot0
	{
	public:
		/// �R���X�g���N�^
		ConstMemberSlot0(int16_t pri = 0) : Slot0(pri), this_(_this) {}
		/// �R���X�g���N�^ �v���C�I���e�B�[�Ɗ֘A�t����C���X�^���X���w�肷��
		ConstMemberSlot0(const _T *_this, int16_t pri = 0) : Slot0(pri), this_(_this) {}
		/// �C���X�^���X���w�肷��
		void bind(const _T *_this) { this_ = _this; }
		virtual bool operator()()
		{
			if(this_) return (this_.get()->*Method)();
			return false;
		}
	private:
		Handle<const _T> this_;
	};

	/** �����o�֐����w�肷��SignalSlot�N���X�e���v���[�g
	 *
	 * �w�肵�����\�b�h��false��Ԃ��ƁA���s�セ��Signal����͂����B
	 *
	 * ���s����Â������Atrue��Ԃ����ƁB
	 *
	 * @param _T �I�u�W�F�N�g�̌^
	 * @param Method ���s����郁���o�֐��̎w��
	 *
	 * @code
	 *  class Foo : public Object {
	 *  public:
	 *      bool update(int &pri);
	 *      ConstMemberSignalSlot0<Foo, Foo::update> slot;
	 *  };
	 * 
	 *  Pointer<Foo> foo;
	 *  update_signal.connect(foo->slot);
	 * @endcode
	 * 
	 * �h�L�������g�ɂ͋L�ڂ���Ȃ����AMemberSlot1�`MemberSlot10����`����Ă��āA(int &pri�ȊO�Ɂj�ő�P�O�̈�����������
	 * MemberSlot���`�ł���B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:52
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<bool _Thunk, class _T, bool (_T::*Method)(bool &)>
	class MemberSignalSlot0 : public SignalSlot0<_Thunk>
	{
	public:
		/// �R���X�g���N�^
		MemberSignalSlot0(int16_t pri = 0) : SignalSlot0<_Thunk>(pri) {}
		/// �R���X�g���N�^ �v���C�I���e�B�[�Ɗ֘A�t����C���X�^���X���w�肷��
		MemberSignalSlot0(_T *_this, int16_t pri = 0) : SignalSlot0<_Thunk>(pri), this_(_this) {}
		/// �C���X�^���X���w�肷��
		void bind(_T *_this) { this_ = _this; }
		virtual bool operator()(bool &handled)
		{
			if(this_) return (this_.get()->*Method)(handled);
			return false;
		}
	private:
		Handle<_T> this_;
	};

	/** �����o�֐����w�肷��SignalSlot�N���X�e���v���[�g
	 *
	 * MemberSignalSlot��const�������o�֐��p�B
	 *
	 * �ꕔ�R���p�C���ŕK�v�B�R���p�C���\�Ȃ�MemberSignalSlot<const T, ...>�ł��悢�B
	 *
	 * �w�肵�����\�b�h��false��Ԃ��ƁA���s�セ��Signal����͂����B\n
	 * ���s����Â������Atrue��Ԃ����ƁB
	 *
	 * @param _T �I�u�W�F�N�g�̌^
	 * @param Method ���s����郁���o�֐��̎w��
	 *
	 * @code
	 *  class Foo : public Object {
	 *  public:
	 *      bool draw(int &pri) const;
	 *      ConstMemberSignalSlot0<Foo, Foo::draw> slot;
	 *  };
	 * 
	 *  Pointer<Foo> foo;
	 *  draw_invoker.connect(foo->slot);
	 * @endcode
	 *
	 * �h�L�������g�ɂ͋L�ڂ���Ȃ����AConstMemberSignalSlot1�`ConstMemberSignalSlot10����`����Ă��āA
	 * (bool handled �ȊO�Ɂj�ő�P�O�̈�����������ConstMemberSignalSlot���`�ł���B
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 22:51:52
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<bool _Thunk, class _T, bool (_T::*Method)(bool &) const>
	class ConstMemberSignalSlot0 : public SignalSlot0<_Thunk>
	{
	public:
		/// �R���X�g���N�^
		ConstMemberSignalSlot0(int16_t pri = 0) : SignalSlot0<_Thunk>(pri) {}
		/// �R���X�g���N�^ �v���C�I���e�B�[�Ɗ֘A�t����C���X�^���X���w�肷��
		ConstMemberSignalSlot0(const _T *_this, int16_t pri = 0) : SignalSlot0<_Thunk>(pri), this_(_this) {}
		/// �C���X�^���X���w�肷��
		void bind(const _T *_this) { this_ = _this; }
		virtual bool operator()(bool &handled)
		{
			if(this_) return (this_.get()->*Method)(handled);
			return false;
		}
	private:
		Handle<const _T> this_;
	};

} // namespace gctp

#include <boost/preprocessor/cat.hpp> 
#include <gctp/signal.inl>

#endif //_GCTP_SIGNAL_HPP_