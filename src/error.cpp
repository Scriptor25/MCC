#include <filesystem>
#include <iostream>
#include <mcc/error.hpp>

void mcc::Error(const char *message) noexcept(false)
{
    std::cerr << message << std::endl;
    throw std::runtime_error(message);
}

void mcc::Error(const std::string &message) noexcept(false)
{
    std::cerr << message << std::endl;
    throw std::runtime_error(message);
}

void mcc::Error(const SourceLocation &where, const char *message) noexcept(false)
{
    std::cerr
            << std::filesystem::canonical(where.Filename).string()
            << ':'
            << where.Row
            << ':'
            << where.Col
            << ": "
            << message
            << std::endl;
    throw std::runtime_error(message);
}

void mcc::Error(const SourceLocation &where, const std::string &message) noexcept(false)
{
    std::cerr
            << std::filesystem::canonical(where.Filename).string()
            << ':'
            << where.Row
            << ':'
            << where.Col
            << ": "
            << message
            << std::endl;
    throw std::runtime_error(message);
}

void mcc::Warning(const SourceLocation &where, const char *message)
{
    std::cerr
            << std::filesystem::canonical(where.Filename).string()
            << ':'
            << where.Row
            << ':'
            << where.Col
            << ": "
            << message
            << std::endl;
}

void mcc::Assert(const bool condition, const char *message) noexcept(false)
{
    if (condition)
        return;
    Error(message);
}

void mcc::Assert(const bool condition, const std::string &message) noexcept(false)
{
    if (condition)
        return;
    Error(message);
}

void mcc::Assert(const bool condition, const SourceLocation &where, const char *message) noexcept(false)
{
    if (condition)
        return;
    Error(where, message);
}

void mcc::Assert(const bool condition, const SourceLocation &where, const std::string &message) noexcept(false)
{
    if (condition)
        return;
    Error(where, message);
}
