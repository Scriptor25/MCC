#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantInteger::Create(const IntegerT value)
{
    return std::make_shared<ConstantInteger>(value);
}

mcc::ConstantInteger::ConstantInteger(const IntegerT value)
    : Value(value)
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
