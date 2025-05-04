#include <mcc/error.hpp>
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

mcc::CommandT mcc::NamedValue::GenerateInline(bool use_stack) const
{
    return "data get storage " + Location.String() + " stack[0].var." + ID;
}

mcc::Result mcc::NamedValue::GenResult(const bool stringify, bool use_stack) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = "stack[0].var." + ID,
    };
}

bool mcc::NamedValue::RequireStack() const
{
    return true;
}
