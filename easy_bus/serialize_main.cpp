#include "serialize.hpp"
#include <iostream>

int main()
{
	std::cout << "serialize_main successful" << std::endl;
	return 0;
}

class cbox : public Serializable<cbox>
{
public:
    int a;
    double b;
    string str;

    static string serialize(const cbox& inc)
    {
        OutEngine x;
        x << inc.a << inc.b << inc.str;
        return x.str();
    }
    static int deserialize(string inc, cbox& box)
    {
        InEngine x(inc);
        x >> box.a >> box.b >> box.str;
        return x.donesize();
    }
};
int main()
{
    cbox box;
    box.a = 11;
    box.b = 6.6;
    box.str = "Hello World";

    cbox box3;
    box3.a = 33;
    box3.b = 12.5;
    box3.str = "Yummy Hamburger!";

    OutEngine oe;
    oe << box << box3;

    string b = oe.str();
    cout << b << endl;

    cbox box2;
    cbox box4;
    InEngine ie(b);
    ie >> box2 >> box4;

    cout << box2.a << endl;
    cout << box2.b << endl;
    cout << box2.str << endl;

    cout << box4.a << endl;
    cout << box4.b << endl;
    cout << box4.str << endl;
    return 0;
