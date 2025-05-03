#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantResource::Create(ResourceLocation location)
{
    return std::make_shared<ConstantResource>(std::move(location));
}

mcc::ConstantResource::ConstantResource(ResourceLocation location)
    : Location(std::move(location))
{
}

mcc::CommandResult mcc::ConstantResource::GenResult(const bool stringify) const
{
    return {
        .Type = CommandResultType_Value,
        .Value = Location.String(),
    };
}
