#ifndef ROLA_SAFECPP_SCPP_ASSERT_HPP
#define ROLA_SAFECPP_SCPP_ASSERT_HPP

#include <sstream>
#include <iostream>
#include <stdlib.h>

#ifdef SCPP_THROW_EXCEPTION_ON_BUG
#include <exception>

namespace rola
{
	class ScppAssertFailedException : public std::exception
	{
	public:
		ScppAssertFailedException(const char* file_name,
			unsigned line_number,
			const char* message)
		{
			std::ostringstream s;
			s << "SCPP assertion failed with message '" << message
				<< "' in file " << file_name << " #" << line_number;
			what_ = s.str();
		}

		virtual const char* what() const throw()
		{
			return what_.c_str();
		}

		virtual ~ScppAssertFailedException() throw() {}

	private:
		std::string what_;
	};
} // namespace rola

#endif // SCPP_THROW_EXCEPTITION_ON_BUG

inline void SCPP_AssertErrorHandler(const char* file_name,
	unsigned line_number,
	const char* message)
{
	// this is a good place to put your debug breakpoint:
	// you can also add writing of the same info into a log file if appropriate.

#ifdef SCPP_THROW_EXCEPTION_ON_BUG
	throw rola::ScppAssertFailedException(file_name, line_number, message);
#else
	std::cerr << message << " in file " << file_name << " #" << line_number << std::endl << std::flush;
	exit(1);
#endif
}

// permanent sanity check macro
#define SCPP_ASSERT(condition, msg)					\
	if(!(condition)) {								\
		std::ostringstream s;						\
		s << msg;									\
		SCPP_AssertErrorHandler(					\
			__FILE__, __LINE__, s.str().c_str());	\
	}

#ifdef _DEBUG
#define SCPP_TEST_ASSERT_ON
#endif

// temporary (for testing only) sanity check macro
#ifdef SCPP_TEST_ASSERT_ON
#define SCPP_TEST_ASSERT(condition, msg) SCPP_ASSERT(condition, msg)
#else
#define SCPP_TEST_ASSERT(condition, msg) // do nothing
#endif

#endif // !ROLA_SAFECPP_SCPP_ASSERT_HPP
