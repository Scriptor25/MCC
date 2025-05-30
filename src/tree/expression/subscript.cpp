#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
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
    const auto base = Base->GenerateValue(builder, frame);
    const auto index = Index->GenerateValue(builder, frame);

    const auto constant_base = std::dynamic_pointer_cast<ConstantArray>(base);
    const auto constant_index = std::dynamic_pointer_cast<ConstantNumber>(index);

    if (constant_base)
    {
        Assert(!!constant_index, Where, "index must be a constant number");
        return constant_base->Values[constant_index->Value];
    }

    return ElementReference::Create(Where, base, index);
}
