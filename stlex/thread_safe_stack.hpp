#ifndef ROLA_STLEX_THREAD_SAFE_STACK_HPP
#define ROLA_STLEX_THREAD_SAFE_STACK_HPP

#include <exception>
#include <stack>
#include <mutex>
#include <memory>

namespace rola
{
	struct Empty_stack : std::exception
	{
		const char* what() const
		{
			return "empty stack";
		}
	};

	/// <summary>
	/// a thread safe stack based on mutex
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class Thread_safe_stack
	{
	private:
		std::stack<T> data_;
		mutable std::mutex m_;

	public:
		Thread_safe_stack()
		{}

		Thread_safe_stack(Thread_safe_stack const& other)
		{
			std::lock_guard<std::mutex> lock(other.m_);
			data_ = other.data_;
		}

		Thread_safe_stack& operator=(Thread_safe_stack const&) = delete;

		void push(T new_value)
		{
			std::lock_guard<std::mutex> lock(m_);
			data.push(std::move(new_value));
		}

		std::shared_ptr<T> pop()
		{
			std::lock_guard<std::mutex> lock(m_);
			if (data.empty())
				throw Empty_stack();

			std::shared_ptr<T> const res(std::make_shared<T>(std::move(data_.top())));
			data_.pop();
			return res;
		}

		void pop(T& value)
		{
			std::lock_guard<std::mutex> lock(m_);
			if (data_.empty()) throw Empty_stack();
			value = std::move(data.top());
			data.pop();
		}

		bool empty() const
		{
			std::lock_guard<std::mutex> lock(m_);
			return data_.empty();
		}
	};
} // namespace rola

#endif // !ROLA_STLEX_THREAD_SAFE_STACK_HPP
