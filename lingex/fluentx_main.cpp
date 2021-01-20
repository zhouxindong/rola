#include "fluentx.hpp"
#include "loopable.hpp"

#include <iostream>
#include <cassert>

using namespace rola;
using namespace rola::Flux;

int main_fluentx()
{
	int i = 0;

	// If...Then
	Flux::If(8 % 2 == 0)->Then([&]() {++i; });
	assert(i == 1);
	Flux::If([]() {return 9 % 2 == 1; })->Then([&]() {++i; });
	assert(i == 2);
	Flux::If(8 % 2 == 1)->Then([&]() {++i; });
	assert(i == 2);
	Flux::If([]() {return 9 % 2 == 0; })->Then([&]() {++i; });
	assert(i == 2);

	// If...Then...Else
	i = 0;
	int j = 0;

	If(8 % 2 == 0)->
		Then([&]() {++i; }).
		Else([&]() {++j; });
	assert(i == 1);
	assert(j == 0);

	If(8 % 2 == 1)->
		Then([&]() {++i; }).
		Else([&]() {++j; });
	assert(i == 1);
	assert(j == 1);

	// ElseIf
	auto result = 0;
	auto expected = 3;
	If([]() {return 8 % 2 == 1; })
		->Then([&]() { result = 1; })
		.ElseIf([]() {return 8 % 2 == 1; }).Then([&]() { result = 2; })
		.Else([&]() { result = 3; });
	assert(result == expected);

	// And
	result = 0;
	expected = 1;
	If(true)->And(true).Then([&]() { result = 1; });
	assert(result == expected);

	// AndNot
	result = 0;
	expected = 1;
	If(true)->And(true).AndNot(false).Then([&]() { result = 1; });
	assert(result == expected);
	If(true)->And(true).AndNot(true).Then([&]() { result = 2; });
	assert(result == expected);

	// Or
	result = 0;
	expected = 1;
	If([]() { return true; })->And(true).AndNot(true).Or(true).Then([&]() { result = 1; });
	assert(result == expected);

	// OrNot
	result = 0;
	expected = 1;
	If([]() { return true; })
		->And([]() { return true; })
		.AndNot([]() { return true; })
		.OrNot(false)
		.Then([&]() { result = 1; });
	assert(result == expected);

	// Xor
	result = 0;
	expected = 0;
	If(true)->And(true).AndNot(true).OrNot(false).Xor(true).Then([&]() { result = 1; });
	assert(result == expected);

	expected = 2;
	If(true)->And(true).AndNot(true).OrNot(false).Xor(false).Then([&]() { result = 2; });
	assert(result == expected);

	// Xnor
	result = 0;
	expected = 0;
	If(true)->And(true).AndNot(true).OrNot(false).Xnor(false).Then([&]() { result = 1; });
	assert(result == expected);

	expected = 2;
	If(true)->And(true).AndNot(true).OrNot(false).Xnor(true).Then([&]() { result = 2; });
	assert(result == expected);

	// While(cond, action)
	result = 0;
	While([&]() {return result != 6; }, [&]() {++result; });
	assert(result == 6);

	// While(cond)
	result = 0;
	auto sum = 0;
	While([&]() {return ++result != 6; })->Do([&]() { sum += result; });
	assert(15 == sum);

#pragma region

	// While-Do-LaterBreak
	result = 0;
	expected = 0;
	While([&]() 
		{
		expected += 1;
		return result < 6;
		})
		->
			LaterBreak([&]() {return result == 4; })
			.Do([&]() {result += 1; });
	assert(4 == expected);

	// While-EarlyBreak-Do
	result = 0;
	expected = 0;
	While([&]()
		{
			expected += 1;
			return result < 6;
		})
		->
			EarlyBreak([&]() {return result == 4; })
			.Do([&]() {result += 1; });
	assert(5 == expected);

	// While-Do-LaterContinue
	result = 0;
	expected = 0;
	While([&]()
		{
			expected += 1;
			return result < 6;
		})
		->
			LaterContinue([&]() {return result == 4; })
			.Do([&]() {result += 1; });
	assert(7 == expected);

	// While-EarlyContinue-Do
	result = 0;
	expected = 0;
	While([&]()
		{
			expected += 1;
			return result < 6 && expected < 100;
		})
		->
			EarlyContinue([&]() { return result == 4; })
			.Do([&]() {result += 1; });
	assert(100 == expected);

#pragma endregion

	result = 0;
	Do([&]() {++result; })
		->While([&]() {return result < 5; });
	assert(result == 5);

	std::cout << "fluentx_main successful\n";
	return 0;
}