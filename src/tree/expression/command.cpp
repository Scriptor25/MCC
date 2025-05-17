#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::CommandExpression::CommandExpression(SourceLocation where, CommandT command)
    : Expression(std::move(where)),
      Command(std::move(command))
{
}

std::ostream &mcc::CommandExpression::Print(std::ostream &stream) const
{
    return stream << "?`" << Command << '`';
}

mcc::ValuePtr mcc::CommandExpression::GenerateValue(Builder &builder) const
{
    return builder.CreateCommand(Command);
}
