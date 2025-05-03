#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseReturnExpression()
{
    Expect(TokenType_Symbol, "return");
    auto expression = ParseExpression();
    return std::make_unique<ReturnExpression>(std::move(expression));
}
