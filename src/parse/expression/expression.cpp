#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseExpression()
{
    return ParseBinaryExpression(ParseOperandExpression(), 0);
}
