#ifndef ROLA_UTILS_LOCAL_TIME_HPP
#define ROLA_UTILS_LOCAL_TIME_HPP

#include <chrono>
#include "stlex/chrono_ex.hpp"

#if defined (WIN32)
#include <Windows.h>
#pragma comment (lib, "advapi32.lib")
#elif defined (unix)
#include <sys/time.h>
#include <stdlib.h>
#else
#endif

namespace rola
{
    inline std::chrono::system_clock::time_point current_datetime()
    {
        return std::chrono::system_clock::now();
    }

#if defined (WIN32)

    inline bool adjust_local_time_win32(std::chrono::system_clock::time_point const& tp)
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
        auto tps = split_ymdhms(tp);
        st.wYear = static_cast<WORD>(tps.year);
        st.wMonth = static_cast<WORD>(tps.month);
        st.wDay = static_cast<WORD>(tps.day);
        st.wHour = static_cast<WORD>(tps.hour);
        st.wMinute = static_cast<WORD>(tps.minute);
        st.wSecond = static_cast<WORD>(tps.second);
        st.wMilliseconds = static_cast<WORD>(tps.ms);

        return SetLocalTime(&st) != 0;
    }

#elif defined (unix)
    inline bool adjust_local_time_linux(std::chrono::system_clock::time_point const& tp)
    {
        struct timeval t_timeval;

        t_timeval.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
        t_timeval.tv_usec = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count() % 1000000;
        int rec = settimeofday(&t_timeval, nullptr);
        if (rec != 0)
            return false;

        system("hwclock -w");
        return true;
    }
#else
    inline bool adjust_local_time_xxx(std::chrono::system_clock::time_point const&)
    {
        return false;
    }
#endif

#if defined (WIN32)
#define adjust_local_time adjust_local_time_win32
#elif defined (unix)
#define adjust_local_time adjust_local_time_linux
#else
#define adjust_local_time adjust_local_time_xxx
#endif
} // namespace rola

#endif // ROLA_UTILS_LOCAL_TIME_HPP
