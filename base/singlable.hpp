#ifndef ROLA_BASE_SINGLABLE_HPP
#define ROLA_BASE_SINGLABLE_HPP

#include <utility>
#include <mutex>

namespace rola
{
	namespace base
	{
		/*
		Singlable<Wrapped> can be a class sington to Wrapped object
		*/
		template <typename Wrapped>
		class Singlable
		{
		public:
			Singlable()								= delete;
			Singlable(Singlable const&)				= delete;
			Singlable(Singlable&&)					= delete;
			Singlable& operator=(Singlable const&)	= delete;
			Singlable& operator=(Singlable&&)		= delete;

		public:
			template <typename ...Args>
			static Wrapped* instance_p(Args&&... args)
			{
				return instance_(std::forward<Args>(args)...);
			}

		private:
			static Wrapped* s_instance_ptr_;
			static char* s_place_;
			static std::mutex s_mutex_;

		private:
			static void alloc_()
			{
				s_place_ = new char[sizeof(Wrapped)];
			}

			static void free_()
			{
				(reinterpret_cast<Wrapped*>(s_place_))->~Wrapped();
				delete[] s_place_;
			}

			template <typename ...Args>
			static Wrapped* instance_(Args&&... args)
			{
				if (s_instance_ptr_ == nullptr)
				{
					std::lock_guard<std::mutex> lock(s_mutex_);
					if (s_instance_ptr_ == nullptr)
					{
						alloc_();
						new (s_place_) Wrapped(std::forward<Args>(args)...);
						s_instance_ptr_ = reinterpret_cast<Wrapped*>(s_place_);
						std::atexit(&Singlable::free_);
					}
				}
				return s_instance_ptr_;
			}
		};

		template <typename Wrapped>
		Wrapped* Singlable<Wrapped>::s_instance_ptr_ = nullptr;

		template <typename Wrapped>
		char* Singlable<Wrapped>::s_place_ = nullptr;

		template <typename Wrapped>
		std::mutex Singlable<Wrapped>::s_mutex_;
	} // namespace base
} // namespace rola

#endif // !ROLA_BASE_SINGLABLE_HPP
