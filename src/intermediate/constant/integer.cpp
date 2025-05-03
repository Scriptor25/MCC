#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantInteger::Create(const IntegerT value)
{
    return std::make_shared<ConstantInteger>(value);
}

mcc::ConstantInteger::ConstantInteger(const IntegerT value)
    : Value(value)
{
}

mcc::CommandResult mcc::ConstantInteger::GenResult(const bool stringify) const
{
    auto value = std::to_string(Value);
    if (stringify)
        value = '"' + value + '"';

    return {
        .Type = CommandResultType_Value,
        .Value = value,
    };
}
