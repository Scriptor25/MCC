#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::SubscriptExpression::SubscriptExpression(SourceLocation where, ExpressionPtr array, ExpressionPtr index)
    : Expression(std::move(where)),
      Array(std::move(array)),
      Index(std::move(index))
{
}

std::ostream &mcc::SubscriptExpression::Print(std::ostream &stream) const
{
    return Index->Print(Array->Print(stream) << '[') << ']';
}

mcc::ValuePtr mcc::SubscriptExpression::GenerateValue(Builder &builder) const
{
    const auto array = Array->GenerateValue(builder);
    const auto index = Index->GenerateValue(builder);

    const auto constant_index = std::dynamic_pointer_cast<ConstantInteger>(index);
    Assert(!!constant_index, Where, "index must be constant integer");

    return builder.CreateExtract(array, constant_index->Value);
}
