#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseForStatement()
{
    StatementPtr prefix, suffix;
    ExpressionPtr condition;

    auto where = Expect(TokenType::Symbol, "for").Where;
    Expect(TokenType::Other, "(");
    if (!SkipIf(TokenType::Other, ","))
    {
        prefix = ParseStatement();
        Expect(TokenType::Other, ",");
    }
    if (!SkipIf(TokenType::Other, ","))
    {
        condition = ParseExpression();
        Expect(TokenType::Other, ",");
    }
    if (!SkipIf(TokenType::Other, ")"))
    {
        suffix = ParseStatement();
        Expect(TokenType::Other, ")");
    }
    auto do_ = ParseStatement();
    return std::make_unique<ForStatement>(
            where,
            std::move(prefix),
            std::move(condition),
            std::move(suffix),
            std::move(do_));
}
