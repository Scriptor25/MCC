#pragma once

#include <string>
#include <mcc/package.hpp>

namespace mcc
{
    struct Context
    {
        Package &Package;
        std::string Namespace;
    };
}
