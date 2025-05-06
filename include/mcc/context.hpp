#pragma once

#include <string>
#include <mcc/command.hpp>
#include <mcc/package.hpp>

namespace mcc
{
    struct Context
    {
        Package &Package;
        const DefinitionsT &Definitions;
        std::string Namespace;
    };
}
