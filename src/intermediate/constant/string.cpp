#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantString::Create(
        const SourceLocation &where,
        Context &context,
        const std::string &value)
{
    auto self  = std::make_shared<ConstantString>(where, context, value);
    self->Self = self;
    return self;
}

mcc::ConstantString::ConstantString(
        const SourceLocation &where,
        Context &context,
        std::string value)
    : Constant(
              where,
              context.GetString()),
      Value(std::move(value))
{
}

mcc::Result mcc::ConstantString::GenerateResult() const
{
    return {
        .Type    = ResultType_::Value,
        .Value   = '"' + Value + '"',
        .NotNull = true,
    };
}

mcc::Result mcc::ConstantString::GenerateResultUnwrap() const
{
    return {
        .Type    = ResultType_::Value,
        .Value   = Value,
        .NotNull = true,
    };
}
