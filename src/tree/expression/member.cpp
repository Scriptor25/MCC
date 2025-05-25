#include <mcc/expression.hpp>
#include <mcc/value.hpp>

mcc::MemberExpression::MemberExpression(const SourceLocation &where, ExpressionPtr object, const std::string &member)
    : Expression(where),
      Object(std::move(object)),
      Member(member)
{
}

std::ostream &mcc::MemberExpression::Print(std::ostream &stream) const
{
    return Object->Print(stream) << '.' << Member;
}

mcc::ValuePtr mcc::MemberExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    const auto object = Object->GenerateValue(builder, frame);
    return MemberReference::Create(Where, object, Member);
}
