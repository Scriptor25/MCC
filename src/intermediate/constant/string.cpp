#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantString::Create(
    const SourceLocation &where,
    TypeContext &context,
    const std::string &value)
{
    return std::make_shared<ConstantString>(where, context, value);
}

mcc::ConstantString::ConstantString(const SourceLocation &where, TypeContext &context, const std::string &value)
    : Constant(where, context, context.GetString()),
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
