#ifndef _GCTP_STATICQUEUE_HPP_
#define _GCTP_STATICQUEUE_HPP_
#ifdef GCTP_ONCE
#pragma once
#endif // GCTP_ONCE
/**@file
 * サイズ固定のキューテンプレート
 *
 */

namespace gctp {

	/// 固定サイズのキュー
	template<typename _T, int _MAX_SIZE>
	class StaticQueue {
	public:
		enum Const {
			MAX_SIZE = _MAX_SIZE,
		};
		StaticQueue() : begin_(0), size_(0) {}
		bool push(const _T &v)
		{
			if(size_<MAX_SIZE) {
				size_++;
				(*this)[(std::size_t)(size_-1)] = v;
				return true;
			}
			return false;
		}
		bool push()
		{
			if(size_<MAX_SIZE) {
				size_++;
				return true;
			}
			return false;
		}
		void pop()
		{
			if(size_>0) {
				size_--;
				begin_++;
				if(begin_ >= MAX_SIZE) begin_ = 0;
			}
		}
		std::size_t size()
		{
			return size_;
		}
		_T &top()
		{
			return values_[begin_];
		}
		const _T &top() const
		{
			return values_[begin_];
		}
		_T &operator[](std::size_t i)
		{
			return values_[(i+begin_)%MAX_SIZE];
		}
		const _T &operator[](std::size_t i) const
		{
			return values_[(i+begin_)%MAX_SIZE];
		}

	private:
		_T values_[MAX_SIZE];
		int begin_;
		std::size_t size_;
	};

} // namespace gctp
#endif // _GCTP_STATICQUEUE_HPP_
