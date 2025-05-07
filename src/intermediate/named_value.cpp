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

mcc::CommandT mcc::NamedValue::GenerateInline(const bool use_stack) const
{
    Assert(use_stack, "named value requires stack usage");

    return "data get storage " + Location.String() + " stack[0].var." + ID;
}

mcc::Result mcc::NamedValue::GenerateResult(const bool stringify, const bool use_stack) const
{
    Assert(use_stack, "named value requires stack usage");

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
