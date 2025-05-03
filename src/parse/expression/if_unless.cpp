#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseIfUnlessExpression(bool unless)
{
    Expect(TokenType_Other, "(");

    auto condition = ParseExpression();

    Expect(TokenType_Other, ")");

    auto then = ParseExpression();

    Expect(TokenType_Symbol, "else");

    auto else_ = ParseExpression();

    return std::make_unique<IfUnlessExpression>(unless, std::move(condition), std::move(then), std::move(else_));
}
