#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantSymbol::Create(std::string value)
{
    return std::make_shared<ConstantSymbol>(std::move(value));
}

mcc::ConstantSymbol::ConstantSymbol(std::string value)
    : Value(std::move(value))
{
}

mcc::Result mcc::ConstantSymbol::GenerateResult(bool stringify, bool use_stack) const
{
    auto value = Value;
    if (stringify)
        value = '"' + value + '"';

    return {
        .Type = ResultType_Value,
        .Value = std::move(value),
    };
}
