#ifndef ROLA_STLEX_FILEX_HPP
#define ROLA_STLEX_FILEX_HPP

#include <vector>
#include <fstream>
#include <string>
#include <utility>
#include <functional>
#include <type_traits>

namespace rola
{
	inline bool write_data(char const* const filename, char const* const data, size_t const size)
	{
		auto success = false;
		std::ofstream ofile(filename, std::ios::binary);
		if (ofile.is_open()) // file_buf open
		{
			try
			{
				ofile.write(data, size);
				success = true;
			}
			catch (std::ios_base::failure&)
			{
				// handle the error
			}
			ofile.close();
		}
		return success;
	}

	inline size_t read_data(char const* const filename, std::function<char* (size_t const)> allocator)
	{
		size_t readbytes = 0;
		std::ifstream ifile(filename, std::ios::ate | std::ios::binary);

		if (ifile.is_open())
		{
			auto length = static_cast<size_t>(ifile.tellg());
			ifile.seekg(0, std::ios_base::beg);

			auto buffer = allocator(length);

			try
			{
				ifile.read(buffer, length);
				readbytes = static_cast<size_t>(ifile.gcount());
			}
			catch (std::ios_base::failure&)
			{
				// handle the error
			}

			ifile.close();
		}

		return readbytes;
	}

	inline std::vector<unsigned char> read_directly(char const* const filename)
	{
		std::vector<unsigned char> input;
		std::ifstream ifile(filename, std::ios::binary);
		if (ifile.is_open())
		{
			input = std::vector<unsigned char>(
				std::istreambuf_iterator<char>(ifile),
				std::istreambuf_iterator<char>());
			ifile.close();
		}
		return input;
	}

	namespace details
	{
		template <bool IsPod, typename Cnt>
		struct object_serialize_helper
		{
			static bool serialize(const char* const filename, const Cnt& data)
			{
				std::ofstream ofile(filename, std::ios::binary);
				if (ofile.is_open())
				{
					for (auto const& value : data)
					{
						ofile.write(reinterpret_cast<const char*>(&value), sizeof(value));
					}
					ofile.close();
					return true;
				}
				return false;
			}

			static Cnt deserialize(const char* const filename)
			{
				Cnt input;
				std::ifstream ifile(filename, std::ios::binary);
				if (ifile.is_open())
				{
					while (true)
					{
						typename Cnt::value_type value;
						ifile.read(reinterpret_cast<char*>(&value), sizeof(value));
						if (ifile.fail() || ifile.eof())
							break;
						input.push_back(value);
					}
					ifile.close();
				}
				return input;
			}
		};

		template <typename Cnt>
		struct object_serialize_helper<false, Cnt>
		{
			static bool serialize(const char* const filename, const Cnt& data)
			{
				std::ofstream ofile(filename, std::ios::binary);
				if (ofile.is_open())
				{
					auto size = static_cast<int>(data.size());
					ofile.write(reinterpret_cast<char*>(&size), sizeof(size));

					for (auto const& value : data)
					{
						if (!value.write(ofile))
							return false;
					}
					ofile.close();
					return true;
				}
				return false;
			}

			static Cnt deserialize(const char* const filename)
			{
				Cnt input;
				std::ifstream ifile(filename, std::ios::binary);
				if (ifile.is_open())
				{
					auto size{ 0 };
					ifile.read(reinterpret_cast<char*>(&size), sizeof(size));

					for (auto i = 0; i < size; ++i)
					{
						typename Cnt::value_type value;
						if (ifile.fail() || ifile.eof())
							break;
						if (!value.read(ifile))
							break;

						input.push_back(value);
					}
					ifile.close();
				}
				return input;
			}
		};
	} // namespace details

	template <typename Cnt>
	inline bool serialize(const char* const filename, const Cnt& data)
	{
		return details::object_serialize_helper<std::is_pod_v<typename Cnt::value_type>, Cnt>::serialize(filename, data);
	}

	template <typename Cnt>
	inline Cnt deserialize(const char* const filename)
	{
		return details::object_serialize_helper<std::is_pod_v<typename Cnt::value_type>, Cnt>::deserialize(filename);
	}
} // namespace rola

#endif // !ROLA_STLEX_FILEX_HPP
