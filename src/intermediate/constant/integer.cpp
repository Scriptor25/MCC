#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantInteger::Create(const IntegerT value)
{
    return std::make_shared<ConstantInteger>(value);
}

mcc::ConstantInteger::ConstantInteger(const IntegerT value)
    : Value(value)
{
}

mcc::Command mcc::ConstantInteger::GenInline() const
{
    return std::to_string(Value);
}
