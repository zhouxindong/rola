#ifndef ROLA_STLEX_THREAD_HPP
#define ROLA_STLEX_THREAD_HPP

#include <future>
#include <chrono>

namespace rola
{
	namespace stlex
	{
#pragma region async_defer

		template <typename Result>
		inline bool is_defered(std::future<Result> const& f)
		{
			return f.wait_for(std::chrono::seconds(0)) == std::future_status::deferred;
		}

		inline bool is_defered(std::future<void> const& f)
		{
			return f.wait_for(std::chrono::seconds(0)) == std::future_status::deferred;
		}

		template <typename F, typename ...Ts>
		inline std::future<typename std::result_of_t<F(Ts...)>> 
			really_async(F&& f, Ts&&... params)
		{
			return std::async(
				std::launch::async,
				std::forward<F>(f),
				std::forward<Ts>(params)...);
		}

#pragma endregion

#pragma region thread_raii

		class thread_raii
		{
		public:
			enum class dtor_action { join, detach };

			thread_raii(std::thread&& t, dtor_action a)
				: action_(a)
				, t_(std::move(t)) {}

			~thread_raii()
			{
				if (t_.joinable())
				{
					if (action_ == dtor_action::join)
						t_.join();
					else if (action_ == dtor_action::detach)
						t_.detach();
				}
			}

			thread_raii(thread_raii&&) = default;
			thread_raii& operator=(thread_raii&&) = default;

		public:
			std::thread& get() { return t_; }

		private:
			dtor_action action_;
			std::thread t_; // always be the lastest! cecause thread after init would be start
							// so let everything to be ready
		};

#pragma endregion

	} // namespace stlex
} // namespace rola

#endif // !ROLA_STLEX_THREAD_HPP
