#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

namespace mcc
{
    struct ActionParameter
    {
        bool IsFlag;
        std::string Pattern;
        std::string Description;
    };

    struct Action
    {
        unsigned ID;
        std::string Pattern;
        std::string Description;
        std::vector<ActionParameter> Parameters;
    };

    class Actions
    {
    public:
        explicit Actions(std::vector<Action> actions);

        void operator()(int argc, const char **argv);

    private:
        std::map<std::string, Action> m_Actions;

        std::string m_Executable;
        Action *m_Action = nullptr;
        std::map<unsigned, std::string> m_Strings;
        std::set<unsigned> m_Flags;
    };
}
