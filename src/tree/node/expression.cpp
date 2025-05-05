#include <mcc/tree.hpp>

mcc::ExpressionNode::ExpressionNode(SourceLocation where, ExpressionPtr expression)
    : FormatNode(std::move(where)),
      Expression(std::move(expression))
{
}

std::ostream &mcc::ExpressionNode::Print(std::ostream &stream) const
{
    return Expression->Print(stream << "${") << '}';
}

mcc::ValuePtr mcc::ExpressionNode::Generate(Builder &builder, const bool inline_) const
{
    return Expression->Generate(builder, inline_);
}
