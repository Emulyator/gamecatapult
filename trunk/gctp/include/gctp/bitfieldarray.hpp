#ifndef _GCTP_BITFIELDARRAY_HPP_
#define _GCTP_BITFIELDARRAY_HPP_
#include <gctp/config.hpp>
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/** @file
 * GameCatapult 固定長ビットフィールド配列テンプレート
 *
 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
 * @date 2004/02/13 23:00:48
 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
 * GameCatapultライセンス(http://www.water.sannet.ne.jp/sowwa/doc/license.html)
 * にもとづいて、使用・改変・再配布は自由に行って構いません。
 */
#include <cstddef> // for std::size_t

namespace gctp {

	/** 固定長ビットフィールド配列テンプレート
	 *
	 * std::bitsetの、1以上のビット長版\n
	 * _LenはBYTE_BIT以下である必要がある。
	 * @author SAM (T&GG, Org.)<sowwa_NO_SPAM_THANKS@water.sannet.ne.jp>
	 * @date 2004/02/13 23:00:17
	 * Copyright (C) 2001,2002,2003,2004 SAM (T&GG, Org.). All rights reserved.
	 */
	template<int _Bit, int _Len, typename ValueType = int, typename ByteType = unsigned char>
	struct BitFieldArray {
		enum {
			BYTE_BIT = sizeof(ByteType)*8,
			BIT_MASK = ((1<<_Bit)-1),
			BYTE_MASK = ((1<<BYTE_BIT)-1),
			ARRAY_LEN = (_Bit*_Len+BYTE_BIT-1)/BYTE_BIT,
			_FORCE_SIZE_T_ = 1<<(sizeof(std::size_t)*8-2)
		};
		
		class ConstField {
		public:
			ConstField(const BitFieldArray &subject, std::size_t pos)
				: subject_(subject)
				, begin_index_((pos*_Bit)/BYTE_BIT)
				, end_index_((pos*_Bit+_Bit-1)/BYTE_BIT)
				, bit_offset_(pos*_Bit - begin_index_*BYTE_BIT)
			{
			}
			
			operator ValueType() const
			{
				if(begin_index_==end_index_) {
					return static_cast<ValueType>(
						(subject_.bytes[begin_index_]>>bit_offset_) & BIT_MASK
						);
				}
				else {
					return static_cast<ValueType>(
						((subject_.bytes[begin_index_]>>bit_offset_)
						  | (subject_.bytes[end_index_]<<(BYTE_BIT-bit_offset_))
						& BIT_MASK)
					);
				}
			}

		protected:
			const BitFieldArray &subject_;
			std::size_t begin_index_;
			std::size_t end_index_;
			std::size_t bit_offset_;
		};
		
		class Field : public ConstField {
		public:
			Field(const BitFieldArray &subject, std::size_t pos)
				: ConstField(subject, pos)
			{
			}

			const Field &operator=(ValueType value) const
			{
				BitFieldArray &subject = const_cast<BitFieldArray &>(subject_);
				subject.bytes[begin_index_] = static_cast<ByteType>(
					(subject.bytes[begin_index_]&(~(BIT_MASK<<bit_offset_)))
					| ((value&BIT_MASK)<<bit_offset_)
					);
				if(begin_index_!=end_index_) {
					subject.bytes[end_index_] = static_cast<ByteType>(
						(subject.bytes[end_index_]&(~((1<<(_Bit+bit_offset_-BYTE_BIT))-1)))
						| ((value&BIT_MASK)>>(BYTE_BIT-bit_offset_))
						);
				}
				return *this;
			}
		};
		
		ByteType bytes[ARRAY_LEN];
		
		ConstField operator[](std::size_t pos) const
		{
			return ConstField(*this, pos);
		}
		
		Field operator[](std::size_t pos)
		{
			return Field(*this, pos);
		}
		
		template<int _Pos>
		void set(ValueType value)
		{
			const std::size_t begin_index = ((_Pos*_Bit)/BYTE_BIT);
			const std::size_t end_index = ((_Pos*_Bit+_Bit-1)/BYTE_BIT);
			const std::size_t bit_offset = (_Pos*_Bit - begin_index*BYTE_BIT);
			bytes[begin_index] = static_cast<ByteType>(
				(bytes[begin_index]&(~(BIT_MASK<<bit_offset)))
				| ((value&BIT_MASK)<<bit_offset)
				);
			if(begin_index!=end_index) {
				bytes[end_index] = static_cast<ByteType>(
					(subject.bytes[end_index]&(~((1<<(_Bit+bit_offset-BYTE_BIT))-1)))
					| ((value&BIT_MASK)>>(BYTE_BIT-bit_offset))
					);
			}
		}

		template<int _Pos>
		ValueType get()
		{
			const std::size_t begin_index = ((_Pos*_Bit)/BYTE_BIT);
			const std::size_t end_index = ((_Pos*_Bit+_Bit-1)/BYTE_BIT);
			const std::size_t bit_offset = (_Pos*_Bit - begin_index*BYTE_BIT);
			if(begin_index==end_index) {
				return static_cast<ValueType>(
					(bytes[begin_index]>>bit_offset) & BIT_MASK
					);
			}
			else {
				return static_cast<ValueType>(
					((bytes[begin_index]>>bit_offset)
					  | (bytes[end_index]&((1<<(_Bit+bit_offset-BYTE_BIT))-1)))
					& BIT_MASK
					);
			}
		}
	};

} // namespace gctp

#endif // _GCTP_BITFIELDARRAY_HPP_
