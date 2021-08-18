
constexpr int on_stack_max = 20;

template <typename T>
struct Scoped	// store a T in Scoped
{
	T obj;
};

template <typename T>
struct On_heap	// store a T on the free store
{
	T* objp;
};

#include <type_traits>
#include <array>

template <typename T>
using Handle = typename std::conditional<(sizeof(T) <= on_stack_max),
	Scoped<T>,
	On_heap<T>>::type;

void f()
{
	Handle<double> v1;
	Handle<std::array<double, 200>> v2;
}