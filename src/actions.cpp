#include <iomanip>
#include <iostream>
#include <ranges>
#include <mcc/actions.hpp>

mcc::Actions::Actions(const std::vector<Action> &actions)
{
    for (auto &action: actions)
        m_Actions.emplace(action.Pattern, action);
}

void mcc::Actions::operator()(const int argc, const char **argv)
{
    m_Executable = argv[0];
    m_Action = argc >= 2 && m_Actions.contains(argv[1])
                   ? &m_Actions.at(argv[1])
                   : nullptr;
    m_Strings.clear();
    m_Flags.clear();

    if (argc <= 2 || !m_Action)
        return;

    for (auto i = 2; i < argc; ++i)
    {
        for (unsigned j = 0; j < m_Action->Parameters.size(); ++j)
        {
            auto &parameter = m_Action->Parameters[j];

            if (parameter.Pattern != argv[i])
                continue;

            if (parameter.IsFlag)
                m_Flags.emplace(j);
            else
                m_Strings.emplace(j, argv[++i]);

            break;
        }
    }
}

unsigned mcc::Actions::ActionID() const
{
    return m_Action ? m_Action->ID : ~0;
}

bool mcc::Actions::String(const unsigned index, std::string &destination) const
{
    if (!m_Strings.contains(index))
        return false;
    destination = m_Strings.at(index);
    return true;
}

bool mcc::Actions::Flag(const unsigned index) const
{
    return m_Flags.contains(index);
}

void mcc::Actions::Print() const
{
    std::cerr << "MCC - Minecraft Command Compiler" << std::endl;
    std::cerr << "Usage: mcc <action> [<option|flag>...]" << std::endl;

    std::cerr << "Actions:" << std::endl;
    for (auto &action: m_Actions | std::ranges::views::values)
    {
        std::cerr
                << std::endl
                << "  "
                << action.Pattern
                << " - "
                << action.Description
                << std::endl;

        size_t width = 0;
        for (auto &parameter: action.Parameters)
            width = std::max(width, parameter.Pattern.size());

        for (unsigned i = 0; i < action.Parameters.size(); ++i)
            std::cerr
                    << "   + '"
                    << action.Parameters[i].Pattern
                    << "'"
                    << std::setw(width - action.Parameters[i].Pattern.size())
                    << std::left
                    << ""
                    << " - "
                    << action.Parameters[i].Description << std::endl;
    }
}
