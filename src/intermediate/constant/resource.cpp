#include <utility>
#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantResource::Create(const SourceLocation &where, const ResourceLocation &location)
{
    return std::make_shared<ConstantResource>(where, location);
}

mcc::ConstantResource::ConstantResource(const SourceLocation &where, ResourceLocation location)
    : Constant(where, TypeContext::GetNull()),
      Location(std::move(location))
{
}

mcc::Result mcc::ConstantResource::GenerateResult(const bool stringify) const
{
    auto value = Location.String();

    return {
        .Type = ResultType_Value,
        .Value = std::move(value),
    };
}
