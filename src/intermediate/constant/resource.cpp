#include <utility>
#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantResource::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location)
{
    return std::make_shared<ConstantResource>(where, context, location);
}

mcc::ConstantResource::ConstantResource(const SourceLocation &where, TypeContext &context, ResourceLocation location)
    : Constant(where, context.GetNull()),
      Location(std::move(location))
{
}

mcc::Result mcc::ConstantResource::GenerateResult() const
{
    return {
        .Type = ResultType_Value,
        .Value = Location.String(),
        .NotNull = true,
    };
}
