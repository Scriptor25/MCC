#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantFloat::Create(const FloatT value)
{
    return std::make_shared<ConstantFloat>(value);
}

mcc::ConstantFloat::ConstantFloat(const FloatT value)
    : Value(value)
{
}

mcc::CommandResult mcc::ConstantFloat::GenResult(const bool stringify) const
{
    auto value = std::to_string(Value);
    if (stringify)
        value = '"' + value + '"';

    return {
        .Type = CommandResultType_Value,
        .Value = value,
    };
}
