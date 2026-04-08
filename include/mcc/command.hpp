#pragma once

#include <mcc/common.hpp>

namespace mcc
{
    class CommandVector
    {
    public:
        explicit CommandVector(std::vector<CommandT> &commands)
            : m_Commands(commands)
        {
        }

        CommandVector &Append(CommandT command)
        {
            m_Commands.push_back(std::move(command));
            return *this;
        }

        template<typename... Args>
        CommandVector &Append(
                std::format_string<Args...> format,
                Args &&...args)
        {
            m_Commands.push_back(std::format(std::move(format), std::forward<Args>(args)...));
            return *this;
        }

    private:
        std::vector<CommandT> &m_Commands;
    };
}
