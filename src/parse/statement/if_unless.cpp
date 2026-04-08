#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseIfUnlessStatement()
{
    auto token = ExpectEnum("if", "unless");
    Expect(TokenType::Other, "(");
    auto condition = ParseExpression();
    Expect(TokenType::Other, ")");

    auto then = ParseStatement();

    StatementPtr else_;
    if (SkipIf(TokenType::Symbol, "else"))
        else_ = ParseStatement();

    return std::make_unique<
            IfUnlessStatement
    >(token.Where, token.Value == "unless", std::move(condition), std::move(then), std::move(else_));
}
