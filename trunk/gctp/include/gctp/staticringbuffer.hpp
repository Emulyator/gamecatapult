#ifndef _GCTP_STATICRINGBUFFER_HPP_
#define _GCTP_STATICRINGBUFFER_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file
 * サイズ固定のキューテンプレート
 *
 */

namespace gctp {

	/// 固定サイズのリングバッファ
	template<typename _T, int _MAX_SIZE>
	class StaticRingBuffer {
	public:
		enum Const {
			MAX_SIZE = _MAX_SIZE
		};
		typedef _T ValueType;
		StaticRingBuffer() : front_(0), size_(0) {}
		void clear()
		{
			front_ = 0;
			size_ = 0;
			for(int i = 0; i < MAX_SIZE; i++) values_[i] = ValueType();
		}
		void push(const _T &v)
		{
			push();
			back() = v;
		}
		void push()
		{
			if(size_==MAX_SIZE) pop();
			size_++;
		}
		void pop()
		{
			if(size_>0) {
				size_--;
				front_++;
				if(front_ >= MAX_SIZE) front_ = 0;
			}
		}
		std::size_t size()
		{
			return size_;
		}
		bool empty()
		{
			return size_ == 0;
		}
		_T &operator[](std::size_t i)
		{
			return values_[(i+front_)%MAX_SIZE];
		}
		const _T &operator[](std::size_t i) const
		{
			return values_[(i+front_)%MAX_SIZE];
		}
		_T &front()
		{
			return values_[front_];
		}
		const _T &front() const
		{
			return values_[front_];
		}
		_T &back()
		{
			if(empty()) return front();
			return (*this)[size_-1];
		}
		const _T &back() const
		{
			if(empty()) return front();
			return (*this)[size_-1];
		}

	private:
		_T values_[MAX_SIZE];
		int front_;
		std::size_t size_;
	};

} // namespace gctp
#endif // _GCTP_STATICRINGBUFFER_HPP_
