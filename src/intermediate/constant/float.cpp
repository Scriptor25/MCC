#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantFloat::Create(const FloatT value)
{
    return std::make_shared<ConstantFloat>(value);
}

mcc::ConstantFloat::ConstantFloat(const FloatT value)
    : Value(value)
{
}

mcc::Command mcc::ConstantFloat::GenInline() const
{
    return std::to_string(Value);
}
