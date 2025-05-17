#include <mcc/constant.hpp>

mcc::ConstantPtr mcc::ConstantBoolean::Create(const SourceLocation &where, const bool value)
{
    return std::make_shared<ConstantBoolean>(where, value);
}

mcc::ConstantBoolean::ConstantBoolean(const SourceLocation &where, const bool value)
    : Constant(where),
      Value(value)
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
