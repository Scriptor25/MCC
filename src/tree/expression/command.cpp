#include <utility>
#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::CommandExpression::CommandExpression(const SourceLocation &where, TypePtr type, CommandT command)
    : Expression(where),
      Type(std::move(type)),
      Command(std::move(command))
{
}

std::ostream &mcc::CommandExpression::Print(std::ostream &stream) const
{
    stream << '?';
    if (Type)
        stream << ':' << Type;
    return stream << "`" << Command << '`';
}

mcc::ValuePtr mcc::CommandExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    return builder.CreateCommand(Where, Type ? Type : builder.GetContext().GetVoid(), Command);
}
