#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantResource::Create(ResourceLocation location)
{
    return std::make_shared<ConstantResource>(std::move(location));
}

mcc::ConstantResource::ConstantResource(ResourceLocation location)
    : Location(std::move(location))
{
}

mcc::Result mcc::ConstantResource::GenerateResult(const bool stringify, bool use_stack) const
{
    return {
        .Type = ResultType_Value,
        .Value = Location.String(),
    };
}
