// use specialization to provide alternative implementations of class templates

// use tag dispatch to provide alternative implementations of a function

struct pod_tag {};
struct non_pod_tag {};

template <class T>
struct copy_trait
{
	using tag = non_pod_tag;
};

template <>
struct copy_trait<int>
{
	using tag = pod_tag;
};

template <class Iter>
Iter copy_helper(Iter first, Iter last, Iter out, pod)
{
	// use memmove
}

template <class Iter>
Iter copy_helper(Iter first, Iter last, Iter out, non_pod_tag)
{
	// use loop calling copy constructors
}

template <class Iter>
Iter copy(Iter first, Iter last, Iter out)
{
	return copy_helper(first, last, out, typename copy_trait<Iter>::tag{});
}

#pragma region 模板的自定义点

// 1. 隐式接口，模板直接依赖于类型具有给定名字的成员
template <typename T>
void sample1(T t)
{
	t.foo();
	typename T::value_type x;
}

// 2. 隐式接口，通过ADL查找的非成员函数(即希望它位于模板用来实例化的类型所在的名字空间中)
template <typename T>
void sample2(T t)
{
	foo(t);
	std::cout << t;
}

// 3. 使用特化，模板依赖于“类型已经特化了你提供的另一个类模板”
template <typename T>
void sample3(T t)
{
	S3Traits<T>::foo(t);
	typename S3Traits<T>::value_type x;
}

#pragma endregion

#pragma region 如何特化函数模板

template <class T>
struct FImpl;

template <class T>
void f(T t) // don't change it
{
	FImpl<T>::f(t);
}

template <class T>
struct FImpl // can be safe special
{
	static void f(T t);
};

#pragma endregion