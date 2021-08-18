
#include <vector>
#include <memory>

template <typename T>
class Base
{
public:
	void doSomething()
	{
		T& derived = static_cast<T&>(*this);
		// use derived...
	}
};

class Derived : public Base<Derived>
{

};

// CRTP helper
template <typename T>
struct crtp
{
	T& underlying()
	{
		return static_cast<T&>(*this);
	}

	T const& underlying() const
	{
		return static_cast<T const&>(*this);
	}
};

template <typename T>
class Base2 : crtp<T>
{

};

/*
CRTP: allows simulating the virtual functions-based runtime polymorphism at compile time
*/
// 1. provide a class template that will represent the base class for other classes that should be treated
// polymorphically at compile time. Polymorphic functions are invoked from this class:
template <class T>
class control
{
public:
	void draw()
	{
		static_cast<T*>(this)->erase_background();
		static_cast<T*>(this)->paint();
	}
};

class button : public control<button>
{
public:
	void erase_background()
	{
	}

	void paint()
	{}
};

class checkbox : public control<checkbox>
{
public:
	void erase_background() {}
	void paint() {}
};

template <class T>
void draw_control(control<T>& c)
{
	c.draw();
}

void foo()
{
	button b;
	draw_control(b);

	checkbox c;
	draw_control(c);
}

/*
if CRTP need to be stored homogeneously in a container
*/
class controlbase
{
public:
	virtual void draw() = 0;
	virtual ~controlbase() {}
};

template <class T>
class control : public controlbase
{
public:
	virtual void draw() override
	{
		static_cast<T*>(this)->erase_background();
		static_cast<T*>(this)->paint();
	}
};



void draw_controls(std::vector<std::shared_ptr<controlbase>>& v)
{
	for (auto& c : v)
	{
		c->draw();
	}
}

void foo2()
{
	std::vector<std::shared_ptr<controlbase>> v;
	v.emplace_back(std::make_shared<button>());
	v.emplace_back(std::make_shared<checkbox>());

	draw_controls(v);
}