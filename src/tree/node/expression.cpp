#include <mcc/format.hpp>
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

bool mcc::ExpressionNode::IsConstant() const
{
    return Expression->IsConstant();
}

mcc::ValuePtr mcc::ExpressionNode::Generate(Builder &builder) const
{
    return Expression->GenerateValue(builder);
}
