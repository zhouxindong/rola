
#pragma region callable basic

int ask()
{
	return 42;
}

typedef decltype(ask)* function_ptr;

class convertible_to_function_ptr
{
public:
	operator function_ptr() const
	{
		return ask;
	}
};

auto ask_ptr = &ask; // function pointer, ask_ptr()
auto& ask_ref = ask; // function ref, ask_ref()
convertible_to_function_ptr ask_wrapper; // functor, ask_wrapper()

class older_than
{
	int limit_;

public:
	older_than(int limit)
		: limit_(limit)
	{}

	template <typename T>
	bool operator()(T&& object) const
	{
		return std::forward<T>(object).age() > limit_;
	}
};

auto predicate = [limit = 42](auto&& object){
	return std::forward<decltype(object)>(object).age() > limit;
};

#pragma endregion

#pragma region callable generic

/**
 * 两种不同形式的调用类型的偏特化及其中的区别
 */

 /**
  * def_signature模板: 用于调用形式解析
  类成员指针类型只能为Type T::*
  不能这样声明Ret(T::*f)(Args...)
  */
#include <utility>

template <typename T>
struct def_signature;

template <typename R,typename ...Args>
struct def_signature<R(Args...)>
{
	template <typename Callable>
	static R call(Callable&& c, Args... args)
	{
		return std::forward<Callable>(c)(args...);
	}
};

/**
 * call_signature模板: 用于调用对象解析
 注意: 成员函数指针不是可调用对象，无法处理
 */
template <typename T>
struct call_signature;

template <typename Callable, typename ...Args>
struct call_signature<Callable(Args...)>
{
	using result_type = decltype(std::declval<Callable>()(std::declval<Args>()...));

	static result_type call(Callable c, Args... args)
	{
		return result_type c(args...);
	}
};

#pragma endregion

#pragma region invoke

/**
*
INVOKE(std::forward<F>(f), std::forward<Args>(args)...)
INVOKE(f, t1, t2, ..., tN) -- #1
1. f是T的成员函数指针
1.1 std::is_base_of<T, std::decay_t<decltype(t1)>>::value == true
#1 == (t1.*f)(t2, ..., tN)
1.2 std::decay_t<decltype(t1)>是std::reference_wrapper的一个特化
#1 == (t1.get().*f)(t2, ..., tN)
1.3 #1 == ((*t1).*f)(t2, ..., tN)

2. N == 1 and f是T的成员函数指针(INVOKE(f, t1))
2.1 std::is_base_of<T, std::decay_t<decltype(t1)>>::value == true
#1 == t1.*f
2.2 std::decay_t<decltype(t1)>是std::reference_wrapper的一个特化
#1 == t.get().*f
2.3 #1 == (*t1).*f

3. 非1和2情形
#1 == f(t1, t2, ..., tN)
*/
#include <utility>
#include <type_traits>

namespace detail
{
	template <typename>
	constexpr bool is_reference_wrapper_v = false;

	template <typename U>
	constexpr bool is_reference_wrapper_v<std::reference_wrapper<U>> = true;

	// situation for class member pointer
	template <class T, class Type, class T1, class... Args>
	constexpr decltype(auto) INVOKE(Type T::* f, T1&& t1, Args&&... args)
	{
		if constexpr (std::is_member_function_pointer_v<decltype(f)>) { // member func pointer
			if constexpr (std::is_base_of_v<T, std::decay_t<T1>>) // a object
				return (std::forward<T1>(t1).*f)(std::forward<Args>(args)...);
			else if constexpr (is_reference_wrapper_v<std::decay_t<T1>>) // // t1 is a ref of T
				return (t1.get().*f)(std::forward<Args>(args)...); // std::forward<T1>(t1).get().*f ok
			else // a pointer
				return ((*std::forward<T1>(t1)).*f)(std::forward<Args>(args)...);
		}
		else { // member obj pointer
			static_assert(std::is_member_object_pointer_v<decltype(f)>);
			static_assert(sizeof...(args) == 0);
			if constexpr (std::is_base_of_v<T, std::decay_t<T1>>)
				return std::forward<T1>(t1).*f;
			else if constexpr (is_reference_wrapper_v<std::decay_t<T1>>)
				return t1.get().*f;
			else
				return (*std::forward<T1>(t1)).*f;
		}
	}

	// other callable 
	template <class F, class... Args>
	constexpr decltype(auto) INVOKE(F&& f, Args&&... args)
	{
		return std::forward<F>(f)(std::forward<Args>(args)...);
	}
} // namespacedetail

template <typename F, typename ...Args>
constexpr decltype(auto) invoke(F&& f, Args&&... args)
{
	return detail::INVOKE(std::forward<F>(f), std::forward<Args>(args)...);
}

#pragma endregion

#pragma region result of callable

namespace detail
{
	// basic for generic callable, get the type of result
	template <typename T>
	struct invoke_impl
	{
		template <typename F, typename ...Args>
		static auto call(F&& f, Args&&... args)
			-> decltype(std::forward<F>(f)(std::forward<Args>(args)...));
	};

	// partial special for member pointer(data or function), get the type of result
	template <typename B, typename MT>
	struct invoke_impl<MT B::*>
	{
		/**
		* get()分三种不同情况，分别获取成员指针作为可调用对象被调用时的绑定对象
		*/

		// 1. t1是成员指针所绑定类的类型 -> (t1.*f)(t2, ..., tN)
		template <typename T, typename Td = typename std::decay_t<T>,
			typename = typename std::enable_if_t<std::is_base_of_v<B, Td>>>
			static auto get(T&& t)->T&&;

		// 2. t1是reference_wrapper包装类，返回原始类型 -> (t1.get().*f)(t2, ..., tN)
		template <typename T, typename Td = typename std::decay_t<T>,
			typename = typename std::enable_if_t<is_reference_wrapper_v<Td>>>
			static auto get(T&& t)->decltype(t.get());

		// 3. 否则即为指针，其指向对象为成员指针所绑定的对象 -> ((*t1).*f)(t2, ..., tN)
		template <typename T, typename Td = typename std::decay_t<T>,
			typename = typename std::enable_if_t<!std::is_base_of_v<B, Td>>,
			typename = typename std::enable_if_t<!is_reference_wrapper_v<Td>>>
			static auto get(T&& t)->decltype(*std::forward<T>(t));

		// 用于成员函数指针
		template <typename T, typename ...Args, typename MT1,
			typename = typename std::enable_if_t<std::is_function_v<MT1>>>
			static auto call(MT1 B::* pmf, T&& t, Args&&... args)
			-> decltype((invoke_impl::get(std::forward<T>(t)).*pmf)(std::forward<Args>(args)...));

		// 用于成员数据
		template <typename T>
		static auto call(MT B::* pmd, T&& t)
			-> decltype(invoke_impl::get(std::forward<T>(t)).*pmd);
	};

	template <typename F, typename ...Args,
		typename Fd = typename std::decay_t<F>>
		auto RESULT_OF(F&& f, Args&&... args)
		-> decltype(invoke_impl<Fd>::call(std::forward<F>(f), std::forward<Args>(args)...));
} // namespace detail

namespace c11
{
	template <typename>
	struct result_of;

	template <typename F, typename ...Args>
	struct result_of<F(Args...)>
	{
		using type =
			decltype(detail::RESULT_OF(std::declval<F>(), std::declval<Args>()...));
	};
} // namespace c11

namespace c14
{
	template <typename AlwaysVoid, typename, typename...>
	struct result_of;

	template <typename F, typename ...Args>
	struct result_of<
		decltype(void(detail::RESULT_OF(std::declval<F>(), std::declval<Args>()...))),
		F, Args...>
	{
		using type = decltype(detail::RESULT_OF(std::declval<F>(), std::declval<Args>()...));
	};
} // namespace c14

namespace c17
{
	template <typename>
	struct result_of;

	template <typename F, typename ...Args>
	struct result_of<F(Args...)> : c14::result_of<void, F, Args...> {};

	template <typename F, typename ...Args>
	struct invoke_result : c14::result_of<void, F, Args...> {};
} // namespace c17
#pragma endregion

#pragma region invoke2

// 1. 可调用对象(函数，lambda, functor)
struct InvokeCallable
{
	template <typename Callable, typename ...Args>
	static decltype(auto) call(Callable&& obj, Args&&... args)
	{
		return std::forward<Callable>(obj)(std::forward<Args>(args)...);
	}
};

// 2. 调用成员函数(SFINAE)
struct InvokeMemFunc
{
	// pointer
	template <typename Type, typename B, typename T, typename ...Args>
	static auto call(Type B::* f, T&& obj, Args&&... args)
		-> decltype(
			std::declval<std::enable_if_t<std::is_pointer_v<std::decay_t<T>>>>(),
			(obj->*f)(std::forward<Args>(args)...))
	{
		return (obj->*f)(std::forward<Args>(args)...);
	}

	// object
	template <typename Type, typename B, typename T, typename ...Args>
	static auto call(Type B::* f, T&& obj, Args&&... args)
		-> decltype(
			std::declval<std::enable_if_t<std::is_base_of_v<B, std::decay_t<T>>>>(),
			(obj.*f)(std::forward<Args>(args)...))
	{
		return (obj.*f)(std::forward<Args>(args)...);
	}
};

// 3. 调用成员变量 (SFINAE)
struct InvokeMemData
{
	template <typename Type, typename B, typename T>
	static auto call(Type B::* p, T&& obj)
		-> decltype(
			std::declval<std::enable_if_t<std::is_pointer_v<std::decay_t<T>>>>(),
			(obj->*p))
	{
		return (obj->*p);
	}

	template <typename Type, typename B, typename T>
	static auto call(Type B::* p, T&& obj)
		-> decltype(
			std::declval<std::enable_if_t<std::is_base_of_v<B, std::decay_t<T>>>>(),
			(obj.*p))
	{
		return (obj.*p);
	}
};

/**
 * 利用类模板的偏物化完成类型定义转发(通过继承)
 */
template <
	typename Callable,
	bool IsMemFun = std::is_member_function_pointer_v<std::decay_t<Callable>>,
	bool IsMemData = std::is_member_object_pointer_v<std::decay_t<Callable>>>
	struct INVOKE2;

template <typename Callable>
struct INVOKE2<Callable, true, false> : InvokeMemFunc {};

template <typename Callable>
struct INVOKE2<Callable, false, true> : InvokeMemData {};

template <typename Callable>
struct INVOKE2<Callable, false, false> : InvokeCallable {};

template <typename Callable, typename ...Args>
decltype(auto) invoke2(Callable&& f, Args&&... args)
{
	return INVOKE2<Callable>::call(std::forward<Callable>(f),
		std::forward<Args>(args)...);
}
#pragma endregion

#pragma region class member function

template <typename T>
struct IsPointerToMemberT : std::false_type {
};

template <typename T, typename C>
struct IsPointerToMemberT<T C::*> : std::true_type {
	using MemberT = T;
	using ClassT = C;
};

// 在将类成员函数指针作为可调用对象，将类实例作为第一个隐藏参数传递时，传递指针还是传递对象，其调用对象是不一样的：
// std::async(&X::foo, &x, 42, "hello"); 调用p->foo(42, "hello"), 其中p是&x
// std::async(&X::foo, x, 42, "hello"); 调用tmpx.foo(42, "hello"), 其中tmpx是x的副本

#pragma endregion

#pragma region type_traits

struct C
{
	bool operator()(int) const
	{
		return true;
	}
};
// std::is_invocable<T, Args...>::value
void foo()
{
	std::is_invocable<C>::value; // false
	std::is_invocable<C, int>::value; // true
	std::is_invocable<int*>::value; // false
	std::is_invocable<int(*)()>::value; //true
}

// std::is_invocable_r<RET_T, T, Args...>::value
void foo2()
{
	std::is_invocable_r<bool, C, int>::value; // true
	std::is_invocable_r<int, C, long>::value; // true
	std::is_invocable_r<void, C, int>::value; // true
	std::is_invocable_r<char*, C, int>::value; // false
	std::is_invocable_r<long, int(*)(int)>::value; // false
	std::is_invocable_r<long, int(*)(int), int>::value; // true
	std::is_invocable_r<long, int(*)(int), double>::value; // true
}

#include <string>
// std::invoke_result<T, Args...>::value
// std::result_of<T, Args...>::value
std::string bar(int) { return std::string(""); }
using R0 = typename std::result_of<decltype(&bar)(int)>::type; // C++11
using R1 = typename std::result_of_t<decltype(&bar)(int)>; // C++14
using R2 = typename std::invoke_result_t<decltype(bar), int>; // C++17

#pragma endregion

#pragma region apply

namespace details
{
	template <class F, class T, std::size_t ... I>
	auto apply(F&& f, T&& t, std::index_sequence<I...>)
	{
		return std::invoke(std::forward<F>(f),
			std::get<I>(std::forward<T>(t))...);
	}
}

template <class F, class T>
auto apply(F&& f, T&& t)
{
	return details::apply(std::forward<F>(f), std::forward<T>(t),
		std::make_index_sequence<std::tuple_size<std::decay_t<T>>::value>{});
}

#pragma endregion