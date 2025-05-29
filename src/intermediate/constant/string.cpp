#include <mcc/constant.hpp>
#include <mcc/type.hpp>
#include <utility>

mcc::ConstantPtr mcc::ConstantString::Create(const SourceLocation &where, TypeContext &context, const std::string &value)
{
    return std::make_shared<ConstantString>(where, context, value);
}

mcc::ConstantString::ConstantString(const SourceLocation &where, TypeContext &context, std::string value)
    : Constant(where, context.GetString()), Value(std::move(value))
{
}

mcc::Result mcc::ConstantString::GenerateResult() const
{
    return {
        .Type    = ResultType_Value,
        .Value   = '"' + Value + '"',
        .NotNull = true,
    };
}

mcc::Result mcc::ConstantString::GenerateResultUnwrap() const
{
    return {
        .Type    = ResultType_Value,
        .Value   = Value,
        .NotNull = true,
    };
}
