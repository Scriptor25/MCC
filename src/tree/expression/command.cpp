#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::CommandExpression::CommandExpression(const SourceLocation &where, const TypePtr &type, const CommandT &command)
    : Expression(where),
      Type(type),
      Command(command)
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
    return builder.CreateCommand(Where, Type ? Type : TypeContext::GetVoid(), Command);
}
