#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantResource::Create(ResourceLocation location)
{
    return std::make_shared<ConstantResource>(std::move(location));
}

mcc::ConstantResource::ConstantResource(ResourceLocation location)
    : Location(std::move(location))
{
}

mcc::Command mcc::ConstantResource::GenInline() const
{
    return Location.Namespace + ':' + Location.Path;
}
