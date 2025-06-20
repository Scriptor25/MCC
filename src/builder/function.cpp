#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/value.hpp>

mcc::FunctionPtr mcc::Builder::CreateFunction(
    const SourceLocation &where,
    ResourceLocation location,
    const ParameterList &parameters,
    const TypePtr &result_type,
    const bool throws)
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    auto &function = m_Functions[location.Namespace][location.Path];
    Assert(!function, where, "already defined function {}", location);
    return function = Function::Create(where, m_Context, location, parameters, result_type, throws);
}

bool mcc::Builder::HasFunction(ResourceLocation location) const
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    return m_Functions.contains(location.Namespace) && m_Functions.at(location.Namespace).contains(location.Path);
}

mcc::FunctionPtr mcc::Builder::GetFunction(const SourceLocation &where, ResourceLocation location) const
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    Assert(m_Functions.contains(location.Namespace), where, "undefined namespace {}", location.Namespace);
    Assert(m_Functions.at(location.Namespace).contains(location.Path), where, "undefined function {}", location);

    return m_Functions.at(location.Namespace).at(location.Path);
}
