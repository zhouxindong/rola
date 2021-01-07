#include "easy_async.hpp"
#include <iostream>

int main_task()
{
	// 1. task object
	// create a task which runs asynchronously
	auto my_task = async::spawn([] {return 42; });

	// do other stuff while the task is not finshed
	while (!my_task.ready())
	{
		// do stuff...
	}

	// wait for the task to complete without getting the result
	my_task.wait();

	// wait for the task to complete and get the result
	int answer = my_task.get();

	// create a task with a preset value
	auto my_task2 = async::make_task(42);

	// print the value stored in the task
	std::cout << my_task2.get() << std::endl;


	// 2. continuations
	// spawn a task
	auto t1 = async::spawn([] {return 42; });
	
	// chain a value-based continuation
	auto t2 = t1.then([](int result) {return result; });

	// chain a task-based continuation
	t2.then([](async::task<int> parent) {std::cout << parent.get() << std::endl; });

	// equivalent code with direct chaining
	async::spawn([] {
		return 42;
		}).
		then([](int result)
			{
				return result;
			}).
		then([](async::task<int> parent) 
			{
				std::cout << parent.get() << std::endl;				
			});

	// 3. composition
	// using when_any to find task which finishes first
		async::task<char> tasks[] = 
		{			
			async::spawn([] {return 'A'; }),
			async::spawn([] {return 'B'; }),
			async::spawn([] {return 'C'; })
		};
		async::when_any(tasks).then(
			[](async::when_any_result<std::vector<async::task<char>>> result)
			{
				std::cout << "Task " << result.index << " finished with value "
					<< result.tasks[result.index].get() << std::endl;
			}
		);

		// using when_all to combine results of multiple tasks
		auto a = async::spawn([] {return std::string("Hello "); });
		auto b = async::spawn([] {return std::string("World!"); });
		async::when_all(a, b).then(
			[](std::tuple<async::task<std::string>, async::task<std::string>> result)
			{
				std::cout << std::get<0>(result).get() << std::get<1>(result).get() << std::endl;
			}
		);

		// 4. exceptions
		async::spawn([]
			{
				throw std::runtime_error("some error");
				return 99;
			}
		).then([](int result)
			{
				// this is not executed because it is a value-based continuation
				std::cout << result << std::endl;
			}
		).then([](async::task<void> t)
			{
				// the exception from the parent task is propagated through value-based
				// continuations and caught in task-based continuations
				try
				{
					t.get();
				}
				catch (const std::runtime_error& e)
				{
					std::cout << e.what() << std::endl;
				}
			}
		);

		// 5. cancellation
		// create a token
			async::cancellation_token c;
			auto t3 = async::spawn([&c]
				{
					// throw an exception if the task has been canceled
					async::interruption_point(c);

					// this is equivalent to the following:
					// if (c.is_canceled())
					//     async::cancel_current_task(); // throws async::task_canceled
				}
			);

			// set the token to cancel work
			c.cancel();

			// because the task and c.cancel() are executing concurrently, the token
			// may or may not be canceled by the time the task reaches the interruption
			// point. Depending on which comes first, this may throw a
			// async::task_canceled exception.			
			try
			{
				t3.get();
			}
			catch (const async::task_canceled& e)
			{
				std::cout << "catch async::task_canceled" << std::endl;
			}

			// 6. shared task
			// parent task, note the use of .shared() to get a shared task
			auto t4 = async::spawn(
				[] {
					std::cout << "Parent task" << std::endl;
				}
			).share();

			// first child, using value-based continuation
			t4.then([] {
				std::cout << "Child task A" << std::endl;
				}
			);

			// second child, using task-based continuation
			t4.then([](async::shared_task<void>)
				{
					std::cout << "Child task B" << std::endl;
				}
			);

			// 7. task unwraping
			// the outer task is task<int>, and its result is set when the inner task
			// completes (instead of block to wait inner completed)
			async::spawn([] {
				std::cout << "Outer task" << std::endl;
				// return a task<int>
				return async::spawn([] {
					std::cout << "Inner task" << std::endl;
					return 42;
					});
				}
			).then([](int result)
				{
					std::cout << "Continuation task" << std::endl;
					std::cout << result << std::endl;
				}
			);

			// 8. event task
			// create an event
			async::event_task<int> e;

			// get a task associated with the event
			auto t5 = e.get_task();
			
			// add a continuation to the task
			t5.then([](int result)
				{
					std::cout << result << std::endl;
				}
			);

			// set the event value, which will cause the continuation to run
			e.set(99);

			// to set an exception:
			// e.cancel();
			// e.set_exception(std::make_exception_ptr(async::task_canceled));
			// these are equivalent but cancel is slightly more efficient

			// 9. local task
			auto&& t6 = async::local_spawn(
				[] {
					std::cout << "Local task" << std::endl;
				}
			);

	return 0;
}