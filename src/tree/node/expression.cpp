#include <mcc/expression.hpp>
#include <mcc/format.hpp>

mcc::ExpressionNode::ExpressionNode(const SourceLocation &where, ExpressionPtr expression)
    : FormatNode(where), Expression(std::move(expression))
{
}

std::ostream &mcc::ExpressionNode::Print(std::ostream &stream) const
{
    return Expression->Print(stream << "${") << '}';
}

mcc::ValuePtr mcc::ExpressionNode::Generate(Builder &builder, const Frame &frame) const
{
    return Expression->GenerateValue(builder, frame);
}
