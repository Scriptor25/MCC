#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::Builder::CreateVariable(
    const SourceLocation &where,
    const TypePtr &type,
    const std::string &name,
    const bool is_mutable,
    const ValuePtr &initializer)
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!m_Variables.empty(), where, "variables must not be empty");
    Assert(!!type, where, "type must not be null");
    Assert(!name.empty(), where, "name must not be empty");

    auto &variable = m_Variables.back()[name];
    Assert(!variable, where, "already defined variable {}", name);

    variable = Allocate(where, type, is_mutable);
    if (initializer)
        (void) CreateStore(where, variable, initializer, true);
    return variable;
}

mcc::ValuePtr mcc::Builder::InsertVariable(const SourceLocation &where, const std::string &name, const ValuePtr &value)
{
    Assert(!!m_InsertBlock, where, "insert block must not be null");
    Assert(!m_Variables.empty(), where, "variables must not be empty");
    Assert(!name.empty(), where, "name must not be empty");
    Assert(!!value, where, "value must not be null");

    auto &variables = m_Variables.back();
    auto &variable = variables[name];
    Assert(!variable, where, "already defined variable {}", name);

    return variable = value;
}

bool mcc::Builder::HasVariable(const std::string &name) const
{
    for (const auto &variables : std::ranges::reverse_view(m_Variables))
        if (variables.contains(name))
            return true;

    const ResourceLocation location{ { name } };

    return HasGlobal(location) || HasFunction(location);
}

mcc::ValuePtr mcc::Builder::GetVariable(const SourceLocation &where, const std::string &name) const
{
    Assert(!m_Variables.empty(), where, "variables must not be empty");

    for (const auto &variables : std::ranges::reverse_view(m_Variables))
        if (variables.contains(name))
            return variables.at(name);

    const ResourceLocation location{ { name } };

    if (HasGlobal(location))
        return GetGlobal(where, location);

    if (HasFunction(location))
        return GetFunction(where, location);

    Error(where, "undefined variable {}", name);
}
