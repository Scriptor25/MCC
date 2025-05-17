#include <mcc/builder.hpp>
#include <mcc/instruction.hpp>
#include <mcc/tree.hpp>
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

bool mcc::CommandExpression::IsConstant() const
{
    return false;
}

mcc::ValuePtr mcc::CommandExpression::GenerateValue(Builder &builder) const
{
    return builder.CreateCommand(Command);
}
