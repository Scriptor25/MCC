#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::CommandExpression::CommandExpression(const SourceLocation &where, const CommandT &command)
    : Expression(where),
      Command(command)
{
}

std::ostream &mcc::CommandExpression::Print(std::ostream &stream) const
{
    return stream << "?`" << Command << '`';
}

mcc::ValuePtr mcc::CommandExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    return builder.CreateCommand(Where, Command);
}
