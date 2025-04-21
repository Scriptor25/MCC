#pragma once

#include <map>
#include <string>
#include <vector>
#include <mcc/package.hpp>

namespace mcc
{
    using Command = std::string;

    struct Function
    {
        std::string ID;
        std::vector<Command> Commands;
    };

    struct Tag
    {
        std::string ID;
        bool Required;
    };

    struct Package
    {
        /**
         * namespace:function -> function
         */
        std::map<std::string, std::map<std::string, Function>> Functions;
        /**
         * namespace:tag -> tag...
         */
        std::map<std::string, std::map<std::string, std::vector<Tag>>> Tags;
    };
}
