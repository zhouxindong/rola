/**
* T唯一推断为ref的情形
*/
template <typename T>
void passR(T&& arg)	// arg is a forwarding reference(可以看成是万能引用)
{
	T x;	// for passed lvalues, x is a reference, which requires an initializer
}
void foo()
{
	passR(42);	// T -> int
	int i;
	passR(i);	// ERROR! T -> int&
}

int& lvalue();
int&& xvalue();
int prvalue();