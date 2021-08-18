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

#pragma region ģ����Զ����

// 1. ��ʽ�ӿڣ�ģ��ֱ�����������;��и������ֵĳ�Ա
template <typename T>
void sample1(T t)
{
	t.foo();
	typename T::value_type x;
}

// 2. ��ʽ�ӿڣ�ͨ��ADL���ҵķǳ�Ա����(��ϣ����λ��ģ������ʵ�������������ڵ����ֿռ���)
template <typename T>
void sample2(T t)
{
	foo(t);
	std::cout << t;
}

// 3. ʹ���ػ���ģ�������ڡ������Ѿ��ػ������ṩ����һ����ģ�塱
template <typename T>
void sample3(T t)
{
	S3Traits<T>::foo(t);
	typename S3Traits<T>::value_type x;
}

#pragma endregion

#pragma region ����ػ�����ģ��

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