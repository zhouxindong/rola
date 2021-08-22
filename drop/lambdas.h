
// inheriting from a lambda (C++11)
template <typename TCall, typename UCall>
class SimpleOverloaded : public TCall, UCall
{
public:
	SimpleOverloaded(TCall tf, UCall uf)
		: TCall(tf), UCall(uf)
	{}

	using TCall::operator();
	using UCall::operator();
};

template <typename TCall, typename UCall>
SimpleOverloaded<TCall, UCall> MakeOverloaded(TCall&& tf, UCall&& uf)
{
	return SimpleOverloaded<TCall, UCall>(tf, uf);
}

// inheriting from a lambda (C++17)
template <class ... Ts> struct overloaded17 : Ts... { using Ts::operator()...; };
template <class ... Ts> overloaded17(Ts...)->overloaded17<Ts...>;



#define LIFT(foo)		\
	[](auto&& ... x)	\
		noexcept(noexcept(foo(std::forward<decltype(x)>(x)...)))	\
		-> decltype(foo(std::forward<decltype(x)>(x)...))	\
	{ return foo(std::forward<decltype(x)>(x)...); }