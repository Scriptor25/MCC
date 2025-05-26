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

mcc::Result mcc::ConstantNumber::GenerateResult() const
{
    return {
        .Type = ResultType_Value,
        .Value = std::to_string(Value),
        .NotNull = Value != 0.0,
    };
}
