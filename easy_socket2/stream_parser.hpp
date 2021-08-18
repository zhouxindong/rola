#ifndef ROLA_EASY_SOCKET2_STREAM_PARSER_HPP
#define ROLA_EASY_SOCKET2_STREAM_PARSER_HPP

#include <mutex>
#include <deque>
#include <vector>
#include <algorithm>

#include "easy_signal/easy_signal.hpp"
#include "utils/byte_order.hpp"

namespace rola
{
	enum class matched_state
	{
		unknown = 0,

		squize_len,
		more_data,

		to_find_head,
		to_find_tail, 
	};

	/// <summary>
	/// stream data parser
	/// </summary>
	class stream_parser
	{
	protected:
		std::deque<char> data;
		Easy_signal<void(std::vector<char>&)> matched;

	public:
		Easy_signal<void(std::vector<char>&)>& matched_signal()
		{
			return matched;
		}

	public:

		virtual ~stream_parser() = default;
		virtual stream_parser* clone() = 0;

	public:
		void push(const char* buf, size_t len)
		{
			for (size_t i = 0; i < len; ++i)
			{
				data.push_back(buf[i]);
			}

			process_();
		}

	private:
		virtual void process_() = 0;
	};

	/// <summary>
	/// fixed len frame data
	/// </summary>
	template <size_t Len>
	class fixed_len_stream : public stream_parser
	{
	public:
		stream_parser* clone() override
		{
			return new fixed_len_stream(*this);
		}

	private:
		void process_() override
		{
			if (data.size() < Len)
				return;

			std::vector<char> v;
			for (size_t i = 0; i < Len; ++i)
			{
				v.push_back(data.front());
				data.pop_front();
			}

			matched.emit_(/*std::move(v)*/v);

			process_();
		}
	};

	/// <summary>
	/// len(4 bytes) + data
	/// </summary>
	class data_len_stream : public stream_parser
	{
	public:
		data_len_stream* clone() override
		{
			return new data_len_stream(*this);
		}

	private:
		void process_() override
		{
			switch (state_)
			{
			case matched_state::squize_len:
				squlize_len_state_();
				break;

			case matched_state::more_data:
				more_data_state_();
				break;
			}
		}

		void squlize_len_state_()
		{
			if (data.size() < 4)
				return;

			char len[4];
			for (auto i = 0; i < 4; ++i)
			{
				len[i] = data.front();
				data.pop_front();
			}

			left_len_ = network_to_host<int>(len);
			more_data_state_();
		}

		void more_data_state_()
		{
			auto size = data.size();

			if (size < left_len_) // no enough data
			{
				for (auto i = 0; i < size; ++i)
				{
					v_.push_back(data.front());
					data.pop_front();
				}
				state_ = matched_state::more_data;
				left_len_ -= size;
			}
			else // enough data
			{
				for (auto i = 0; i < left_len_; ++i)
				{
					v_.push_back(data.front());
					data.pop_front();
				}
				matched.emit_(v_);
				v_.clear();

				state_ = matched_state::squize_len;
				left_len_ = 0;
				if (data.size() > 0)
					squlize_len_state_();
			}
		}

	private:
		matched_state state_{ matched_state::squize_len };
		size_t left_len_{ 0 };
		std::vector<char> v_;
	};

	/// <summary>
	/// head + data + tail
	/// </summary>
	template <size_t Max = 4096>
	class head_tail_stream : public stream_parser
	{
	public:
		head_tail_stream(std::initializer_list<char> head,
			std::initializer_list<char> tail)
			: head_(head)
			, tail_(tail)
			, head_size_{ head_.size() }
			, tail_size_{ tail_.size() }
		{}

		head_tail_stream* clone() override
		{
			return new head_tail_stream(*this);
		}

	private:
		void process_() override
		{
			while (!data.empty())
			{
				switch (state_)
				{
				case matched_state::to_find_head:
					find_head_();
					break;

				case matched_state::to_find_tail:
					find_tail_();
					break;
				}
				if (need_more_) break;
			}
		}

		void find_head_()
		{
			need_more_ = false;
			if (data.size() < head_.size())
			{
				need_more_ = true;
				return;
			}

			if (std::equal(head_.cbegin(), head_.cend(), data.cbegin())) // find head
			{
				for (auto i = 0; i < head_size_; ++i)
				{
					data.pop_front();
				}
				state_ = matched_state::to_find_tail;
			}
			else
				data.pop_front();
		}

		void find_tail_()
		{
			need_more_ = false;
			if (data.size() < tail_.size())
			{
				need_more_ = true;
				return;
			}

			if (std::equal(tail_.cbegin(), tail_.cend(), data.cbegin())) // find tail
			{
				for (auto i = 0; i < tail_size_; ++i)
					data.pop_front();
				matched.emit_(v_);
				v_.clear();
				state_ = matched_state::to_find_head;
			}
			else
			{
				if (v_.size() > Max) // lost tail, occur error
				{
					v_.clear();
					state_ = matched_state::to_find_head;
				}
				else
				{
					v_.push_back(data.front());
					data.pop_front();
				}
			}
		}

	private:
		std::vector<char> head_;
		std::vector<char> tail_;
		size_t head_size_;
		size_t tail_size_;
		bool need_more_{ false };
		std::vector<char> v_;
		matched_state state_{ matched_state::to_find_head };
	};

} // namespace rola

#endif // ROLA_EASY_SOCKET2_STREAM_PARSER_HPP
