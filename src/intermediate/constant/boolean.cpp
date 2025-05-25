#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantBoolean::Create(const SourceLocation &where, TypeContext &context, const bool value)
{
    return std::make_shared<ConstantBoolean>(where, context, value);
}

mcc::ConstantBoolean::ConstantBoolean(const SourceLocation &where, TypeContext &context, const bool value)
    : Constant(where, context.GetBoolean()),
      Value(value)
{
}

mcc::Result mcc::ConstantBoolean::GenerateResult(const bool stringify) const
{
    std::string value(Value ? "true" : "false");
    if (stringify)
        value = '"' + value + '"';

    return {
        .Type = ResultType_Value,
        .Value = std::move(value),
        .NotNull = Value,
    };
}
