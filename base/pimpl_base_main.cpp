#include "pimpl_base.hpp"
#include <iostream>

class demo_class_private_data : public rola::pimpl_base_private_data
{
public:
	demo_class_private_data()
		: height{ 0.0 }
	{}
public:
	double height;
};

class demo_class : public rola::pimpl_base
{
	PRIVATE_DATA_CLASS(demo_class)

public:
	demo_class()
		: pimpl_base(*new demo_class_private_data)
	{}

public:
	int get_version() const
	{
		return p_data()->Version;
	}

	void set_version(int version)
	{
		p_data()->Version = version;
	}

	void set_height(double h)
	{
		p_data()->height = h;
	}

	double get_height() const
	{
		return p_data()->height;
	}
};

int main_pb()
{
	demo_class dc;
	dc.set_version(99);
	dc.set_height(17.23);
	std::cout << dc.get_version() << "\n";
	std::cout << dc.get_height() << "\n";
	return 0;
}