#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::SubscriptExpression::SubscriptExpression(const SourceLocation &where, ExpressionPtr array, ExpressionPtr index)
    : Expression(where),
      Array(std::move(array)),
      Index(std::move(index))
{
}

std::ostream &mcc::SubscriptExpression::Print(std::ostream &stream) const
{
    return Index->Print(Array->Print(stream) << '[') << ']';
}

mcc::ValuePtr mcc::SubscriptExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    const auto array = Array->GenerateValue(builder, frame);
    const auto index = Index->GenerateValue(builder, frame);

    const auto constant_index = std::dynamic_pointer_cast<ConstantInteger>(index);
    Assert(!!constant_index, Index->Where, "index must be constant integer");

    return builder.CreateExtract(Where, array, constant_index->Value);
}
