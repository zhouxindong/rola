/**
 * ±‡“Î∆⁄¬∑æ∂—°‘Ò
 1. if constexpr
 2. template class partial special
 3. overload function dispatc
 */
 // 1. if constexpr
template <typename T>
bool f(T p)
{
	if constexpr (sizeof(T) <= sizeof(long long))
	{
		return p > 0;
	}
	else
	{
		return p.compare(0) > 0;
	}
}

// 2. template class partial special
template <bool b>
struct Dispatch
{
	template <typename T>
	static bool f(T p)
	{
		return p.compare(0) > 0;
	}
};

template <>
struct Dispatch<true>
{
	template <typename T>
	static bool f(T p)
	{
		return p > 0;
	}
};

template <typename T>
bool f(T p)
{
	return Dispatch<sizeof(T) <= sizeof(long long)>::f(p);
}

// 3. function overload dispatch
template <typename T>
bool dispatch_impl(T p, std::true_type)
{
	return p > 0;
}

template <typename T>
bool dispatch_impl(T p, std::false_type)
{
	return p.compare(0) > 0;
}

template <typename T>
bool dispatch(T p)
{
	return dispatch_impl(p, std::bool_constant<sizeof(T) <= sizeof(long long)>());
}
