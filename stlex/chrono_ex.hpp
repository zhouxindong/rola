#ifndef ROLA_STLEX_CHRONO_EX_HPP
#define ROLA_STLEX_CHRONO_EX_HPP

#include <chrono>
#include <ostream>
#include <tuple>
#include <ctime>
#include <iomanip>

namespace rola
{
	namespace chrono
	{
		/*typedef std::chrono::duration<int, std::ratio<3600 * 24>> days;*/
		using days = std::chrono::duration<
			std::chrono::hours::rep,
			std::ratio_multiply<std::chrono::hours::period, std::ratio<24>>>;

		inline constexpr days  operator ""_days(unsigned long long h)
		{
			return days{ h };
		}

		struct HMS
		{
			long hours;
			long minutes;
			long seconds;
			long millis;
		};

		struct YMDHMS
		{
			long year;
			long month;
			long day;
			long hour;
			long minute;
			long second;
			long ms;
		};

		constexpr int GetTimeRetryCount = 100;
		constexpr int RetryIntervalMs	= 10;
	} // namespace chrono

	// return days since 1970-01-01
	// negative values indicate days prior to 1970-01-01
	inline constexpr int days_from_civil(int y, int m, int d) noexcept
	{
		y -= m <= 2;
		const int era = (y >= 0 ? y : y - 399) / 400;
		const unsigned yoe = static_cast<unsigned>(y - era * 400);				// [0, 399]
		const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;	// [0, 365]
		const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;				// [0, 146096]
		return era * 146097 + (doe)-719468;
	}

	// get milliseconds count from a time point
	inline std::chrono::milliseconds get_ms(std::chrono::system_clock::time_point const& tp)
	{
		std::chrono::system_clock::duration dt = tp.time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(dt);
	}	

	// split milliseconds to <hour, minute, second, millisecond>
	inline rola::chrono::HMS split_hms(std::chrono::milliseconds ms)
	{
		rola::chrono::HMS hms;
		hms.hours = std::chrono::duration_cast<std::chrono::hours>
			(ms).count();
		hms.minutes = std::chrono::duration_cast<std::chrono::minutes>
			(ms % std::chrono::hours(1)).count();
		hms.seconds = static_cast<long>(std::chrono::duration_cast<std::chrono::seconds>
			(ms % std::chrono::minutes(1)).count());
		hms.millis = static_cast<long>(std::chrono::duration_cast<std::chrono::milliseconds>
			(ms % std::chrono::seconds(1)).count());
		return hms;
	}

	inline rola::chrono::HMS split_hms(long long ms)
	{
		std::chrono::milliseconds val(ms);
		return split_hms(val);
	}

	inline rola::chrono::YMDHMS
		split_ymdhms(std::chrono::system_clock::time_point const& tp)
	{
		rola::chrono::YMDHMS ymdhms;
		auto tt = std::chrono::system_clock::to_time_t(tp);
		tm local_tm;
#if defined (WIN32)
		::localtime_s(&local_tm, &tt);
#else
		local_tm = *::localtime(&tt);
#endif
		ymdhms.year = local_tm.tm_year + 1900;
		ymdhms.month = local_tm.tm_mon + 1;
		ymdhms.day = local_tm.tm_mday;
		ymdhms.hour = local_tm.tm_hour;
		ymdhms.minute = local_tm.tm_min;
		ymdhms.second = local_tm.tm_sec;

		auto dt = tp.time_since_epoch();
		ymdhms.ms = static_cast<long>(std::chrono::duration_cast<std::chrono::milliseconds>
			(dt % std::chrono::seconds(1)).count());

		return ymdhms;
	}

	inline std::chrono::system_clock::time_point 
		from_ymdhms(int year, int month, int day, int hour, int minute, int sec, int ms = 0)
	{
		struct std::tm t;
		t.tm_sec = sec;
		t.tm_min = minute;
		t.tm_hour = hour;
		t.tm_mday = day;
		t.tm_mon = month - 1;
		t.tm_year = year - 1900;
		
		std::time_t tt = std::mktime(&t);
		if (tt == -1)
			throw "no valid system time";
		return (std::chrono::system_clock::from_time_t(tt) + std::chrono::milliseconds(ms));
	}

	inline int64_t to_secs_since_epoch(std::chrono::system_clock::time_point const& tp)
	{
		return std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
	}

	inline int64_t to_msecs_since_epoch(std::chrono::system_clock::time_point const& tp)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
	}

#pragma region operator<<

	inline std::ostream& operator<<(std::ostream& s, chrono::HMS const& hms)
	{
		s << hms.hours << ':';
		s << std::setw(2) << std::setfill('0') << hms.minutes << ':';
		s << std::setw(2) << std::setfill('0') << hms.seconds << '.';
		s << std::setw(3) << std::setfill('0') << hms.millis;
		return s;
	}

	inline std::ostream& operator<<(std::ostream& s, chrono::YMDHMS const& ymdhms)
	{
		s << ymdhms.year << '-';
		s << std::setw(2) << std::setfill('0') << ymdhms.month << '-';
		s << std::setw(2) << std::setfill('0') << ymdhms.day << ' ';
		s << chrono::HMS{ ymdhms.hour, ymdhms.minute, ymdhms.second, ymdhms.ms };
		return s;
	}

	template <typename V, typename R>
	inline std::ostream& operator<<(std::ostream& s, const std::chrono::duration<V, R>& d)
	{
		s << "[" << d.count() << " of "
			<< R::num << "/"
			<< R::den << "]";
		return s;
	}

	inline std::ostream& operator<<(std::ostream& s, std::chrono::system_clock::time_point const& tp)
	{
		s << split_ymdhms(tp);
		return s;
	}

#pragma endregion

#pragma region utils

	template <typename Time = std::chrono::microseconds,
		typename Clock = std::chrono::high_resolution_clock>
		struct perf_timer
	{
		template <typename F, typename ... Args>
		static Time duration(F&& f, Args&& ... args)
		{
			auto start = Clock::now();
			//std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
			std::forward<F>(f)(std::forward<Args>(args)...);
			auto end = Clock::now();

			return std::chrono::duration_cast<Time>(end - start);
		}
	};

#pragma endregion
} // namespace rola

#endif // !ROLA_STLEX_CHRONO_EX_HPP
