#ifndef ROLA_EASY_SIGNAL_EASY_SIGNAL_HPP
#define ROLA_EASY_SIGNAL_EASY_SIGNAL_HPP

#include <functional>
#include <vector>
#include <algorithm>
#include <utility>
#include <type_traits>

#include <mpl/easy_bind.hpp>
#include <utils/guid.hpp>
#include <stlex/container.hpp>

namespace rola
{
#pragma region class Easy_connect

	// fwd
	template <typename Signature>
	class Easy_signal;

	/*
	* Easy_connect
	*/
	template <typename Signature>
	class Easy_connect;

	template <typename R, typename... Args>
	class Easy_connect<R(Args...)>
	{
	public: // ctor
		template <typename Callable>
		Easy_connect(Callable&& c, Easy_signal<R(Args...)>& signal) noexcept;

		Easy_connect(Easy_connect const& rhs) noexcept;
		Easy_connect(Easy_connect&& rhs) noexcept;

	public: // operators
		Easy_connect& operator=(Easy_connect const& rhs) noexcept;
		Easy_connect& operator=(Easy_connect&& rhs) noexcept;

		R operator()(Args... args) const; // const

	public:
		std::string const& get_id() const { return id_; }
		void disconnect();
		void reconnect();

	private:
		void swap(Easy_connect& rhs) noexcept;

	private:
		std::function<R(Args...)> callee_;
		std::string id_;
		std::reference_wrapper<Easy_signal<R(Args...)>> signal_;
		bool connected_{ true };
	};

	template<typename R, typename ...Args>
	template<typename Callable>
	inline Easy_connect<R(Args...)>::Easy_connect(Callable&& c, Easy_signal<R(Args...)>& signal) noexcept
		: callee_(std::forward<Callable>(c))
		, id_(utils::guid_hex(24))
		, signal_(std::ref(signal))
	{
	}

	template<typename R, typename ...Args>
	inline Easy_connect<R(Args...)>::Easy_connect(Easy_connect const& rhs) noexcept
		: callee_(rhs.callee_)
		, id_(rhs.id_)
		, signal_(rhs.signal_)
		, connected_(rhs.connected_)
	{
	}

	template<typename R, typename ...Args>
	inline Easy_connect<R(Args...)>::Easy_connect(Easy_connect&& rhs) noexcept
		: callee_(std::move(rhs.callee_))
		, id_(std::move(rhs.id_))
		, signal_(std::move(rhs.signal_))
		, connected_(rhs.connected_)
	{
	}

	template<typename R, typename ...Args>
	inline Easy_connect<R(Args...)>& Easy_connect<R(Args...)>::operator=(Easy_connect const& rhs) noexcept
	{
		Easy_connect temp(rhs);
		swap(temp);
		return *this;
	}

	template<typename R, typename ...Args>
	inline Easy_connect<R(Args...)>& Easy_connect<R(Args...)>::operator=(Easy_connect&& rhs) noexcept
	{
		swap(rhs);
		return *this;
	}

	template<typename R, typename ...Args>
	inline R Easy_connect<R(Args...)>::operator()(Args ...args) const
	{
		return callee_(args...);
	}

	template<typename R, typename ...Args>
	inline void Easy_connect<R(Args...)>::disconnect()
	{
		if (!connected_) return;
		signal_.get().disconnect(id_);
		connected_ = false;
	}

	template<typename R, typename ...Args>
	inline void Easy_connect<R(Args...)>::reconnect()
	{
		if (connected_) return;
		signal_.get().connect(*this);
		connected_ = true;
	}

	template<typename R, typename ...Args>
	inline void Easy_connect<R(Args...)>::swap(Easy_connect& rhs) noexcept
	{
		std::swap(callee_, rhs.callee_);
		std::swap(id_, rhs.id_);
		std::swap(signal_, rhs.signal_);
		std::swap(connected_, rhs.connected_);
	}

#pragma endregion

#pragma region class Easy_signal

	template <typename Signature>
	class Easy_signal;

	template <typename R, typename... Args>
	class Easy_signal<R(Args...)>
	{
	public: // base
		void swap(Easy_signal& rhs) noexcept
		{
			std::swap(slots_, rhs.slots_);
			std::swap(signals_, rhs.signals_);
		}

	public: // ctor
		Easy_signal() noexcept {}
		~Easy_signal() noexcept {}
		Easy_signal(Easy_signal const& rhs) noexcept;
		Easy_signal(Easy_signal&& rhs) noexcept;

	public: // operators
		Easy_signal& operator=(Easy_signal const& rhs) noexcept;
		Easy_signal& operator=(Easy_signal&& rhs) noexcept;

	public: // connect to this signal
		/**
		* connect callable to signal
		* 1. function
		* 2. functor
		* 3. lambda
		*/
		template <typename Callable>
		Easy_connect<R(Args...)> connect(Callable&& call) noexcept;

		/**
		* connect class member function pointer to this signal
		*/
		template <typename R2, typename Class>
		Easy_connect<R(Args...)> connect(R2 Class::* pmf, Class& obj);

		/**
		* connect signal to signal
		*/
		void connect(Easy_signal& rhs);
		void connect(Easy_signal const& rhs);

		/**
		* connect connect to signal
		*/
		void connect(Easy_connect<R(Args...)> const& conn);
		void connect(Easy_connect<R(Args...)>& conn);

	public: // emit signal
		template <typename... CallArgs>
		void operator()(CallArgs&&... args) const;

		template <typename ...CallArgs>
		void emit_(CallArgs&&... args) const;

		template <typename Callback, typename ...CallArgs>
		auto emit_handle_result(Callback&& cb, CallArgs&&... args) const;

	public:
		void disconnect(std::string id);

	private:
		template <typename R2, typename Class, std::size_t... Is>
		Easy_connect<R(Args...)> connect_pmf_(R2 Class::* pmf, Class& obj, detail::Indices<Is...>);

	private:
		std::vector<Easy_connect<R(Args...)>> slots_;
		std::vector<std::reference_wrapper<Easy_signal>> signals_;
	};

	template<typename R, typename ...Args>
	inline Easy_signal<R(Args...)>::Easy_signal(Easy_signal const& rhs) noexcept
		: slots_(rhs.slots_)
		, signals_(rhs.signals_)
	{
	}

	template<typename R, typename ...Args>
	inline Easy_signal<R(Args...)>::Easy_signal(Easy_signal&& rhs) noexcept
		: slots_(std::move(rhs.slots_))
		, signals_(std::move(rhs.signals_))
	{
	}

	template<typename R, typename ...Args>
	inline Easy_signal<R(Args...)>& Easy_signal<R(Args...)>::operator=(Easy_signal const& rhs) noexcept
	{
		Easy_signal temp(rhs);
		swap(temp);
		return *this;
	}

	template<typename R, typename ...Args>
	inline Easy_signal<R(Args...)>& Easy_signal<R(Args...)>::operator=(Easy_signal&& rhs) noexcept
	{
		swap(rhs);
		return *this;
	}

	template<typename R, typename ...Args>
	inline void Easy_signal<R(Args...)>::connect(Easy_signal& rhs)
	{
		signals_.push_back(std::ref(rhs));
	}

	template<typename R, typename ...Args>
	inline void Easy_signal<R(Args...)>::connect(Easy_signal const& rhs)
	{
		signals_.push_back(std::ref(rhs));
	}

	template<typename R, typename ...Args>
	inline void Easy_signal<R(Args...)>::connect(Easy_connect<R(Args...)> const& conn)
	{
		slots_.push_back(conn);
	}

	template<typename R, typename ...Args>
	inline void Easy_signal<R(Args...)>::connect(Easy_connect<R(Args...)>& conn)
	{
		slots_.push_back(conn);
	}

	template<typename R, typename ...Args>
	inline void Easy_signal<R(Args...)>::disconnect(std::string id)
	{
		stlex::erase_remove(slots_, [id](auto& conn) {
			return conn.get_id() == id;
			});
	}

	template<typename R, typename ...Args>
	template<typename Callable>
	inline Easy_connect<R(Args...)> Easy_signal<R(Args...)>::connect(Callable&& call) noexcept
	{
		using Functor = std::decay_t<Callable>;
		auto conn = Easy_connect<R(Args...)>(std::forward<Functor>(call), *this);
		slots_.push_back(conn);
		return conn;
	}

	template<typename R, typename ...Args>
	template<typename R2, typename Class>
	inline Easy_connect<R(Args...)> Easy_signal<R(Args...)>::connect(R2 Class::* pmf, Class& obj)
	{
		return connect_pmf_(pmf, obj, detail::Build_indices<sizeof...(Args)>());
	}

	template<typename R, typename ...Args>
	template<typename ...CallArgs>
	inline void Easy_signal<R(Args...)>::operator()(CallArgs&& ...args) const
	{
		std::for_each(
			slots_.cbegin(),
			slots_.cend(),
			[&](auto const& f)
			{
				f(std::forward<CallArgs>(args)...);
			}
		);

		std::for_each(
			signals_.cbegin(),
			signals_.cend(),
			[&](auto sig)
			{
				sig.get()(std::forward<CallArgs>(args)...);
			}
		);
	}

	template<typename R, typename ...Args>
	template<typename ...CallArgs>
	inline void Easy_signal<R(Args...)>::emit_(CallArgs&& ...args) const
	{
		operator()(std::forward<CallArgs>(args)...);
	}

	template<typename R, typename ...Args>
	template<typename Callback, typename ...CallArgs>
	inline auto Easy_signal<R(Args...)>::emit_handle_result(Callback&& cb, CallArgs&& ...args) const
	{
		std::vector<R> v;
		std::for_each(
			slots_.cbegin(),
			slots_.cend(),
			[&](auto const& f)
			{
				v.push_back(f(std::forward<CallArgs>(args)...));
			}
		);
		std::for_each(
			signals_.cbegin(),
			signals_.cend(),
			[&](auto sig)
			{
				sig.get()(std::forward<CallArgs>(args)...);
			}
		);

		return std::forward<Callback>(cb)(std::move(v));
	}

	template<typename R, typename ...Args>
	template<typename R2, typename Class, std::size_t ...Is>
	inline Easy_connect<R(Args...)> Easy_signal<R(Args...)>::connect_pmf_(R2 Class::* pmf, Class& obj, detail::Indices<Is...>)
	{
		auto conn = Easy_connect<R(Args...)>(std::bind(pmf, &obj, Place_holder<Is + 1>{}...), *this); // skip member function the first hide arg
		slots_.push_back(conn);
		return conn;
	}

	template <typename R, typename ... Args, typename R2, typename ... Args2>
	void swap(Easy_signal<R(Args...)>& a, Easy_signal<R2(Args2...)>& b) noexcept
	{
		a.swap(b);
	}

#pragma endregion
} // namespace rola

#endif // !ROLA_EASY_SIGNAL_EASY_SIGNAL_HPP
