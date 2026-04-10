#pragma once

#include <mcc/common.hpp>
#include <mcc/resource.hpp>

#include <vector>

namespace mcc
{
    class Module final
    {
    public:
        static bool CheckFunction(
                const FunctionPtr &function,
                const ResourceLocation &location,
                const ParameterList &parameters);

        explicit Module(Context &context);

        [[nodiscard]] Context &GetContext() const;

        [[nodiscard]] std::vector<FunctionPtr>::const_iterator begin() const;
        [[nodiscard]] std::vector<FunctionPtr>::const_iterator end() const;

        FunctionPtr CreateFunction(
                const SourceLocation &where,
                ResourceLocation location,
                const ParameterList &parameters,
                const TypePtr &result,
                bool throws);

        [[nodiscard]] FunctionPtr GetFunction(
                const ResourceLocation &location,
                const ParameterList &parameters) const;

        ResourceLocation Mangle(const FunctionPtr &function) const;

    private:
        Context &m_Context;

        std::vector<FunctionPtr> m_Functions;
    };
}
