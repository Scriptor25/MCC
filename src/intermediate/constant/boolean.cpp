#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantBoolean::Create(const bool value)
{
    return std::make_shared<ConstantBoolean>(value);
}

mcc::ConstantBoolean::ConstantBoolean(const bool value)
    : Value(value)
{
}

mcc::Command mcc::ConstantBoolean::GenInline() const
{
    return Value ? "true" : "false";
}
