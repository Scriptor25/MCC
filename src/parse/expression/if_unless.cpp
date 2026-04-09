#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseIfUnlessExpression()
{
    auto token = ExpectEnum("if", "unless");
    Expect(TokenType::Other, "(");
    auto condition = ParseExpression();
    Expect(TokenType::Other, ")");
    auto then = ParseExpression();
    Expect(TokenType::Symbol, "else");
    auto else_ = ParseExpression();
    return std::make_unique<IfUnlessExpression>(
            token.Where,
            token.Value == "unless",
            std::move(condition),
            std::move(then),
            std::move(else_));
}
