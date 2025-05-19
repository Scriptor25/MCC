#include <mcc/value.hpp>

mcc::ValuePtr mcc::FunctionResult::Create(
    const SourceLocation &where,
    TypeContext &context,
    const TypePtr &type,
    const ResourceLocation &location)
{
    return std::make_shared<FunctionResult>(where, context, type, location);
}

mcc::FunctionResult::FunctionResult(
    const SourceLocation &where,
    TypeContext &context,
    const TypePtr &type,
    const ResourceLocation &location)
    : Value(where, context, type),
      Location(location)
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
