/**
* TΨһ�ƶ�Ϊref������
*/
template <typename T>
void passR(T&& arg)	// arg is a forwarding reference(���Կ�������������)
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