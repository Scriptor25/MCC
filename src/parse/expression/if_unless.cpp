#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseIfUnlessExpression()
{
    auto token = ExpectEnum("if", "unless");
    auto unless = token.Value == "unless";

    Expect(TokenType_Other, "(");

    auto condition = ParseExpression();

    Expect(TokenType_Other, ")");

    auto then = ParseExpression();

    Expect(TokenType_Symbol, "else");

    auto else_ = ParseExpression();

    return std::make_unique<IfUnlessExpression>(
        token.Where,
        unless,
        std::move(condition),
        std::move(then),
        std::move(else_));
}
