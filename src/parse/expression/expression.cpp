#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseExpression()
{
    return ParseBinaryExpression(ParseCallExpression(), 0);
}
