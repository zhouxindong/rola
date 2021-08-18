#ifndef ROLA_STLEX_LOCALTIME_UTILS_HPP
#define ROLA_STLEX_LOCALTIME_UTILS_HPP

#include <thread>

#include "chrono_ex.hpp"

#ifdef HiSilicon
#include "sdk_lib.h"
#endif

namespace rola
{
	inline std::chrono::system_clock::time_point current_datetime()
	{
#ifdef HiSilicon
		int year = 0;
		int month = 0;
		int day = 0;
		int hour = 0;
		int min = 0;
		int sec = 0;

		for (int i = 0; i < chrono::GetTimeRetryCount; ++i)
		{
			if (SDK_GetSystemTime(&year, &month, &day, &hour, &min, &sec) == 0)
			{
				return from_ymdhms(year, month, day, hour, min, sec);
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(chrono::RetryIntervalMs));
			}
		}

		return std::chrono::system_clock::time_point{};
#else
		return std::chrono::system_clock::now();
#endif
	}

#if defined (WIN32)
#include <Windows.h>
#pragma comment (lib, "advapi32.lib")
	inline bool adjustLocalTime_Win32(std::chrono::system_clock::time_point const& new_time, bool = true)
	{
		HANDLE token;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
			&token))
			return false;

		TOKEN_PRIVILEGES tkp;
		tkp.PrivilegeCount = 1;
		LookupPrivilegeValue(nullptr, SE_SYSTEMTIME_NAME, &tkp.Privileges[0].Luid);

		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(token, FALSE, &tkp, sizeof(tkp),
			static_cast<PTOKEN_PRIVILEGES>(nullptr), nullptr);

		DWORD lasterror = GetLastError();
		if (lasterror != ERROR_SUCCESS)
			return false;

		SYSTEMTIME st;

		GetLocalTime(&st);
		auto sptime = split_ymdhms(new_time);
		st.wYear = static_cast<unsigned short>(sptime.year);
		st.wMonth = static_cast<unsigned short>(sptime.month);
		st.wDay = static_cast<unsigned short>(sptime.day);
		st.wHour = static_cast<unsigned short>(sptime.hour);
		st.wMinute = static_cast<unsigned short>(sptime.minute);
		st.wSecond = static_cast<unsigned short>(sptime.second);
		st.wMilliseconds = static_cast<unsigned short>(sptime.ms);

		return SetLocalTime(&st) != 0;
	}
#elif defined (unix)
#if defined(HiSilicon)
	inline bool adjustLocalTime_HiSilicon(std::chrono::system_clock::time_point const& new_time, bool adjust_utc = true)
	{
		auto add8hour = new_time;
		if (adjust_utc)
		{
			add8hour = new_time + std::chrono::hours(8);
		}

		auto sptime = split_ymdhms(add8hour);

		return 0 == SDK_SetSystemTime(sptime.year, sptime.month, sptime.day, sptime.hour, sptime.minute, sptime.second);
	}
#else
#include <sys/time.h>
#include <stdlib.h>
	inline bool adjustLocalTime_Linux(std::chrono::system_clock::time_point const& new_time, bool = true)
	{
		struct timeval t_timeval;

		t_timeval.tv_sec = new_time.to_secs_since_epoch();
		t_timeval.tv_usec = split_ymdhms(new_time).ms * 1000; // us
		int rec = settimeofday(&t_timeval, nullptr);
		if (rec != 0)
			return false;

		system("hwclock -w");
		return true;
	}
#endif
#else
	inline bool adjustLocalTime_Xxx(QDateTime const&)
	{
		return false;
	}
#endif // if defined (WIN32)
	
	#if defined (WIN32)
	#define adjustLocalTime adjustLocalTime_Win32
	#elif defined (unix)
	#if defined(HiSilicon)
	#define adjustLocalTime adjustLocalTime_HiSilicon
	#else
	#define adjustLocalTime adjustLocalTime_Linux
	#endif
	#else
	#define adjustLocalTime adjustLocalTime_Xxx
	#endif
} // namespace rola

#endif // ROLA_STLEX_LOCALTIME_UTILS_HPP
