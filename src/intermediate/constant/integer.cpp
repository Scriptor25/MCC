#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantInteger::Create(const SourceLocation &where, const IntegerT value)
{
    return std::make_shared<ConstantInteger>(where, value);
}

mcc::ConstantInteger::ConstantInteger(const SourceLocation &where, const IntegerT value)
    : Constant(where, TypeContext::GetNumber()),
      Value(value)
{
}

mcc::Result mcc::ConstantInteger::GenerateResult(const bool stringify) const
{
    auto value = std::to_string(Value);
    if (stringify)
        value = '"' + value + '"';

    return {
        .Type = ResultType_Value,
        .Value = std::move(value),
    };
}
