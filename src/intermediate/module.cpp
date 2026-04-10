#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/module.hpp>

bool mcc::Module::CheckFunction(
        const FunctionPtr &function,
        const ResourceLocation &location,
        const ParameterList &parameters)
{
    if (location != function->Location)
        return false;

    if (parameters.size() != function->Parameters.size())
        return false;

    for (unsigned i = 0; i < parameters.size(); ++i)
    {
        if (parameters[i].FieldType != function->Parameters[i].FieldType)
            return false;
        if (parameters[i].Type != function->Parameters[i].Type)
            return false;
    }

    return true;
}

mcc::Module::Module(Context &context)
    : m_Context(context)
{
}

mcc::Context &mcc::Module::GetContext() const
{
    return m_Context;
}

std::vector<mcc::FunctionPtr>::const_iterator mcc::Module::begin() const
{
    return m_Functions.begin();
}

std::vector<mcc::FunctionPtr>::const_iterator mcc::Module::end() const
{
    return m_Functions.end();
}

mcc::FunctionPtr mcc::Module::CreateFunction(
        const SourceLocation &where,
        ResourceLocation location,
        const ParameterList &parameters,
        const TypePtr &result,
        const bool throws)
{
    for (auto &function : m_Functions)
        if (CheckFunction(function, location, parameters))
            Error(where, "function {} is already defined with parameters {}", location, parameters);

    auto function = Function::Create(where, *this, location, parameters, result, throws);
    m_Functions.push_back(function);
    return function;
}

mcc::FunctionPtr mcc::Module::GetFunction(
        const ResourceLocation &location,
        const ParameterList &parameters) const
{
    for (auto &function : m_Functions)
        if (CheckFunction(function, location, parameters))
            return function;
    return {};
}

mcc::ResourceLocation mcc::Module::Mangle(const FunctionPtr &function) const
{
    auto count = 0u;
    for (auto &fn : m_Functions)
    {
        if (fn->Location != function->Location)
            continue;

        if (fn != function)
        {
            ++count;
            continue;
        }

        if (!count)
            return function->Location;

        auto location = function->Location;
        auto &segment = location.Path.back();
        segment += std::to_string(count);

        return location;
    }

    return function->Location;
}
