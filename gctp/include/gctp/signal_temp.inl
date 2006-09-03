/* Signalテンプレート宣言用インラインファイル
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
	 * Slotの引数を増やしたバージョン\n
	 * Slot10まである。
	 * @see Slot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<GCTP_SIGNAL_TEMPLATEPARAM>
	class GCTP_SLOT_NAME : public Slot
	{
	public:
		/// コンストラクタ プライオリティーを指定する
		GCTP_SLOT_NAME(int16_t pri) : Slot(pri) {}
		/** 呼び出しの実体
		 * @param pri プライオリティ 呼び出し先でこのスロットのプライオリティーを変更可能にするため
		 * @return 継続するか？(falseを返すとdisconnectされる)または、呼び出しを中断する（接続先のシグナル_Thunkがtrueのとき）
		 */
		virtual bool operator()(GCTP_SIGNAL_PARAM) = 0;
	};

	/** SignalN
	 *
	 * Signalの引数を増やしたバージョン\n
	 * Signal10まである。
	 * @see Signal0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<bool _Thunk, GCTP_SIGNAL_TEMPLATEPARAM>
	class GCTP_SIGNAL_NAME : public Signal
	{
	public:
		/// コンストラクタ
		GCTP_SIGNAL_NAME(uint16_t flags = SORT|MASK) : Signal(flags) {}
		/// Slotの型
		typedef GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM> SlotType;
		/// スロット接続
		GCTP_SIGNAL_NAME &operator<<(const SlotType &slot) { connect(slot); return *this; }
		/// スロット切断
		GCTP_SIGNAL_NAME &operator>>(const SlotType &slot) { disconnect(slot); return *this; }
		/// 関連付けられた関数をすべて呼び出す
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
	 * MonoSignalの引数を増やしたバージョン\n
	 * MonoSignal10まである。
	 * @see MonoSignal0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<GCTP_SIGNAL_TEMPLATEPARAM>
	class GCTP_MONOSIGNAL_NAME : public MonoSignal
	{
	public:
		/// Slotの型
		typedef GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM> SlotType;
		/// スロット接続
		GCTP_MONOSIGNAL_NAME &operator<<(const SlotType &slot) { connect(slot); return *this; }
		/// スロット切断
		GCTP_MONOSIGNAL_NAME &operator>>(const SlotType &slot) { disconnect(slot); return *this; }
		/// 関連付けられた関数をすべて呼び出す
		bool operator()(GCTP_SIGNAL_ARGTAGS)
		{
			Handle<SlotType> slot = slot_;
			if(slot) return (*slot)(GCTP_SIGNAL_ARGS);
			return false;
		}
	};

	/** SignalSlotN
	 *
	 * SignalSlotの引数を増やしたバージョン\n
	 * SignalSlot10まである。
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
		/// 関係付けられている関数の呼び出し
		virtual bool operator()(GCTP_SIGNAL_ARGTAGS)
		{
			bool handled = true, ret;
			ret = (*this)(handled, GCTP_SIGNAL_ARGS);
			if(!handled) signal_(GCTP_SIGNAL_ARGS);
			return ret;
		}
		/// スロット接続
		GCTP_SIGNALSLOT_NAME &operator<<(const SlotType &slot) { signal_.connect(slot); return *this; }
		/// スロット切断
		GCTP_SIGNALSLOT_NAME &operator>>(const SlotType &slot) { signal_.disconnect(slot); return *this; }
		
		/// シグナルオブジェクトを返す
		GCTP_SIGNAL_NAME<_Thunk, GCTP_SIGNAL_PARAM> &signal() { return signal_; }
		/// シグナルオブジェクトを返す
		const GCTP_SIGNAL_NAME<_Thunk, GCTP_SIGNAL_PARAM> &signal() const { return signal_; }

		/** 呼び出しの実体
		 * @param pri プライオリティ 呼び出し先でこのスロットのプライオリティーを変更可能にするため
		 * @return 継続するか？(falseを返すとdisconnectされる)または、呼び出しを中断する（接続先のシグナル_Thunkがtrueのとき）
		 */
		virtual bool operator()(bool &handled, GCTP_SIGNAL_ARGTAGS) = 0;

	private:
		GCTP_SIGNAL_NAME<_Thunk, GCTP_SIGNAL_PARAM> signal_;
	};

	/** FnSlotN
	 *
	 * FnSlot0の引数を増やしたバージョン\n
	 * Slot10まである。
	 * @see Slot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<GCTP_SIGNAL_TEMPLATEPARAM, bool (*Method)(GCTP_SIGNAL_PARAM)>
	class GCTP_FNSLOT_NAME : public GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>
	{
	public:
		/// コンストラクタ プライオリティーを指定する
		GCTP_FNSLOT_NAME(int16_t pri = 0) : GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>(pri) {}
		/// 関数呼び出し実装のインターフェース
		virtual bool operator()(GCTP_SIGNAL_ARGTAGS) { return (*Method)(GCTP_SIGNAL_ARGS); }
	};

	/** MemberSlotN
	 *
	 * MemberSlotの引数を増やしたバージョン\n
	 * MemberSlot10まである。
	 * @see MemberSlot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<class _T, GCTP_SIGNAL_TEMPLATEPARAM, bool (_T::*Method)(GCTP_SIGNAL_PARAM)>
	class GCTP_SIGNAL_MEMBERSLOTNAME : public GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>
	{
	public:
		/// コンストラクタ
		GCTP_SIGNAL_MEMBERSLOTNAME(int16_t pri = 0) : GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>(pri) {}
		/// コンストラクタ プライオリティーと関連付けるインスタンスを指定する
		GCTP_SIGNAL_MEMBERSLOTNAME(_T *_this, int16_t pri = 0) : GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>(pri), this_(_this) {}
		/// インスタンスを指定する
		void bind(_T *_this) { this_ = _this; }
		/// メンバ関数呼び出しの実装
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
	 * ConstMemberSlotの引数を増やしたバージョン\n
	 * ConstMemberSlot10まである。
	 * @see MemberSlot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<class _T, GCTP_SIGNAL_TEMPLATEPARAM, bool (_T::*Method)(GCTP_SIGNAL_PARAM) const>
	class GCTP_SIGNAL_CONSTMEMBERSLOTNAME : public GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>
	{
	public:
		/// コンストラクタ
		GCTP_SIGNAL_CONSTMEMBERSLOTNAME(int16_t pri = 0) : GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>(pri) {}
		/// コンストラクタ プライオリティーと関連付けるインスタンスを指定する
		GCTP_SIGNAL_CONSTMEMBERSLOTNAME(const _T *_this, int16_t pri = 0) : GCTP_SLOT_NAME<GCTP_SIGNAL_PARAM>(pri), this_(_this) {}
		/// インスタンスを指定する
		void bind(const _T *_this) { this_ = _this; }
		/// メンバ関数呼び出しの実装
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
	 * MemberSignalSlotの引数を増やしたバージョン\n
	 * MemberSignalSlot10まである。
	 * @see MemberSignalSlot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<bool _Thunk, class _T, GCTP_SIGNAL_TEMPLATEPARAM, bool (_T::*Method)(bool &, GCTP_SIGNAL_PARAM)>
	class GCTP_SIGNAL_MEMBERSIGNALSLOTNAME : public GCTP_SIGNALSLOT_NAME<_Thunk, GCTP_SIGNAL_PARAM>
	{
	public:
		/// コンストラクタ
		GCTP_SIGNAL_MEMBERSIGNALSLOTNAME(int16_t pri = 0) : GCTP_SIGNALSLOT_NAME<_Thunk, GCTP_SIGNAL_PARAM>(pri) {}
		/// コンストラクタ プライオリティーと関連付けるインスタンスを指定する
		GCTP_SIGNAL_MEMBERSIGNALSLOTNAME(_T *_this, int16_t pri = 0) : GCTP_SIGNALSLOT_NAME<_Thunk, GCTP_SIGNAL_PARAM>(pri), this_(_this) {}
		/// インスタンスを指定する
		void bind(_T *_this) { this_ = _this; }
		/// メンバ関数呼び出しの実装
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
	 * ConstMemberSignalSlotの引数を増やしたバージョン\n
	 * ConstMemberSignalSlot10まである。
	 * @see MemberSignalSlot0
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 * @date 2004/02/13 22:51:11
	 */
	template<bool _Thunk, class _T, GCTP_SIGNAL_TEMPLATEPARAM, bool (_T::*Method)(bool &, GCTP_SIGNAL_PARAM) const>
	class GCTP_SIGNAL_CONSTMEMBERSIGNALSLOTNAME : public GCTP_SIGNALSLOT_NAME<_Thunk, GCTP_SIGNAL_PARAM>
	{
	public:
		/// コンストラクタ
		GCTP_SIGNAL_CONSTMEMBERSIGNALSLOTNAME(int16_t pri = 0) : GCTP_SIGNALSLOT_NAME<_Thunk, GCTP_SIGNAL_PARAM>(pri) {}
		/// コンストラクタ プライオリティーと関連付けるインスタンスを指定する
		GCTP_SIGNAL_CONSTMEMBERSIGNALSLOTNAME(const _T *_this, int16_t pri = 0) : GCTP_SIGNALSLOT_NAME<_Thunk, GCTP_SIGNAL_PARAM>(pri), this_(_this) {}
		/// インスタンスを指定する
		void bind(const _T *_this) { this_ = _this; }
		/// メンバ関数呼び出しの実装
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
