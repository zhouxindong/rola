
#pragma region basic

#include <utility>

// �̳����еĿ����͸�ֵ
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

// ���캯��ί��
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

// ͸��������Ķ��ƹ��캯��
template <typename Base>
struct B : Base {
	using Base::Base; // Ĭ�ϡ��������ƶ����캯�����ᱻ�̳�
};

#pragma endregion

#pragma region class member pointer

/**
* ��Աָ��ָ����ķǾ�̬��Ա
��Աָ������: ������� + ��Ա������ const std::string Screen::*pdata;
(const�޶�����pdata�����԰󶨵Ķ���ĳ����ԣ����������ݳ�Ա���������)
��ʼ��: ����ָ�����ĳ����Ա pdata = &Screen::contents;
ʹ��: �ṩ��Ա�����Ķ��� obj.*pdata, pt->*pdata
*/

/**
* ��Ա����ָ��
char (Screen::*pmf2)(Screen::pos, Screen::pos) const;
pmf2 = &Screen::get
����ͨ����ָ�벻ͬ���ڳ�Ա������ָ��ó�Ա��ָ��֮�䲻�����Զ�ת������
(pt->*pmf2)();
(obj.*pmf2)();
*/

/**
* ��Ա����ָ�벻�ǿɵ��ö�����Ϊ����ǰ����ͨ��.*��->*�󶨵��ض��Ķ����ϡ�
��ν���Ա����ָ��ת��Ϊһ���ɵ��ö���?
1. std::function

std::function<bool (const string&)> fcn = &string::empty;
find_if(strvec.begin(), strvec.end(), fcn);

��������δ����Ա������thisָ����?
ע��functionģ���������������ɵ��ö���Ĳ�����ʵ����this�����ͣ�Ҳ���ǵ�����ÿ�ε��ô���fcn�ĵ�һ����������
��������Ĵ��ǵ���������Ϊ��Ա����ָ��ÿ�ε���ʱ�İ󶨶���
((*it).*p)()

vector<string*> pvec;
function<bool (const string*)> fp = &string::empty;
find_if(pvec.begin(), pvec.end(), fp);

2. mem_fn �ӳ�Աָ������һ���ɵ��ö���
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
�������operator*����κβ�������ͨ������ĳ������

2. operator->()
�㲻�ܸı����������壬��ֻ�ܸı�->���ĸ������л�ȡ��Ա�����Ա��뷵��ָ���������->�������
��������point->mem�ı��ʽ��˵��point������ָ��������ָ���������->����Ķ���
ǰ��(*point).mem������point.operator()->mem
*/

/**
*
1. +�����ʵ��Ϊ�ǳ�Ա������+=ʵ��Ϊ��Ա������ǰ�ߵ��ú��ߣ����߷�������ǰ�߷��ؽ����ʱ����(const)
2. ���ڷ�����ʱ����ĺ���(��Ա��ǳ�Ա����)����������Ϊconst
3. ���ڷ������õĳ�Ա����������const�ͷ�const�����汾
4. << >> �������һ���������Ԫ�����Զ���һ����Ա����(��������)���ɷǳ�Ա���������
5. ++ǰ�úͺ��ð汾�����õ���ǰ��ʵ�֡�
*/

/*

*/
using FuncType = void(int);
class Foo
{
public:
	// void(Foo const&, int)
	void operator()(int i) const // ʹ��ʱ�����Ǵ���һ�����͵���ʱ���󣬶���ʱ������const�����operator()������Ϊconst, �����ʧ��!
	{

	}
	// void(FuncType*, int)
	operator FuncType*() const
	{}
};
#pragma endregion

#pragma region template ctor and operator=

// ģ�幹�캯����ģ�帳ֵ�����������ȡ��Ԥ����Ĺ��캯����ֵ�����
// Ҳ����˵�������û�ж��幹�캯����ֵ��������������ᱳ��Ϊ��������Ӧ�Ķ���
// Ȼ�󣬱��������ɵ�ȱʡ�����ͬ���ṩ��ģ���Ա����һͬ�����������ƥ��
// ����ȱʡ�Ŀ�������͸�ֵ������ĵ��ò�������Ϊconst X&������������ģ���Ա������ƥ������ȱʡ�Ķ���
// ��ģ���Ա�����Ķ��岢�������ڶ��󿽱������Σ���ɵ���ʧ��
// ͨ��ʹ��std::enable_if������

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
						// STR��������ΪPerson2&(Ψһ��������Ϊ���õ�����)
	Person2 p4(std::move(p1));	// #3
	Person2 const p2c("ctmp");	// #1
	Person2 p3c(p2c);			// #2
}

#pragma endregion

#pragma region Person3

// 1# ƥ������ 2#����Ϊ2#��Ҫת��Ϊconst�����ӷ� const�汾��������
// ����������Ҫ���ǵ����ݵĲ�����Person���ܹ�ת��ΪPerson�ı��ʽʱ������ģ���Ա����(enable_if<>)
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
* ����ģ�幹�캯�����ܳ�Ϊ�������캯������˼�ʹ�е�ģ�������������const T&�ĺ������壬
��������Ȼ����ʽ�����������캯����ģ�幹�캯�������������캯��(�����������캯��)һ����뵽���ؽ�����
���ģ�幹�캯���������Ĺ��캯����ƥ�䣬��ô������������Զ�����и�ֵ��
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
* 1. �����ýӿڳ�Ϊ�����
һ�������麯���е�����������:
1. �ͻ�ʹ�õĽӿ�
2. ������ɶ��Ƶ�ʵ��ϸ��
(��������)

����ӿ�NVIģʽ
*/
class Gadget;

class Widget {
public: // �ȶ��ġ�����ӿ�
	int Process(Gadget&);
	bool IsDone();

private: // ����ʵ��ϸ��(��Щ����ģ��ģʽ)
	virtual int DoProcessPhase1(Gadget&);
	virtual int DoProcessPhase2(Gadget&);
	virtual bool DoIsDone();
};

/**
* 2. �������麯����Ϊ˽�е�(��Ӱ����д���ֲ��ᱩ¶��������)
* 3. ֻ�е���������Ҫ���û�����ʵ�ֵ��麯����ʱ�򣬲���Ҫ��Ϊ������
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