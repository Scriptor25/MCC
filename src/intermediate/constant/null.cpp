#include <mcc/constant.hpp>
#include <mcc/type.hpp>

mcc::ConstantPtr mcc::ConstantNull::Create(const SourceLocation &where, TypeContext &context)
{
    return std::make_shared<ConstantNull>(where, context);
}

mcc::ConstantNull::ConstantNull(const SourceLocation &where, TypeContext &context)
    : Constant(where, context.GetNull())
{
}

mcc::Result mcc::ConstantNull::GenerateResult() const
{
    return {
        .Type = ResultType_Value,
        .NotNull = false,
    };
}
