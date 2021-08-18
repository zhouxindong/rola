
class Point {};
class Color {};

class Shape // pure interface
{
public:
	virtual Point center() const = 0;
	virtual Color color() const = 0;

	virtual void rotate(int) = 0;
	virtual void move(Point p) = 0;

	virtual void redraw() = 0;
};

namespace Impl
{
	class Shape : public virtual ::Shape // implementation
	{
	public:
		// ctors, dtor

		Point center() const override
		{
			return Point{};
		}

		Color color() const override
		{
			return Color{};
		}

		void rotate(int) override {}

		void move(Point p) override {}

		void redraw() override {}
	};
}

class Circle : public virtual Shape // pure interface
{
public:
	virtual int radius() = 0;
};

namespace Impl
{
	class Circle : public virtual ::Circle, public Impl::Shape
	{
	public:
		// ctors, dtor
		int radius() override { return 0; }
	};
}

class Smiley : public virtual Circle
{
public:
};

namespace Impl
{
	class Smiley : public virtual ::Smiley, public Impl::Circle
	{
	public:
		// ctors, dtor
	};
}


// with data
struct Interface
{
	virtual void f();
	virtual int g();
	// no data
};

class Utility
{
	void utility1();
	virtual void utility2(); // customization point
public:
	int x;
	int y;
};

class Derive1 : public Interface, virtual protected Utility
{
	// override Interface functions
	// Maybe override Utility virtual functions
};

class Derive2 : public Interface, virtual protected Utility
{
	// override Interface functions
	// Maybe override Utility virtual functions
};