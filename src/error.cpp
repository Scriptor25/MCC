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

void mcc::Error(const Location &where, const char *message) noexcept(false)
{
    std::cerr << where.Filename << ':' << where.Row << ':' << where.Col << ": " << message << std::endl;
    throw std::runtime_error(message);
}

void mcc::Error(const Location &where, const std::string &message) noexcept(false)
{
    std::cerr << where.Filename << ':' << where.Row << ':' << where.Col << ": " << message << std::endl;
    throw std::runtime_error(message);
}

void mcc::Assert(const bool condition, const char *message)
{
    if (condition)
        return;
    Error(message);
}

void mcc::Assert(const bool condition, const std::string &message)
{
    if (condition)
        return;
    Error(message);
}

void mcc::Assert(const bool condition, const Location &where, const char *message)
{
    if (condition)
        return;
    Error(where, message);
}

void mcc::Assert(const bool condition, const Location &where, const std::string &message)
{
    if (condition)
        return;
    Error(where, message);
}
