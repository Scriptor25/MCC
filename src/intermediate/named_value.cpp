#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::ValuePtr mcc::NamedValue::Create(ResourceLocation location, std::string name)
{
    return std::make_shared<NamedValue>(std::move(location), std::move(name));
}

mcc::NamedValue::NamedValue(ResourceLocation location, std::string name)
    : Location(std::move(location)),
      Name(std::move(name))
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
