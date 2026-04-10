#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantNumber::Create(
        const SourceLocation &where,
        Context &context,
        const IntegerT value)
{
    auto self  = std::make_shared<ConstantNumber>(where, context, value);
    self->Self = self;
    return self;
}

mcc::ConstantNumber::ConstantNumber(
        const SourceLocation &where,
        Context &context,
        const IntegerT value)
    : Constant(
              where,
              context.GetNumber()),
      Value(value)
{
}

mcc::Result mcc::ConstantNumber::GenerateResult() const
{
    return {
        .Type    = ResultType_::Value,
        .Value   = std::to_string(Value),
        .NotNull = !!Value,
    };
}
