#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantNumber::Create(const SourceLocation &where, TypeContext &context, const IntegerT value)
{
    return std::make_shared<ConstantNumber>(where, context, value);
}

mcc::ConstantNumber::ConstantNumber(const SourceLocation &where, TypeContext &context, const IntegerT value)
    : Constant(where, context.GetNumber()),
      Value(value)
{
}

mcc::Result mcc::ConstantNumber::GenerateResult(const bool stringify) const
{
    auto value = std::to_string(Value);
    if (stringify)
        value = '"' + value + '"';

    return {
        .Type = ResultType_Value,
        .Value = std::move(value),
        .NotNull = Value != 0.0,
    };
}
