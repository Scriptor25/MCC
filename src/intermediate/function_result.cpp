#include <mcc/value.hpp>

mcc::ValuePtr mcc::FunctionResult::Create(ResourceLocation location)
{
    return std::make_shared<FunctionResult>(std::move(location));
}

mcc::FunctionResult::FunctionResult(ResourceLocation location)
    : Location(std::move(location))
{
}

bool mcc::FunctionResult::RequireStack() const
{
    return false;
}

mcc::Result mcc::FunctionResult::GenerateResult(bool stringify) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = "result",
    };
}
