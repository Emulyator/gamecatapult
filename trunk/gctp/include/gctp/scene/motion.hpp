#ifndef _GCTP_SCENE_MOTION_HPP_
#define _GCTP_SCENE_MOTION_HPP_
/** @file
 * GameCatapult モーションクラスヘッダファイル
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/01/29 20:23:48
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 */
#include <gctp/vector.hpp>
#include <gctp/quat.hpp>
#include <gctp/matrix.hpp>
#include <gctp/pointervector.hpp>
#include <gctp/cstr.hpp>
#include <map>

namespace gctp {
	struct Stance;
	struct Coord;
	class Skeleton;
}

namespace gctp { namespace scene {
	/**モーションデータクラス
	 *
	 * @todo 補完を、補完対象にget2PInterporation/get4PInterporationメンバ関数を要求するのではなく、
	 * Interporator/QuadInterporator関数オブジェクトを引数として受け取って、それで行うようにする。
	 * そうすればHermiteとCutmull-Romを動的に切り替えられる。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:23:34
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class MotionChannel : public Object
	{
	public:
		/// 補間タイプ定数
		enum PosType {
			SPLINE,			///< スプライン補間
			LINEAR,			///< 線形補間
			NONE,			///< 補間なし（Ｘファイルの定義には無い）
			DEFAULT_POSTYPE	///< モーションデータの値をそのまま使う
		};
		/// 終端形式定数
		enum IsOpen {
			CLOSE,			///< 閉じている
			OPEN,			///< 開いている
			DEFAULT_ISOPEN	///< モーションデータの値をそのまま使う
		};

		/** @name アニメーションデータクラス
		 * アニメーションデータの型定義
		 */
		/* @{ */
		struct Key {
			/// キータイム
			ulong			time;
		};

		template<typename _T>
		struct TKey : Key {
			typedef _T ValType;
			_T val;
		};
		
		/// AnimationKeyチャンクの型
		class Keys {
		public:
			enum Type {
				SCALE,
				POSTURE,
				YPR,
				POSITION,
				MATRIX
			};
			Keys(Type type) : type_(type) {}
			virtual ~Keys() {}
			Type type() { return type_; }
			virtual uint num() = 0;
			virtual Key *get(uint i) = 0;
			virtual ulong time() = 0;

			/** キータイムにマッチするデータのインデックスを返す
			 * バイナリーサーチなので、昇順に並んでいる必要あり。
			 *
			 * 検索範囲の脚きりは、今度ゆっくり考えよう…
			 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
			 * @date 2004/10/11 5:34:36
			 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
			 */
			int findIdx(float key) {
				int left = 0, mid, right = num()-1;
				while(left < right) {
					mid = (left + right) / 2;
					uint upper = get(mid+1)->time;
					uint lower = get(mid)->time;
					if(lower <= key && key < upper) return mid;
					else if(lower < key) left = mid + 1;
					else right = mid - 1;
				}
				return (left>0)?right:left;
			}
			virtual Key *getKey(float key) = 0;
		private:
			Type type_; // 実行時型情報にすべき？
		};
		
		/// アニメーションキー実体定義テンプレート
		template<class T, class _VT>
		class KeyArray : public Keys {
		public:
			KeyArray(Type type, size_t size) : Keys(type), data(size) {}
			T &operator [](uint n) {
				return data[n];
			}
			std::size_t num() {
				return data.size();
			}
			Key *get(uint i) {
				return &data[i];
			}
			ulong time() {
				return data.back().time;
			}
			Key *getKey(float key) {
				return get(findIdx(key));
			}
			/// 補間なしのキー値を返す。
			bool getKeyData(_VT &ret, bool is_open, float key) {
				if(0 == num()) return false;
				if(is_open) {
					ret = data[findIdx(key)].val;
				}
				else {
					float _key = fmodf(key, static_cast<float>(time()));
					ret = data[findIdx(_key)].val;
				}
				return true;
			}
			/// ２個のキー値を使った補間値を返す。
			bool get2PInterpolation(_VT &ret, bool is_open, float key) {
				if(0 == num()) return false;
				if(1 == num()) ret = data[0].val;
				else {
					if(is_open) {
						uint i = findIdx(key);
						if((num()-1)==i) { // 最後のキー要素の場合
							ret = data[i].val;
						}
						else {
							ret.set2PInterpolation(data[i].val, data[i+1].val,
								(key-data[i].time)/(data[i+1].time-data[i].time));
						}
					}
					else {
						float _key = fmodf(key, static_cast<float>(time()));
						uint i = findIdx(_key);
						if(i == num()-1) i = 0;
						ret.set2PInterpolation(data[i].val, data[i+1].val,
							(_key-data[i].time)/(data[i+1].time-data[i].time));
					}
				}
				return true;
			}
			/// ４個のキー値を使った補間値を返す。
			bool get4PInterpolation(_VT &ret, bool is_open, float key) {
				if(num() < 4) return get2PInterpolation(ret, is_open, key);
				if(is_open) {
					if(key>time()) {
						ret = data.back().val;
					}
					else {
						T *d[4];
						ulong i = findIdx(key);
						d[0] = &data[(i>0)?i-1:0];
						d[1] = &data[i];
						d[2] = &data[(i+1>=num())?num()-1:i+1];
						d[3] = &data[(i+2>=num())?num()-1:i+2];
						ret.set4PInterpolation(d[0]->val,d[1]->val,d[2]->val,d[3]->val,
							(key-d[1]->time)/(d[2]->time-d[1]->time));
					}
				}
				else {
					float _key = fmodf(key, static_cast<float>(time()));
					uint i = findIdx(_key);
					ret.set4PInterpolation(
						data[(i>0)?(i-1):(num()-1)].val, data[i].val,
						data[(i+1)%num()].val, data[(i+2)%num()].val,
						(_key-data[i].time)/(data[(i+1)%num()].time-data[i].time));
				}
				return true;
			}
		protected:
			std::vector<T> data;
		};
		/* @} */
		
		/** @name アニメーションキー配列クラス
		 */
		/* @{ */
		typedef KeyArray<TKey<Vector>,Vector>	PositionKeys;
		typedef KeyArray<TKey<Quat>,Quat>		PostureKeys;
		typedef KeyArray<TKey<Vector>,Vector>	YPRKeys;
		typedef KeyArray<TKey<Vector>,Vector>	ScaleKeys;
		typedef KeyArray<TKey<Matrix>,Matrix>	MatrixKeys;
		/* @} */
		
		MotionChannel() : is_open_(false), position_type_(SPLINE) {}
		
		void setKeys(Keys *p) { p_ = std::auto_ptr<Keys>(p); }
		Keys *getKeys() { return p_.get(); }
		const Keys *getKeys() const { return p_.get(); }
		ulong time() const { return p_->time(); }
		int num() const { return p_->num(); }
		int getIdxFromKey(float key) const { return p_->findIdx(key); }

		bool isOpen() const {return is_open_;}
		bool setIsOpen(bool is_open) { std::swap(is_open_, is_open); return is_open; }
		PosType posType() const {return position_type_; }
		PosType setPosType(PosType position_type) { std::swap(position_type_, position_type); return position_type; }
		Keys::Type keyType() const { return p_.get()->type(); }
		
		void get(Stance &stance, PosType custom_postype, IsOpen custom_is_open, float keytime) const;
		void get(Coord &coord, PosType custom_postype, IsOpen custom_is_open, float keytime) const;
		void get(Matrix &mat, PosType custom_postype, IsOpen custom_is_open, float keytime) const;

	GCTP_DECLARE_TYPEINFO

	private:
		bool is_open_;
		PosType position_type_;
		std::auto_ptr<Keys> p_;
	};

	typedef PointerVector<MotionChannel> MotionChannelVector;

	/** モーションデータとインデックスのセット
	 *
	 * @todo std::mapによる実装を変えたいなぁ
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/01/29 20:29:13
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	class Motion : public Object
	{
	public:
		typedef std::map<CStr, MotionChannelVector>::iterator iterator;
		typedef std::map<CStr, MotionChannelVector>::const_iterator const_iterator;

		void set(CStr name, MotionChannelVector &channels);
		void setPosType(const MotionChannel::PosType postype);
		void setIsOpen(bool is_open);
		void setTicksPerSec(float ticks_per_sec);
		float ticksPerSec() const;
		float time() const;
		ulong ticktime() const;
		const_iterator begin() const { return map_.begin(); }
		iterator begin() { return map_.begin(); }
		const_iterator end() const { return map_.end(); }
		iterator end() { return map_.end(); }
		const_iterator find(const char *name) const { return map_.find(name); }
		iterator find(const char *name) { return map_.find(name); }
		/// モーションをセット
		void applyTo(Skeleton &skelton, Real keytime);

	GCTP_DECLARE_TYPEINFO

	protected:
		std::map<CStr, MotionChannelVector> map_;
	private:
		float ticks_per_sec_;
	};

}} // namespace gctp::scene

#endif //_GCTP_SCENE_MOTION_HPP_