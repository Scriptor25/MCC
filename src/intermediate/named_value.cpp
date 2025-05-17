#include <mcc/value.hpp>

mcc::ValuePtr mcc::NamedValue::Create(
    const SourceLocation &where,
    const ResourceLocation &location,
    const std::string &name)
{
    return std::make_shared<NamedValue>(where, location, name);
}

mcc::NamedValue::NamedValue(const SourceLocation &where, const ResourceLocation &location, const std::string &name)
    : Value(where),
      Location(location),
      Name(name)
{
}

bool mcc::NamedValue::RequireStack() const
{
    return true;
}

mcc::Result mcc::NamedValue::GenerateResult(const bool stringify) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = "stack[0].var." + Name,
    };
}
