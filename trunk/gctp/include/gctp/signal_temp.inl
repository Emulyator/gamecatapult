/* Signal�e���v���[�g�錾�p�C�����C���t�@�C��
 *
 * @author SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>
 * Copyright (C) 2001 SAM (T&GG, Org.) <sowwa@water.sannet.ne.jp>. All rights reserved.
 * @date 2001/8/6
 */

#define GCTP_SLOT_NAME							BOOST_PP_CAT(Slot,GCTP_SIGNAL_N)
#define GCTP_FNSLOT_NAME						BOOST_PP_CAT(FnSlot,GCTP_SIGNAL_N)
#define GCTP_SIGNAL_NAME						BOOST_PP_CAT(Signal,GCTP_SIGNAL_N)
#define GCTP_MONOSIGNAL_NAME					BOOST_PP_CAT(MonoSignal,GCTP_SIGNAL_N)
#define GCTP_SIGNALSLOT_NAME					BOOST_PP_CAT(SignalSlot,GCTP_SIGNAL_N)
#define GCTP_SIGNAL_MEMBERSLOTNAME				BOOST_PP_CAT(MemberSlot,GCTP_SIGNAL_N)
#define GCTP_SIGNAL_CONSTMEMBERSLOTNAME			BOOST_PP_CAT(ConstMemberSlot,GCTP_SIGNAL_N)
#define GCTP_SIGNAL_MEMBERSIGNALSLOTNAME		BOOST_PP_CAT(MemberSignalSlot,GCTP_SIGNAL_N)
#define GCTP_SIGNAL_CONSTMEMBERSIGNALSLOTNAME	BOOST_PP_CAT(ConstMemberSignalSlot,GCTP_SIGNAL_N)

namespace gctp {

	/** SlotN
	 *
	 * Slot�̈����𑝂₵���o�[�W����\n
	 * Slot10�܂ł���B
	 * @see Slot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<GCTP_SIGNAL_TEMPLATEPARAM>
	class GCTP_SLOT_NAME : public Slot
	{
	public:
		/// �R���X�g���N�^ �v���C�I���e�B�[���w�肷��
		GCTP_SLOT_NAME(int16_t pri) : Slot(pri) {}
		/** �Ăяo���̎���
		 * @param pri �v���C�I���e�B �Ăяo����ł��̃X���b�g�̃v���C�I���e�B�[��ύX�\�ɂ��邽��
		 * @return �p�����邩�H(false��Ԃ���disconnect�����)�܂��́A�Ăяo���𒆒f����i�ڑ���̃V�O�i��_Thunk��true�̂Ƃ��j
		 */
		virtual bool operator()(GCTP_SIGNAL_PARAM) = 0;
	};

	/** SignalN
	 *
	 * Signal�̈����𑝂₵���o�[�W����\n
	 * Signal10�܂ł���B
	 * @see Signal0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<bool _Thunk, GCTP_SIGNAL_TEMPLATEPARAM>
	class GCTP_SIGNAL_NAME : public Signal
	{
	public:
		/// �R���X�g���N�^
		GCTP_SIGNAL_NAME(uint16_t flags = SORT|MASK) : Signal(flags) {}
		/// Slot�̌^
		typedef GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM> SlotType;
		/// �X���b�g�ڑ�
		GCTP_SIGNAL_NAME &operator<<(const SlotType &slot) { connect(slot); return *this; }
		/// �X���b�g�ؒf
		GCTP_SIGNAL_NAME &operator>>(const SlotType &slot) { disconnect(slot); return *this; }
		/// �֘A�t����ꂽ�֐������ׂČĂяo��
		void operator()(GCTP_SIGNAL_ARGTAGS)
		{
			sort();
			for(SlotList::iterator it = slots_.begin(); it != slots_.end();) {
				Handle<SlotType> slot = (*it);
				if(slot) {
					if(canInvoke(*slot)) {
						if(_Thunk) {
							if((*slot)(GCTP_SIGNAL_ARGS)) break;
							else ++it;
						}
						else {
							if((*slot)(GCTP_SIGNAL_ARGS)) ++it;
							else it = slots_.erase(it);
						}
					}
					else ++it;
				}
				else it = slots_.erase(it);
			}
		}
	};

	/** MonoSignalN
	 *
	 * MonoSignal�̈����𑝂₵���o�[�W����\n
	 * MonoSignal10�܂ł���B
	 * @see MonoSignal0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<GCTP_SIGNAL_TEMPLATEPARAM>
	class GCTP_MONOSIGNAL_NAME : public MonoSignal
	{
	public:
		/// Slot�̌^
		typedef GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM> SlotType;
		/// �X���b�g�ڑ�
		GCTP_MONOSIGNAL_NAME &operator<<(const SlotType &slot) { connect(slot); return *this; }
		/// �X���b�g�ؒf
		GCTP_MONOSIGNAL_NAME &operator>>(const SlotType &slot) { disconnect(slot); return *this; }
		/// �֘A�t����ꂽ�֐������ׂČĂяo��
		bool operator()(GCTP_SIGNAL_ARGTAGS)
		{
			Handle<SlotType> slot = slot_;
			if(slot) return (*slot)(GCTP_SIGNAL_ARGS);
			return false;
		}
	};

	/** SignalSlotN
	 *
	 * SignalSlot�̈����𑝂₵���o�[�W����\n
	 * SignalSlot10�܂ł���B
	 * @see SignalSlot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<bool _Thunk, GCTP_SIGNAL_TEMPLATEPARAM>
	class GCTP_SIGNALSLOT_NAME : public GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>
	{
	public:
		typedef GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM> SlotType;
		/// �֌W�t�����Ă���֐��̌Ăяo��
		virtual bool operator()(GCTP_SIGNAL_ARGTAGS)
		{
			bool handled = true, ret;
			ret = (*this)(handled, GCTP_SIGNAL_ARGS);
			if(!handled) signal_(GCTP_SIGNAL_ARGS);
			return ret;
		}
		/// �X���b�g�ڑ�
		GCTP_SIGNALSLOT_NAME &operator<<(const SlotType &slot) { signal_.connect(slot); return *this; }
		/// �X���b�g�ؒf
		GCTP_SIGNALSLOT_NAME &operator>>(const SlotType &slot) { signal_.disconnect(slot); return *this; }
		
		/// �V�O�i���I�u�W�F�N�g��Ԃ�
		GCTP_SIGNAL_NAME<_Thunk, GCTP_SIGNAL_PARAM> &signal() { return signal_; }
		/// �V�O�i���I�u�W�F�N�g��Ԃ�
		const GCTP_SIGNAL_NAME<_Thunk, GCTP_SIGNAL_PARAM> &signal() const { return signal_; }

		/** �Ăяo���̎���
		 * @param pri �v���C�I���e�B �Ăяo����ł��̃X���b�g�̃v���C�I���e�B�[��ύX�\�ɂ��邽��
		 * @return �p�����邩�H(false��Ԃ���disconnect�����)�܂��́A�Ăяo���𒆒f����i�ڑ���̃V�O�i��_Thunk��true�̂Ƃ��j
		 */
		virtual bool operator()(bool &handled, GCTP_SIGNAL_ARGTAGS) = 0;

	private:
		GCTP_SIGNAL_NAME<_Thunk, GCTP_SIGNAL_PARAM> signal_;
	};

	/** FnSlotN
	 *
	 * FnSlot0�̈����𑝂₵���o�[�W����\n
	 * Slot10�܂ł���B
	 * @see Slot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<GCTP_SIGNAL_TEMPLATEPARAM, bool (*Method)(GCTP_SIGNAL_PARAM)>
	class GCTP_FNSLOT_NAME : public GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>
	{
	public:
		/// �R���X�g���N�^ �v���C�I���e�B�[���w�肷��
		GCTP_FNSLOT_NAME(int16_t pri = 0) : GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>(pri) {}
		/// �֐��Ăяo�������̃C���^�[�t�F�[�X
		virtual bool operator()(GCTP_SIGNAL_ARGTAGS) { return (*Method)(GCTP_SIGNAL_ARGS); }
	};

	/** MemberSlotN
	 *
	 * MemberSlot�̈����𑝂₵���o�[�W����\n
	 * MemberSlot10�܂ł���B
	 * @see MemberSlot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<class _T, GCTP_SIGNAL_TEMPLATEPARAM, bool (_T::*Method)(GCTP_SIGNAL_PARAM)>
	class GCTP_SIGNAL_MEMBERSLOTNAME : public GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>
	{
	public:
		/// �R���X�g���N�^
		GCTP_SIGNAL_MEMBERSLOTNAME(int16_t pri = 0) : GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>(pri) {}
		/// �R���X�g���N�^ �v���C�I���e�B�[�Ɗ֘A�t����C���X�^���X���w�肷��
		GCTP_SIGNAL_MEMBERSLOTNAME(_T *_this, int16_t pri = 0) : GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>(pri), this_(_this) {}
		/// �C���X�^���X���w�肷��
		void bind(_T *_this) { this_ = _this; }
		/// �����o�֐��Ăяo���̎���
		virtual bool operator()(GCTP_SIGNAL_ARGTAGS)
		{
			if(this_) return (this_.get()->*Method)(GCTP_SIGNAL_ARGS);
			return false;
		}
	private:
		Handle<_T> this_;
	};

	/** ConstMemberSlotN
	 *
	 * ConstMemberSlot�̈����𑝂₵���o�[�W����\n
	 * ConstMemberSlot10�܂ł���B
	 * @see MemberSlot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<class _T, GCTP_SIGNAL_TEMPLATEPARAM, bool (_T::*Method)(GCTP_SIGNAL_PARAM) const>
	class GCTP_SIGNAL_CONSTMEMBERSLOTNAME : public GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>
	{
	public:
		/// �R���X�g���N�^
		GCTP_SIGNAL_CONSTMEMBERSLOTNAME(int16_t pri = 0) : GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>(pri) {}
		/// �R���X�g���N�^ �v���C�I���e�B�[�Ɗ֘A�t����C���X�^���X���w�肷��
		GCTP_SIGNAL_CONSTMEMBERSLOTNAME(const _T *_this, int16_t pri = 0) : GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>(pri), this_(_this) {}
		/// �C���X�^���X���w�肷��
		void bind(const _T *_this) { this_ = _this; }
		/// �����o�֐��Ăяo���̎���
		virtual bool operator()(GCTP_SIGNAL_ARGTAGS)
		{
			if(this_) return (this_.get()->*Method)(GCTP_SIGNAL_ARGS);
			return false;
		}
	private:
		Handle<const _T> this_;
	};

	/** MemberSignalSlotN
	 *
	 * MemberSignalSlot�̈����𑝂₵���o�[�W����\n
	 * MemberSignalSlot10�܂ł���B
	 * @see MemberSignalSlot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<bool _Thunk, class _T, GCTP_SIGNAL_TEMPLATEPARAM, bool (_T::*Method)(bool &, GCTP_SIGNAL_PARAM)>
	class GCTP_SIGNAL_MEMBERSIGNALSLOTNAME : public GCTP_SIGNALSLOT_NAME<_Thunk, GCTP_SIGNAL_PARAM>
	{
	public:
		/// �R���X�g���N�^
		GCTP_SIGNAL_MEMBERSIGNALSLOTNAME(int16_t pri = 0) : GCTP_SIGNALSLOT_NAME<_Thunk, GCTP_SIGNAL_PARAM>(pri) {}
		/// �R���X�g���N�^ �v���C�I���e�B�[�Ɗ֘A�t����C���X�^���X���w�肷��
		GCTP_SIGNAL_MEMBERSIGNALSLOTNAME(_T *_this, int16_t pri = 0) : GCTP_SIGNALSLOT_NAME<_Thunk, GCTP_SIGNAL_PARAM>(pri), this_(_this) {}
		/// �C���X�^���X���w�肷��
		void bind(_T *_this) { this_ = _this; }
		/// �����o�֐��Ăяo���̎���
		virtual bool operator()(bool &handled, GCTP_SIGNAL_ARGTAGS)
		{
			if(this_) return (this_.get()->*Method)(handled, GCTP_SIGNAL_ARGS);
			return false;
		}
	private:
		Handle<_T> this_;
	};

	/** ConstMemberSignalSlotN
	 *
	 * ConstMemberSignalSlot�̈����𑝂₵���o�[�W����\n
	 * ConstMemberSignalSlot10�܂ł���B
	 * @see MemberSignalSlot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<bool _Thunk, class _T, GCTP_SIGNAL_TEMPLATEPARAM, bool (_T::*Method)(bool &, GCTP_SIGNAL_PARAM) const>
	class GCTP_SIGNAL_CONSTMEMBERSIGNALSLOTNAME : public GCTP_SIGNALSLOT_NAME<_Thunk, GCTP_SIGNAL_PARAM>
	{
	public:
		/// �R���X�g���N�^
		GCTP_SIGNAL_CONSTMEMBERSIGNALSLOTNAME(int16_t pri = 0) : GCTP_SIGNALSLOT_NAME<_Thunk, GCTP_SIGNAL_PARAM>(pri) {}
		/// �R���X�g���N�^ �v���C�I���e�B�[�Ɗ֘A�t����C���X�^���X���w�肷��
		GCTP_SIGNAL_CONSTMEMBERSIGNALSLOTNAME(const _T *_this, int16_t pri = 0) : GCTP_SIGNALSLOT_NAME<_Thunk, GCTP_SIGNAL_PARAM>(pri), this_(_this) {}
		/// �C���X�^���X���w�肷��
		void bind(const _T *_this) { this_ = _this; }
		/// �����o�֐��Ăяo���̎���
		virtual bool operator()(bool &handled, GCTP_SIGNAL_ARGTAGS)
		{
			if(this_) return (this_.get()->*Method)(handled, GCTP_SIGNAL_ARGS);
			return false;
		}
	private:
		Handle<const _T> this_;
	};

} // namespace gctp

#undef GCTP_SLOT_NAME
#undef GCTP_FNSLOT_NAME
#undef GCTP_SIGNAL_NAME
#undef GCTP_MONOSIGNAL_NAME
#undef GCTP_SIGNALSLOT_NAME
#undef GCTP_SIGNAL_MEMBERSLOTNAME
#undef GCTP_SIGNAL_CONSTMEMBERSLOTNAME
#undef GCTP_SIGNAL_MEMBERSIGNALSLOTNAME
#undef GCTP_SIGNAL_CONSTMEMBERSIGNALSLOTNAME
