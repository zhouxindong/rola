#ifndef ROLA_EASY_LOG_EASY_LOG_QT_HPP
#define ROLA_EASY_LOG_EASY_LOG_QT_HPP

#include <string>
#include <QString>
#include <QDateTime>

#include "easy_log/basic_logger.hpp"

#define LOGDB LOG(DEBUG)

namespace rola
{
    inline console_debug& qConsoleDebug()
    {
        return console_debug::get("anyname");
    }

    inline file_debug& qFileDebug(std::string const& filename)
    {
        return file_debug::get(filename);
    }

    inline console_debug& operator<<(console_debug& o, char const* msg)
    {
        std::string s(msg);
        o << (LOGDB << s);
        return o;
    }

    inline file_debug& operator<<(file_debug& o, char const* msg)
    {
        std::string s(msg);
        o << (LOGDB << s);
        return o;
    }

    inline console_debug& operator<<(console_debug& o, bool t)
    {
        o << (t ? LOGDB << "true" : LOGDB << "false");
        return o;
    }

    inline file_debug& operator<<(file_debug& o, bool t)
    {
        o << (t ? LOGDB << "true" : LOGDB << "false");
        return o;
    }

    template <typename T>
    inline console_debug& operator<<(console_debug& o, T t)
    {
        o << (LOGDB << t);
        return o;
    }

    template <typename T>
    inline file_debug& operator<<(file_debug& o, T t)
    {
        o << (LOGDB << t);
        return o;
    }

    inline console_debug& operator<<(console_debug& o, std::string const& s)
    {
        o << (LOGDB << s);
        return o;
    }

    inline file_debug& operator<<(file_debug& o, std::string const& s)
    {
        o << (LOGDB << s);
        return o;
    }

    inline console_debug& operator<<(console_debug& o, QString const& t)
    {
        o << (LOGDB << t.toStdString());
        return o;
    }

    inline file_debug& operator<<(file_debug& o, QString const& t)
    {
        o << (LOGDB << t.toStdString());
        return o;
    }

    inline console_debug& operator<<(console_debug& o, void const* t)
    {
        o << (LOGDB << t);
        return o;
    }

    inline file_debug& operator<<(file_debug& o, void const* t)
    {
        o << (LOGDB << t);
        return o;
    }

    inline console_debug& operator<<(console_debug& o, std::nullptr_t)
    {
        o << (LOGDB << "(nullptr)");
        return o;
    }

    inline file_debug& operator<<(file_debug& o, std::nullptr_t)
    {
        o << (LOGDB << "(nullptr)");
        return o;
    }

    inline console_debug& operator<<(console_debug& o, QDateTime const& t)
    {
        o << (LOGDB << t.toString().toStdString());
        return o;
    }

    inline file_debug& operator<<(file_debug& o, QDateTime const& t)
    {
        o << (LOGDB << t.toString().toStdString());
        return o;
    }
} // namespace rola

#endif // !ROLA_EASY_LOG_EASY_LOG_QT_HPP
