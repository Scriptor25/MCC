#include <mcc/intermediate.hpp>

mcc::ConstantPtr mcc::ConstantBoolean::Create(const bool value)
{
    return std::make_shared<ConstantBoolean>(value);
}

mcc::ConstantBoolean::ConstantBoolean(const bool value)
    : Value(value)
{
}

mcc::Result mcc::ConstantBoolean::GenerateResult(const bool stringify) const
{
    std::string value(Value ? "true" : "false");
    if (stringify)
        value = '"' + value + '"';

    return {
        .Type = ResultType_Value,
        .Value = std::move(value),
    };
}
