#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantString::Create(std::string value)
{
    return std::make_shared<ConstantString>(std::move(value));
}

mcc::ConstantString::ConstantString(std::string value)
    : Value(std::move(value))
{
}

mcc::Command mcc::ConstantString::GenInline() const
{
    return '"' + Value + '"';
}
