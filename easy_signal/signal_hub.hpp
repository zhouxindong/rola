#ifndef ROLA_EASY_SIGNAL_SIGNAL_HUB_HPP
#define ROLA_EASY_SIGNAL_SIGNAL_HUB_HPP

#include "easy_signal.hpp"

#include <unordered_map>
#include <string>
#include <mutex>

namespace rola
{
	// primary
	template <typename Signature>
	class Signal_hub;

	// specialized with R(Args...)
	template <typename R, typename ...Args>
	class Signal_hub<R(Args...)>
	{
	public:
		Easy_signal<R(Args...)>* get_signal(std::string const& sig_name)
		{
			if (sig_name == "")
				return nullptr;

			std::lock_guard<std::mutex> lg(mtx_);

			if (signals_.find(sig_name) == signals_.end())
				signals_[sig_name] = Easy_signal<R(Args...)>();
			return &signals_[sig_name];
		}

	private:
		std::unordered_map<std::string, Easy_signal<R(Args...)>> signals_;
		std::mutex mtx_;
	};
} // namespace rola

#endif // !ROLA_EASY_SIGNAL_SIGNAL_HUB_HPP
