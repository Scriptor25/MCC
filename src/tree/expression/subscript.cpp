#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::SubscriptExpression::SubscriptExpression(const SourceLocation &where, ExpressionPtr base, ExpressionPtr index)
    : Expression(where),
      Base(std::move(base)),
      Index(std::move(index))
{
}

std::ostream &mcc::SubscriptExpression::Print(std::ostream &stream) const
{
    return Index->Print(Base->Print(stream) << '[') << ']';
}

mcc::ValuePtr mcc::SubscriptExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    auto base = Base->GenerateValue(builder, frame);
    const auto index = Index->GenerateValue(builder, frame);

    Assert(
        base->Type->IsArray() || base->Type->IsTuple(),
        Base->Where,
        "base must be of type array or tuple, but is {}",
        base->Type);
    Assert(index->Type->IsNumber(), Index->Where, "index must be of type number, but is {}", index->Type);

    if (const auto constant_base = std::dynamic_pointer_cast<ConstantArray>(base))
    {
        if (const auto constant_index = std::dynamic_pointer_cast<ConstantNumber>(index))
            return constant_base->Values[constant_index->Value];

        base = builder.Allocate(Base->Where, base->Type, false);
        (void) builder.CreateStore(Base->Where, base, constant_base, true);
    }
    else if (const auto argument_base = std::dynamic_pointer_cast<ArgumentValue>(base))
    {
        base = builder.Allocate(Base->Where, base->Type, false);
        (void) builder.CreateStore(Base->Where, base, argument_base, true);
    }

    return ElementReference::Create(Where, base, index);
}
