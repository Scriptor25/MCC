#include <mcc/expression.hpp>
#include <mcc/format.hpp>

mcc::ExpressionNode::ExpressionNode(SourceLocation where, ExpressionPtr expression)
    : FormatNode(std::move(where)),
      Expression(std::move(expression))
{
}

std::ostream &mcc::ExpressionNode::Print(std::ostream &stream) const
{
    return Expression->Print(stream << "${") << '}';
}

mcc::ValuePtr mcc::ExpressionNode::Generate(Builder &builder, const BlockPtr landing_pad) const
{
    return Expression->GenerateValue(builder, landing_pad);
}
