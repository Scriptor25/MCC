#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::CommandExpression::CommandExpression(SourceLocation where, CommandT command)
    : Expression(std::move(where)),
      Command(std::move(command))
{
}

std::ostream &mcc::CommandExpression::Print(std::ostream &stream) const
{
    return stream << "?`" << Command << '`';
}

mcc::ValuePtr mcc::CommandExpression::Generate(Builder &builder, const bool inline_) const
{
    return builder.CreateCommand(Command, inline_);
}
