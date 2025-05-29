#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseIfUnlessStatement()
{
    auto token = ExpectEnum("if", "unless");
    Expect(TokenType_Other, "(");
    auto condition = ParseExpression();
    Expect(TokenType_Other, ")");

    auto then = ParseStatement();

    StatementPtr else_;
    if (SkipIf(TokenType_Symbol, "else"))
        else_ = ParseStatement();

    return std::make_unique<IfUnlessStatement>(token.Where, token.Value == "unless", std::move(condition), std::move(then), std::move(else_));
}
