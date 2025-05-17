#include <mcc/constant.hpp>

mcc::ConstantPtr mcc::ConstantString::Create(const SourceLocation &where, const std::string &value)
{
    return std::make_shared<ConstantString>(where, value);
}

mcc::ConstantString::ConstantString(const SourceLocation &where, const std::string &value)
    : Constant(where, TypeID_String),
      Value(value)
{
}

mcc::Result mcc::ConstantString::GenerateResult(const bool stringify) const
{
    return {
        .Type = ResultType_Value,
        .Value = '"' + Value + '"',
    };
}
