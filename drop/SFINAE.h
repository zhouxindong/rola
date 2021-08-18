
#pragma region class member check

#include <type_traits>
#include <string>

#pragma region c98

template <typename T>
struct Has_serialize_98
{
	template <typename U, U u>
	struct Really_has;

	template <typename C>
	static std::true_type test(Really_has<std::string(C::*)(), &C::serialize>*)
	{}

	template <typename C>
	static std::true_type test(Really_has<std::string(C::*)() const, &C::serialize>*)
	{}

	template <typename>
	static std::false_type test(...)
	{}

	static const bool value = std::is_same_v<
		std::true_type,
		decltype(test<T>(nullptr))>;
};

// ��ν�������ڶԲ�ͬ��֧��������ɵĵ���ʧ�ܴ���?

 //template <class T>
 //std::string serialize(const T& obj)
 //{
	//if (Has_serialize<T>::value) 
	//{
	//	return obj.serialize(); // error: no member named 'serialize' in 'A'.
	//}
	//else 
	//{
	//	return to_string(obj);
	//}
 //}

 // 1. �ڷ��������и�������
 template <typename T>
 std::enable_if_t<Has_serialize_98<T>::value, std::string>
	 serialize(T const& obj)
 {
	 return obj.serialize();
 }

 template <typename T>
 std::enable_if_t<!Has_serialize_98<T>::value, std::string>
	 serialize(T const& obj)
 {
	 return to_string(obj);
 }

 // �����ظ�����
 //template <typename T, typename = std::enable_if_t<Has_serialize<T>::value>>
 //std::string serialize(T const& obj)
 //{
	// return obj.serialize();
 //}

 //template <typename T, typename = std::enable_if_t<!Has_serialize<T>::value>>
 //std::string serialize(T const& obj)
 //{
	// return to_string(obj);
 //}

#pragma endregion

#pragma region c11

 template <typename T>
 struct Has_serialize_11_1
 {
	 template <typename C>
	 static constexpr decltype(std::declval<C>().serialize(), bool())
		 test(int)
	 {
		 return true;
	 }

	 template <typename C>
	 static constexpr bool test(...)
	 {
		 return false;
	 }

	 static constexpr bool value = test<T>(int());
 };

 template <typename T, typename = std::string>
 struct Has_serialize_11_2 : std::false_type
 {};

 template <typename T>
 struct Has_serialize_11_2<T, decltype(std::declval<T>().serialize())> : std::true_type
 {};

#pragma endregion

#pragma region lambda generic check

/**
* Closure_savor: ����ģ����������������ĳ��lambda���ʽ�Զ����ɵıհ�����
ģ����������͵�operator()�е�β�÷���ֵ�������������Ŵ�SFINAE����Լ��
*/
 template <typename UnnamedType>
 struct Closure_savor
 {
 private:
	 template <typename ...Args>
	 constexpr auto test_validity(int)
		 -> decltype(std::declval<UnnamedType>()(std::declval<Args>()...), std::true_type())
	 {
		 return std::true_type();
	 }

	 template <typename ...Args>
	 constexpr std::false_type test_validity(...)
	 {
		 return std::false_type();
	 }

 public:
	 template <typename ...Args>
	 constexpr auto operator()(Args&&... args)
	 {
		 return test_validity<Args...>(int());
	 }
 };

 /**
* is_valid: ����һ��lambda����������һ��Closure_savor<>����ģ�����������lambda�ıհ�����
*/
 template <typename UnnamedType>
 constexpr auto is_valid(UnnamedType&& t)
 {
	 return Closure_savor<UnnamedType>();
 }

 static auto Has_serialize_lad = is_valid([](auto&& x)->decltype(x.serialize()) {});
 
 template <typename T>
 auto serialize_g(T&& obj)
	 -> std::enable_if_t<decltype(Has_serialize_lad(std::forward<T>(obj)))::value, std::string>
 {
	 return obj.serialize();
 }

 template <typename T>
 auto serialize_g(T&& obj)
	 -> std::enable_if_t<!decltype(Has_serialize_lad(std::forward<T>(obj)))::value, std::string>
 {
	 return to_string(obj);
 }

#pragma endregion

#pragma region inheriance

 /**
* ʹ��Լ���࣬��ȷĳ��Լ�������д�Լ������ģ����������Ӧ��Լ����
������Ĺ��캯������Լ�����ȱʡ���캯�������Լ�����
*/
 template <typename T>
 class HasClone
 {
 public:
	 static void Constraints()
	 {
		 T* (T::* test)() const = &T::Clone;
		 test;
	 }
	 HasClone()
	 {
		 void(*p)() = Constraints;
	 }
 };

 template <typename T>
 class C3 : HasClone<T>
 {
 };

#pragma endregion

#pragma region T constraints

 /**
* ����decltypeָ��ģ�������Լ��
* ��decltype�Ŀ�ʼ����������Լ��(ת��Ϊvoid��ֹ�������������)
* �����һ������������ʵ�ķ�������
*/
 
 template <typename T>
 auto len(T const& t)
	 -> decltype((void)(t.size()), T::size_type())
 {
	 return t.size();
 }

 /**
 * �������T�Ƿ���ĳ�����Ͷ���
 */
 template <typename, typename = std::void_t<>>
 struct HasSizeType : std::false_type
 {
 };

 template <typename T>
 struct HasSizeType<T, std::void_t<typename T::size_type>> : std::true_type
 {
 };

 /**
  * �������T�Ƿ���ĳ����Ա
  */
 template <typename, typename = std::void_t<>>
 struct HasMember : std::false_type
 {
 };

 template <typename T>
 struct HasMember<T, std::void_t<decltype(&T::Member)>> : std::true_type
 {
 };

 /**
* can used for overload functions
*/
 template <typename, typename = std::void_t<>>
 struct HasBeginT : std::false_type
 {
 };

 template <typename T>
 struct HasBeginT<T, std::void_t<decltype(std::declval<T>().begin())>> : std::true_type
 {
 };

 template <typename, typename, typename = std::void_t<>>
 struct HasLessT : std::false_type
 {
 };

 template <typename T1, typename T2>
 struct HasLessT<T1, T2, std::void_t<
	 decltype(std::declval<T1>() < std::declval<T2>())>> : std::true_type
 {
 };
#pragma endregion

#pragma endregion

#pragma region dispatch

 // 1. class partial specially

 template <typename Iterator, typename Tag>
 class Advance;

 template <typename Iterator>
 class Advance<Iterator, std::input_iterator_tag>
 {
 public:
	 static void foo()
	 {
		 std::cout << "std::input_iterator_tag\n";
	 }
 };

 template <typename Iterator>
 class Advance<Iterator, std::bidirectional_iterator_tag>
 {
 public:
	 static void foo()
	 {
		 std::cout << "std::bidirectional_iterator_tag\n";
	 }
 };

 template <typename Iterator>
 class Advance<Iterator, std::random_access_iterator_tag>
 {
 public:
	 static void foo()
	 {
		 std::cout << "std::random_access_iterator_tag\n";
	 }
 };

 template <typename Iterator>
 void foo_call(Iterator it)
 {
	 return Advance<Iterator, typename std::iterator_traits<Iterator>::iterator_category>::foo();
 }

 // 2. function tag dispatch
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
 void copy_helper(Iter first, Iter last, Iter out, pod_tag)
 {
	 // use memmove
 }

 template <class Iter>
 void copy_helper(Iter first, Iter last, Iter out, non_pod_tag)
 {
	 // use loop calling copy ctor
 }

 template <class Iter>
 void copy(Iter first, Iter last, Iter out)
 {
	 return copy_helper(first, last, out, typename copy_trait<Iter>::tag{});
 }

#pragma endregion

#pragma region SFINAE

/**
* 1. SFINAE��������
*/
 template <typename T>
 struct IsDefaultConstructible {
 private:
	 template <typename U, typename = decltype(U())> // must templatilze, so can be deduced when called
	 static std::true_type test(void*);

	 template <typename>
	 static std::false_type test(...);

 public:
	 using Type = decltype(test<T>(nullptr));
 };

 template <typename T>
 class HasDereference
 {
 private:
	 template <typename U>
	 struct Identity;

	 template <typename U>
	 static std::true_type test(Identity<decltype(*std::declval<U>())>*);

	 template <typename U>
	 static std::false_type test(...);

 public:
	 static constexpr bool value = decltype(test<T>(nullptr))::value;
 };

/**
* 2. SFINAE�ֲ��ػ�
*/
// ����������������ģ�����
 template <typename...>
 using VoidT = void; // just same as std::void_t definition

 // primary template
 template <typename, typename = VoidT<>>
 struct IsDefaultConstructible3 : std::false_type
 {
 };

 // partial specialization(may be SFINAE'd away)
 // VoidT<>��������������Ŀ�ı�����Լ����֤
 template <typename T>
 struct IsDefaultConstructible3<T, VoidT<decltype(T())>> : std::true_type
 {
 };

 /**
* 4. SFINAE traits
*/
// primary template
 template <typename, typename, typename = std::void_t<>>
 struct HasPlusT : std::false_type
 {
 };

 // partial specialization(may be SFINAE'd away)
 template <typename T1, typename T2>
 struct HasPlusT<T1, T2, std::void_t<decltype(std::declval<T1>() + std::declval<T2>())>>
	 : std::true_type
 {
 };

 template <typename T1, typename T2, bool = HasPlusT<T1, T2>::value>
 struct PlusResultT {
	 using Type = decltype(std::declval<T1>() + std::declval<T2>());
 };

 template <typename T1, typename T2>
 struct PlusResultT<T1, T2, false> {
 };
#pragma endregion