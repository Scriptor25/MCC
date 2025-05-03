#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseExpression()
{
    return ParseBinaryExpression(ParseCallExpression(), ParseCallExpression, 0);
}
