#pragma once

#include <mcc/lex.hpp>

namespace mcc
{
    [[noreturn]] void Error(const char *message) noexcept(false);
    [[noreturn]] void Error(const std::string &message) noexcept(false);
    [[noreturn]] void Error(const SourceLocation &where, const char *message) noexcept(false);
    [[noreturn]] void Error(const SourceLocation &where, const std::string &message) noexcept(false);

    template<typename... Args>
    [[noreturn]] void Error(const char *format, Args &&... args) noexcept(false)
    {
        Error(std::vformat(format, std::make_format_args(args...)));
    }

    template<typename... Args>
    [[noreturn]] void Error(std::string_view format, Args &&... args) noexcept(false)
    {
        Error(std::vformat(std::move(format), std::make_format_args(args...)));
    }

    template<typename... Args>
    [[noreturn]] void Error(const SourceLocation &where, const char *format, Args &&... args) noexcept(false)
    {
        Error(where, std::vformat(format, std::make_format_args(args...)));
    }

    template<typename... Args>
    [[noreturn]] void Error(const SourceLocation &where, std::string_view format, Args &&... args) noexcept(false)
    {
        Error(where, std::vformat(std::move(format), std::make_format_args(args...)));
    }

    void Assert(bool condition, const char *message) noexcept(false);
    void Assert(bool condition, const std::string &message) noexcept(false);
    void Assert(bool condition, const SourceLocation &where, const char *message) noexcept(false);
    void Assert(bool condition, const SourceLocation &where, const std::string &message) noexcept(false);

    template<typename... Args>
    void Assert(const bool condition, const char *format, Args &&... args) noexcept(false)
    {
        Assert(condition, std::vformat(format, std::make_format_args(args...)));
    }

    template<typename... Args>
    void Assert(const bool condition, const std::string_view &format, Args &&... args) noexcept(false)
    {
        Assert(condition, std::vformat(format, std::make_format_args(args...)));
    }

    template<typename... Args>
    void Assert(const bool condition, const SourceLocation &where, const char *format, Args &&... args) noexcept(false)
    {
        Assert(condition, where, std::vformat(format, std::make_format_args(args...)));
    }

    template<typename... Args>
    void Assert(const bool condition, const SourceLocation &where, const std::string_view &format, Args &&... args) noexcept(false)
    {
        Assert(condition, where, std::vformat(format, std::make_format_args(args...)));
    }
}
