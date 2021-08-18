
#pragma region basic

#include <utility>

// 继承类中的拷贝和赋值
class Base {};

class D : public Base {
public:
	D(const D& d) : Base(d) {}
	D(D&& d) : Base(std::move(d)) {}

	D& operator = (const D& rhs) {
		Base::operator=(rhs);
		//static_cast<Base&>(*this) = rhs;
		return *this;
	}

	D& operator = (D&& rhs) {
		Base::operator=(std::move(rhs));
		return *this;
	}
};

// 构造函数委托
class Info {
public:
	Info() : Info(1, 'a') {}
	Info(int i) : Info(i, 'a') {}
	Info(char e) : Info(1, e) {}

private:
	Info(int i, char e) :type_(i), name_(e) {}
	int type_;
	char name_;
};

// 透传至基类的定制构造函数
template <typename Base>
struct B : Base {
	using Base::Base; // 默认、拷贝和移动构造函数不会被继承
};

#pragma endregion

#pragma region class member pointer

/**
* 成员指针指向类的非静态成员
成员指针类型: 类的类型 + 成员的类型 const std::string Screen::*pdata;
(const限定的是pdata，可以绑定的对象的常量性，而不是数据成员本身的类型)
初始化: 令其指向类的某个成员 pdata = &Screen::contents;
使用: 提供成员所属的对象 obj.*pdata, pt->*pdata
*/

/**
* 成员函数指针
char (Screen::*pmf2)(Screen::pos, Screen::pos) const;
pmf2 = &Screen::get
和普通函数指针不同，在成员函数和指向该成员的指针之间不存在自动转换规则
(pt->*pmf2)();
(obj.*pmf2)();
*/

/**
* 成员函数指针不是可调用对象，因为调用前必须通过.*或->*绑定到特定的对象上。
如何将成员函数指针转换为一个可调用对象?
1. std::function

std::function<bool (const string&)> fcn = &string::empty;
find_if(strvec.begin(), strvec.end(), fcn);

编译器如何处理成员函数的this指针呢?
注意function模板参数的声明，其可调用对象的参数其实就是this的类型，也就是迭代器每次调用传给fcn的第一个参数类型
上述代码的答案是迭代器将成为成员函数指针每次调用时的绑定对象
((*it).*p)()

vector<string*> pvec;
function<bool (const string*)> fp = &string::empty;
find_if(pvec.begin(), pvec.end(), fp);

2. mem_fn 从成员指针生成一个可调用对象
find_if(strvec.begin(), strvec.end(), mem_fn(&string::empty));

auto f = mem_fn(&string::empty);
f(*strvec.begin());
f(&strvec[0]);

3. bind
find_if(strvec.begin(), strvec.end(), bind(&string::empty, _1);
*/

#include <functional>
class C
{
public:
	void memfunc(int x, int y) const {}
};

void foo2()
{
	std::function<void(const C&, int, int)> mf;
	mf = &C::memfunc;
	mf(C(), 42, 77);
}

#pragma endregion

#pragma region operators

/**
* 1. operator*()
你可以令operator*完成任何操作，但通常返回某个引用

2. operator->()
你不能改变其内置语义，你只能改变->从哪个对象当中获取成员，所以必须返回指针或重载了->的类对象
对于形如point->mem的表达式来说，point必须是指向类对象的指针或重载了->的类的对象
前者(*point).mem，后者point.operator()->mem
*/

/**
*
1. +运算符实现为非成员函数，+=实现为成员函数，前者调用后者，后者返回自身，前者返回结果临时对象(const)
2. 对于返回临时对象的函数(成员或非成员函数)，返回类型为const
3. 对于返回引用的成员函数，定义const和非const两个版本
4. << >> 运算符不一定是类的友元，可以定义一个成员函数(可以虚拟)，由非成员运算符调用
5. ++前置和后置版本，后置调用前置实现。
*/

/*

*/
using FuncType = void(int);
class Foo
{
public:
	// void(Foo const&, int)
	void operator()(int i) const // 使用时经常是传递一个类型的临时对象，而临时对象是const，如果operator()不声明为const, 则调用失败!
	{

	}
	// void(FuncType*, int)
	operator FuncType*() const
	{}
};
#pragma endregion

#pragma region template ctor and operator=

// 模板构造函数或模板赋值运算符并不会取代预定义的构造函数或赋值运算符
// 也就是说，如果类没有定义构造函数或赋值运算符，编译器会背后为其生成相应的定义
// 然后，编译器生成的缺省定义会同类提供的模板成员函数一同参与调用重载匹配
// 由于缺省的拷贝构造和赋值运算符的调用参数类型为const X&，会很容易造成模板成员函数的匹配优于缺省的定义
// 而模板成员函数的定义并不适用于对象拷贝的情形，造成调用失败
// 通常使用std::enable_if来屏蔽

#pragma region Person

#include <utility>
#include <string>
#include <iostream>

class Person
{
private:
	std::string name;

public:
	explicit Person(std::string const& n) : name(n) {	// #1
		std::cout << "copying string-CONSTR for '" << name << "'\n";
	}

	explicit Person(std::string&& n) : name(std::move(n)) {	// #2
		std::cout << "moving string-CONSTR for '" << name << "'\n";
	}

	Person(Person const& p) : name(p.name) {			// #3
		std::cout << "COPY-CONSTR Person '" << name << "'\n";
	}

	Person(Person&& p) : name(std::move(p.name)) {		// #4
		std::cout << "MOVE-CONSTR Person '" << name << "'\n";
	}
};

void main()
{
	std::string s = "sname";
	Person p1(s);		// #1
	Person p2("tmp");	// #2
	Person p3(p1);		// #3
	Person p4(std::move(p1));	// #4
}

#pragma endregion

#pragma region Person2

class Person2
{
private:
	std::string name;
public:
	// generic constructor for passed initial name:
	template <typename STR>
	explicit Person2(STR&& n) : name(std::forward<STR>(n)) {	// #1
		std::cout << "TMPL-CONSTR for '" << name << "'\n";
	}

	// copy and move constructor
	Person2(Person2 const& p) : name(p.name) {					// #2
		std::cout << "COPY-CONSTR Person '" << name << "'\n";
	}

	Person2(Person2&& p) : name(std::move(p.name)) {			// #3
		std::cout << "MOVE-CONSTR Person '" << name << "'\n";
	}
};

void main()
{
	std::string s = "sname";
	Person2 p1(s);		// #1
	Person2 p2("tmp");	// #1 -> char const[4]
  //Person2 p3(p1);		// ERROR! because non-const lvalue, #1 is better than #2(#2 need convert to const)
						// (1# don't need converted) STR with Person2&
						// STR类型推演为Person2&(唯一类型推演为引用的情形)
	Person2 p4(std::move(p1));	// #3
	Person2 const p2c("ctmp");	// #1
	Person2 p3c(p2c);			// #2
}

#pragma endregion

#pragma region Person3

// 1# 匹配优于 2#，因为2#需要转换为const，增加非 const版本并非良策
// 我们真正需要的是当传递的参数是Person或能够转换为Person的表达式时，禁用模板成员函数(enable_if<>)
class Person3
{
private:
	std::string name;
public:
	// generic constructor for passed initial name:
	template <typename STR,
		typename = std::enable_if_t<
		std::is_convertible_v<STR, std::string>>>
		explicit Person3(STR&& n) : name(std::forward<STR>(n)) {	// #1
		std::cout << "TMPL-CONSTR for '" << name << "'\n";
	}

	// copy and move constructor
	Person3(Person3 const& p) : name(p.name) {					// #2
		std::cout << "COPY-CONSTR Person '" << name << "'\n";
	}

	Person3(Person3&& p) : name(std::move(p.name)) {			// #3
		std::cout << "MOVE-CONSTR Person '" << name << "'\n";
	}
};

void main()
{
	std::string s = "sname";
	Person3 p1(s);				// 1#
	Person3 p2("tmp");			// 1#

	Person3 p3(p1);				// 2#
	Person3 p4(std::move(p1));	// 3#
}

#pragma endregion

#pragma region template function VS. copy ctor

/**
* 由于模板构造函数不能成为拷贝构造函数，因此即使有单模板参数，且形如const T&的函数定义，
在类中仍然会隐式声明拷贝构造函数。模板构造函数将与其他构造函数(包括拷贝构造函数)一起参与到重载解析中
如果模板构造函数比其他的构造函数更匹配，那么将会调用它来对对象进行赋值。
*/

class C
{
public:
	C() = default;
	C(C const&) {}	// #1
	C(C&&) {}		// #2
	template <typename T>
	C(T&&) {}		// #3
};

void main()
{
	C x;
	C x2{ x };				// #3
	C x3{ std::move(x) };	// #2
	C const c;
	C x4{ c };				// #1
	C x5{ std::move(c) };	// #3
}

#pragma endregion

#pragma endregion

#pragma region virtual

/**
* 1. 尽量让接口成为非虚的
一个公用虚函数承担了两个责任:
1. 客户使用的接口
2. 派生类可定制的实现细节
(分离责任)

非虚接口NVI模式
*/
class Gadget;

class Widget {
public: // 稳定的、非虚接口
	int Process(Gadget&);
	bool IsDone();

private: // 定制实现细节(有些类似模板模式)
	virtual int DoProcessPhase1(Gadget&);
	virtual int DoProcessPhase2(Gadget&);
	virtual bool DoIsDone();
};

/**
* 2. 尽量将虚函数置为私有的(不影响重写，又不会暴露给派生类)
* 3. 只有当派生类需要调用基类中实现的虚函数的时候，才需要置为保护的
*/

#pragma region use a factory function if you need "virtual behavior" during initialization
class B
{
protected:
	class Token {};

public:
	explicit B(Token) {} // create an imperfectly initialized object
	virtual void f() = 0;

	template <class T>
	static std::shared_ptr<T> create() // interface for creating shared objects
	{
		auto p = std::make_shared<T>(typename T::Token{});
		p->post_inittialize();
		return p;
	}

protected:
	virtual void post_inittialize() // called right after construction
	{
		f(); // virtual dispatch is safe
	}
};

class D : public B
{
protected:
	class Token {};

public:
	explicit D(Token) : B{ B::Token{} }
	{}

	void f() override {}

protected:
	template <class T>
	friend std::shared_ptr<T> B::create();
};

int main(void) {
	std::shared_ptr<D> p = D::create<D>();
}

#pragma endregion

#pragma region a polymorphic class should suppress copying

class B
{
public:
	B(const B&) = delete;
	B& operator=(const B&) = delete;
	virtual char m() { return 'B'; }
};

class D : public B
{
public:
	char m() override
	{
		return 'D';
	}
};

#pragma endregion

#pragma region other default operation rules

// besides the rules of 6, comparisons, swap, hash
class Bar 
{
public:
	void swap(Bar& rhs) noexcept
	{
	}
};

class Foo
{
public:
	void swap(Foo& rhs) noexcept
	{
		m1.swap(rhs.m1);
		std::swap(m2, rhs.m2);
	}

private:
	Bar m1;
	int m2;
};

void swap(Foo& a, Foo& b)
{
	a.swap(b);
}

void f3(Foo& a, Foo& b)
{
	using std::swap; 
	swap(a, b); // call specific if exists, or general function
}

struct X
{
	std::string name;
	int number;
};

bool operator==(const X& a, const X& b) noexcept
{
	return a.name == b.name && a.number == b.number;
}

#pragma endregion

#pragma region a deep copies of polymorhpic classes

class B
{
public:
	virtual owner<B*> clone() = 0;
	virtual ~B = default;

	B(const B&) = delete;
	B& operator=(const B&) = delete;
};

class D : public B
{
public:
	owner<D*> clone() override; // the covariant return type cannot be a smart pointer
	~D() override;
};

#pragma endregion

#pragma endregion