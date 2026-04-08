#pragma once

#include <mcc/lex.hpp>

namespace mcc
{
    [[noreturn]] void Error(const char *message) noexcept(false);
    [[noreturn]] void Error(const std::string &message) noexcept(false);
    [[noreturn]] void Error(
            const SourceLocation &where,
            const char *message) noexcept(false);
    [[noreturn]] void Error(
            const SourceLocation &where,
            const std::string &message) noexcept(false);

    void Warning(
            const SourceLocation &where,
            const char *message);

    template<typename... Args>
    [[noreturn]] void Error(
            std::format_string<Args...> format,
            Args &&...args) noexcept(false)
    {
        Error(std::format(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    [[noreturn]] void Error(
            const SourceLocation &where,
            std::format_string<Args...> format,
            Args &&...args) noexcept(false)
    {
        Error(where, std::format(format, std::forward<Args>(args)...));
    }

    void Assert(
            bool condition,
            const char *message) noexcept(false);
    void Assert(
            bool condition,
            const std::string &message) noexcept(false);
    void Assert(
            bool condition,
            const SourceLocation &where,
            const char *message) noexcept(false);
    void Assert(
            bool condition,
            const SourceLocation &where,
            const std::string &message) noexcept(false);

    template<typename... Args>
    void Assert(
            const bool condition,
            std::format_string<Args...> format,
            Args &&...args) noexcept(false)
    {
        if (condition)
            return;
        Assert(condition, std::format(format, std::forward<Args>(args)...));
    }

    template<typename... Args>
    void Assert(
            const bool condition,
            const SourceLocation &where,
            std::format_string<Args...> format,
            Args &&...args) noexcept(false)
    {
        if (condition)
            return;
        Assert(condition, where, std::format(format, std::forward<Args>(args)...));
    }
}
