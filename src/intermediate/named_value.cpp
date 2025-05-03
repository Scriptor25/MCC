#include <mcc/intermediate.hpp>

mcc::ValuePtr mcc::NamedValue::Create(ResourceLocation location, std::string id)
{
    return std::make_shared<NamedValue>(location, id);
}

mcc::NamedValue::NamedValue(ResourceLocation location, std::string id)
    : Location(std::move(location)),
      ID(std::move(id))
{
}

mcc::Command mcc::NamedValue::GenInline() const
{
    return "data get storage " + Location.String() + " stack[0].var." + ID;
}

mcc::CommandResult mcc::NamedValue::GenResult(const bool stringify) const
{
    return {
        .Type = CommandResultType_Storage,
        .Path = "stack[0].var." + ID,
    };
}
