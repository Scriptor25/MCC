#include <utility>
#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantResource::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const ResourceLocation &location)
{
    return std::make_shared<ConstantResource>(where, type, location);
}

mcc::ConstantResource::ConstantResource(const SourceLocation &where, const TypePtr &type, ResourceLocation location)
    : Constant(where, type),
      Location(std::move(location))
{
}

mcc::Result mcc::ConstantResource::GenerateResult() const
{
    return {
        .Type = ResultType_Value,
        .Value = '"' + Location.String() + '"',
        .NotNull = true,
    };
}

mcc::Result mcc::ConstantResource::GenerateResultUnwrap() const
{
    return {
        .Type = ResultType_Value,
        .Value = Location.String(),
        .NotNull = true,
    };
}
