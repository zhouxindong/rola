#include <iostream>
#include "filex.hpp"
#include <vector>
#include <string>
#include <cassert>

#pragma region read/write binary

int main_filex1()
{
	std::vector<unsigned char> output{ 0,1,2,3,4,5,6,7,8,9 };
	unsigned char* input = nullptr;
	size_t readb = 0;

	if (rola::write_data("sample.bin", reinterpret_cast<char*>(output.data()), output.size()))
	{
		if ((readb = rola::read_data("sample.bin", [&input](size_t const length) {
			input = new unsigned char[length];
			return reinterpret_cast<char*>(input);
			})) > 0)
		{
			auto cmp = ::memcmp(output.data(), input, output.size());
			std::cout << (cmp == 0 ? "equal" : "not equal") << std::endl;
		}
	}
	delete[] input;
	std::cout << "filex successful: read and write binary data\n";
	return 0;
}

#pragma endregion

#pragma region read/write pod object

struct foopod
{
	bool a;
	char b;
	int c[2];
};

bool operator==(foopod const& f1, foopod const& f2)
{
	return f1.a == f2.a && f1.b == f2.b &&
		f1.c[0] == f2.c[0] && f1.c[1] == f2.c[1];
}

int main_filexpod()
{
	std::vector<foopod> output{
		{true, '1', {1,2}},
		{true, '2', {3,4}},
		{false, '3', {4, 5}}
	};
	rola::serialize("sample.bin", output);

	auto input = rola::deserialize<std::vector<foopod>>("sample.bin");
	assert(output.size() == input.size());
	for (auto i = 0; i < output.size(); ++i)
	{
		assert(output[i] == input[i]);
	}

	std::cout << "filex successful: read and write pod object\n";
	return 0;
}

#pragma endregion

#pragma region read/write non-pod object

class foo
{
	int i;
	char c;
	std::string s;

public:
	foo(int const i = 0, char const c = 0, std::string const& s = {})
		:i(i), c(c), s(s)
	{}

	foo(foo const&) = default;
	foo& operator=(foo const&) = default;

	bool operator==(foo const& rhv) const
	{
		return i == rhv.i &&
			c == rhv.c &&
			s == rhv.s;
	}

	bool operator!=(foo const& rhv) const
	{
		return !(*this == rhv);
	}

	bool write(std::ofstream& ofile) const
	{
		ofile.write(reinterpret_cast<const char*>(&i), sizeof(i));
		ofile.write(&c, sizeof(c));
		auto size = static_cast<int>(s.size());
		ofile.write(reinterpret_cast<char*>(&size), sizeof(size));
		ofile.write(s.data(), s.size());
		return !ofile.fail();
	}

	bool read(std::ifstream& ifile)
	{
		ifile.read(reinterpret_cast<char*>(&i), sizeof(i));
		ifile.read(&c, sizeof(c));
		auto size{ 0 };
		ifile.read(reinterpret_cast<char*>(&size), sizeof(size));
		s.resize(size);
		ifile.read(reinterpret_cast<char*>(&s.front()), size);

		return !ifile.fail();
	}

	friend std::ofstream& operator<<(std::ofstream& ofile, foo const& f);
	friend std::ifstream& operator>>(std::ifstream& ifile, foo& f);
};

inline std::ofstream& operator<<(std::ofstream& ofile, foo const& f)
{
	f.write(ofile);
	return ofile;
}

inline std::ifstream& operator>>(std::ifstream& ifile, foo& f)
{
	f.read(ifile);
	return ifile;
}

int main_filexnonpod()
{
	std::vector<foo> output{
	{12, '1', "one"},
	{23, '2', "two"},
	{45, '3', "three"}
	};
	rola::serialize("sample.bin", output);

	auto input = rola::deserialize<std::vector<foo>>("sample.bin");
	assert(output.size() == input.size());
	for (auto i = 0; i < output.size(); ++i)
	{
		assert(output[i] == input[i]);
	}
	std::cout << "filex successful: non-pod object\n";
	return 0;
}

#pragma endregion