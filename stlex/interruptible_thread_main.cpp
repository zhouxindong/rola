#include <iostream>
#include "interruptible_thread.hpp"
#include <thread>
#include <chrono>

using namespace std;
using namespace rola;

void func()
{
    condition_variable_any cnd;
    mutex mtx;

    cout << 1 << endl;

    unique_lock<mutex> lck(mtx);
    interruptible_wait(cnd, lck, [] { return false; });
    std::cout << "after interruptible wait" << std::endl;
}

void func2()
{
    while (true)
    {
        interruption_point();
        std::this_thread::yield();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "after func2()" << std::endl;
}

int main_interruptible()
{
    Interruptible_thread th(func);

    th.interrupt();
    th.join();

    Interruptible_thread th2(func2);
    system("pause");
    th2.interrupt();
    th2.join();

    std::cout << "interruptible_thread_main successful\n";
    return 0;
}