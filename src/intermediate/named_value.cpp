#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::ValuePtr mcc::NamedValue::Create(ResourceLocation location, std::string id)
{
    return std::make_shared<NamedValue>(std::move(location), std::move(id));
}

mcc::NamedValue::NamedValue(ResourceLocation location, std::string id)
    : Location(std::move(location)),
      ID(std::move(id))
{
}

mcc::CommandT mcc::NamedValue::GenerateInline() const
{
    return std::format("data get storage {} stack[0].var.{}", Location.String(), ID);
}

mcc::Result mcc::NamedValue::GenerateResult(const bool stringify) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = "stack[0].var." + ID,
    };
}
