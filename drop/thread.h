
#include <mutex>

#pragma region lock

class some_big_object
{};

void swap(some_big_object& lhs, some_big_object& rhs)
{}

class X
{
private:
	some_big_object some_detail;
	mutable std::mutex m;
public:
	X(some_big_object const& sd) :some_detail(sd) {}

	friend void swap(X& lhs, X& rhs)
	{
		if (&lhs == &rhs)
			return;

#pragma region use lock and lock_guard

		//std::lock(lhs.m, rhs.m); // acquire two mutex at same time
		//std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock); // lock_a take the owner of a mutex
		//std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock); // lock_b take the owner of b mutex
		//swap(lhs.some_detail, rhs.some_detail);

#pragma endregion

#pragma region use unique_lock and lock 

		std::unique_lock<std::mutex> lock_a(lhs.m, std::defer_lock); // acquire mutex later
		std::unique_lock<std::mutex> lock_b(rhs.m, std::defer_lock); // acquire mutex later
		std::lock(lock_a, lock_b); // acquire tow mutexs
		swap(lhs.some_detail, rhs.some_detail);

#pragma endregion
	}
};

#pragma region move the owner-ship of mutex

void prepare_data() {}
void do_something() {}

std::unique_lock<std::mutex> get_lock()
{
	extern std::mutex some_mutex;
	std::unique_lock<std::mutex> lk(some_mutex);
	prepare_data();
	return lk;
}

void process_data()
{
	std::unique_lock<std::mutex> lk(get_lock());
	do_something();
}

#pragma endregion

#pragma endregion

#pragma region condition_variable

#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>

bool more_data_to_prepare()
{
	return false;
}

struct data_chunk
{};

data_chunk prepare_data2()
{
	return data_chunk();
}

void process(data_chunk&)
{}

bool is_last_chunk(data_chunk&)
{
	return true;
}

std::mutex mut;
std::queue<data_chunk> data_queue;
std::condition_variable data_cond;

void data_preparation_thread()
{
	while (more_data_to_prepare())
	{
		data_chunk const data = prepare_data2();
		std::lock_guard<std::mutex> lk(mut);
		data_queue.push(data);
		data_cond.notify_one();
	}
}

void data_processing_thread()
{
	while (true)
	{
		std::unique_lock<std::mutex> lk(mut);
		// 1. check wait callable return value, if false, unlock lk, thread wait here
		// 2. received notify, thread wake up and lock lk, check wait callable return value again
		// 3. this return true, return from wait keep the lk locked
		// (so must use unique_lock<>, because when wait callable return false, this thread must unlock)
		data_cond.wait(lk, [] {return !data_queue.empty(); });
		data_chunk data = data_queue.front();
		data_queue.pop();
		lk.unlock();
		process(data);
		if (is_last_chunk(data))
			break;
	}
}

#pragma endregion

#pragma region packaged_task<> GUI thread simulate

#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>

std::mutex m;
std::deque<std::packaged_task<void()> > tasks;

bool gui_shutdown_message_received();
void get_and_process_gui_message();

void gui_thread()
{
	while (!gui_shutdown_message_received())
	{
		get_and_process_gui_message();
		std::packaged_task<void()> task;
		{
			std::lock_guard<std::mutex> lk(m);
			if (tasks.empty())
				continue;
			task = std::move(tasks.front());
			tasks.pop_front();
		}
		task();
	}
}

std::thread gui_bg_thread(gui_thread);

template<typename Func>
std::future<void> post_task_for_gui_thread(Func f)
{
	std::packaged_task<void()> task(f);
	std::future<void> res = task.get_future();
	std::lock_guard<std::mutex> lk(m);
	tasks.push_back(std::move(task));
	return res;
}

#pragma endregion

#pragma region wait for a condition variable

#include <condition_variable>
#include <mutex>
#include <chrono>
std::condition_variable cv;
bool done;
std::mutex m;
bool wait_loop()
{
    auto const timeout = std::chrono::steady_clock::now() +
        std::chrono::milliseconds(500);
    std::unique_lock<std::mutex> lk(m);
    while (!done)
    {
        if (cv.wait_until(lk, timeout) == std::cv_status::timeout)
            break;
    }
    return done;
}

#pragma endregion