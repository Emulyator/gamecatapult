#ifndef _GCTP_FIXED_HPP_
#define _GCTP_FIXED_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * �Œ菬���N���X with boost
 *
 * @author shinichiro.h (http://shinh.skr.jp/template/gamenum.html)
 * Copyright (C) 2002 shinichiro.h (http://shinh.skr.jp/template/gamenum.html)
 * modified by SowwaRay<sowwaNO_SPAM_THANKS@water.sannet.ne.jp>
 */
#include <boost/mpl/if.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>
#include <cmath>

namespace gctp {

	/** �Œ菬���N���X�e���v���[�g
	 *
	 * �������r�b�g�����p�����[�^�œn����Œ菬���N���X�̃e���v���[�g
	 * @param _Q �������̃r�b�g��
	 * @param _Var �i�[����ϐ��^�i�f�t�H���g��int�j
	 * @param _BigVar �揜����ۂ̍�ƕϐ��̌^�i�f�t�H���g��long int�j
	 * @param _Int �ϊ����鐮���^�i�f�t�H���g��int�j
	 * @param _Float �ϊ����镂�������^�i�f�t�H���g��double�j
	 * @author shinichiro.h (http://shinh.skr.jp/template/gamenum.html)
     * modified by SowwaRay<sowwaNO_SPAM_THANKS@water.sannet.ne.jp>
	 */
	template <int _Q, typename _Var =int, typename _BigVar =long long, typename _Int =int, typename _Float =double>
	struct Fixed {
		BOOST_STATIC_ASSERT(boost::is_integral<_Var>::value);
		BOOST_STATIC_ASSERT(boost::is_integral<_BigVar>::value);
		BOOST_STATIC_ASSERT(boost::is_integral<_Int>::value);

		/// �R���p�C���^�C���ȃV�t�g�ϊ��B
		/**
		 * ���s���x�ɉe�����Ȃ��B
		 */
		//@{
		struct NoShiftPolicy {
			static _Var shift(_Var v) {
				return v;
			}
		};
		template<int _Q>
		struct LeftShiftPolicy {
			static _Var shift(_Var v) {
				return v << _Q;
			}
		};
		template<int _Q>
		struct RightShiftPolicy {
			static _Var shift(_Var v) {
				return v >> _Q;
			}
		};

		template<int _leftQ, int _rightQ>
		static _Var convert(_Var v) {
			return
				boost::mpl::if_c <
   					(_rightQ == _leftQ),
					NoShiftPolicy,
					boost::mpl::if_c <
    		  			(_leftQ > _rightQ),
						LeftShiftPolicy<_leftQ-_rightQ>,
						RightShiftPolicy<_rightQ-_leftQ>
					>::type
				>::type::shift(v);
		}
		//@}

		static _Var convert(_Float v) {
			return static_cast<_Var>(v * (1 << _Q));
		}

		Fixed() {}
		Fixed(_Float src) : val(convert(src)) {}
		Fixed(_Int src) : val(src << _Q) {}
		Fixed(int, _Int src) : val(src) {}
		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		Fixed(Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> src) : val(convert<_Q, _Q2>(src.val)) {}

		operator _Int() const { return val >> _Q; }
		operator _Float() const { return _Float(val) / (1 << _Q); }

		Fixed operator -() const {
			return Fixed(0, -val);
		}

		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		Fixed& operator += (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) {
			val += convert<_Q, _Q2>(rhs.val);
			return *this;
		}
		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		Fixed operator + (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) const {
			return Fixed(*this)+=rhs;
		}

		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		Fixed& operator -= (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) {
			val -= convert<_Q, _Q2>(rhs.val);
			return *this;
		}
		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		Fixed operator - (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) const {
			return Fixed(*this)-=rhs;
		}

		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		Fixed& operator *= (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) {
			val = _Var((_BigVar(val) * _BigVar(rhs.val))>>_Q2);
			return *this;
		}
		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		Fixed operator * (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) const {
			return Fixed(*this)*=rhs;
		}

		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		Fixed& operator /= (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) {
			val = _Var((_BigVar(val)<<_Q2)/_BigVar(rhs.val));
			return *this;
		}
		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		Fixed operator / (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) const {
			return Fixed(*this)/=rhs;
		}

		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		Fixed& operator %= (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs ) {
			val -= _Var(rhs.val)*_Int((*this)/rhs);
			return *this;
		}
		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		Fixed operator % (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs ) const {
			return Fixed(*this)%=rhs;
		}
		
		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		bool operator > (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) const {
			return val > convert<_Q, _Q2>(rhs.val);
		}
		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		bool operator < (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) const {
			return val < convert<_Q, _Q2>(rhs.val);
		}
		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		bool operator <= (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) const {
			return val <= convert<_Q, _Q2>(rhs.val);
		}
		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		bool operator >= (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) const {
			return val >= convert<_Q, _Q2>(rhs.val);
		}
		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		bool operator == (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) const {
			return val == convert<_Q, _Q2>(rhs.val);
		}
		template<int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
		bool operator != (Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> rhs) const {
			return val != convert<_Q, _Q2>(rhs.val);
		}

		_Var val;
	};

	// std�ɓ����ׂ�����

	template <int _Q, typename _Var, typename _BigVar, typename _Int, typename _Float>
	inline Fixed<_Q, _Var, _BigVar, _Int, _Float> abs(Fixed<_Q, _Var, _BigVar, _Int, _Float> v)
	{
		return Fixed<_Q, _Var, _BigVar, _Int, _Float>(0, std::abs(v.val));
	}
	template <int _Q, typename _Var, typename _BigVar, typename _Int, typename _Float>
	inline Fixed<_Q, _Var, _BigVar, _Int, _Float> fabs(Fixed<_Q, _Var, _BigVar, _Int, _Float> v)
	{
		return Fixed<_Q, _Var, _BigVar, _Int, _Float>(0, std::abs(v.val));
	}
	template <int _Q1, typename _Var1, typename _BigVar1, typename _Int1, typename _Float1
		, int _Q2, typename _Var2, typename _BigVar2, typename _Int2, typename _Float2>
	inline Fixed<_Q1, _Var1, _BigVar1, _Int1, _Float1> fmod(
		Fixed<_Q1, _Var1, _BigVar1, _Int1, _Float1> rhs,
		Fixed<_Q2, _Var2, _BigVar2, _Int2, _Float2> lhs)
	{
		return rhs%lhs;
	}
	template <int _Q, typename _Var, typename _BigVar, typename _Int, typename _Float>
	inline Fixed<_Q, _Var, _BigVar, _Int, _Float> ceil(Fixed<_Q, _Var, _BigVar, _Int, _Float> v)
	{
		return Fixed<_Q, _Var, _BigVar, _Int, _Float>(0, (v.val&(_Var(-1)<<_Q))+((v.val&(_Var((1<<_Q)-1)))?(1<<_Q):0));
	}
	template <int _Q, typename _Var, typename _BigVar, typename _Int, typename _Float>
	inline Fixed<_Q, _Var, _BigVar, _Int, _Float> floor(Fixed<_Q, _Var, _BigVar, _Int, _Float> v)
	{
		return Fixed<_Q, _Var, _BigVar, _Int, _Float>(0, v.val&(_Var(-1)<<_Q));
	}

}

#endif //_GCTP_FIXED_HPP_