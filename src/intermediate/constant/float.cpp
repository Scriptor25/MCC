#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantFloat::Create(const SourceLocation &where, TypeContext &context, const FloatT value)
{
    return std::make_shared<ConstantFloat>(where, context, value);
}

mcc::ConstantFloat::ConstantFloat(const SourceLocation &where, TypeContext &context, const FloatT value)
    : Constant(where, context, context.GetNumber()),
      Value(value)
{
}

mcc::Result mcc::ConstantFloat::GenerateResult(const bool stringify) const
{
    auto value = std::to_string(Value);
    if (stringify)
        value = '"' + value + '"';

    return {
        .Type = ResultType_Value,
        .Value = std::move(value),
    };
}
