#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseForStatement()
{
    StatementPtr prefix, suffix;
    ExpressionPtr condition;

    auto where = Expect(TokenType_Symbol, "for").Where;
    Expect(TokenType_Other, "(");
    if (!SkipIf(TokenType_Other, ","))
    {
        prefix = ParseStatement();
        Expect(TokenType_Other, ",");
    }
    if (!SkipIf(TokenType_Other, ","))
    {
        condition = ParseExpression();
        Expect(TokenType_Other, ",");
    }
    if (!SkipIf(TokenType_Other, ")"))
    {
        suffix = ParseStatement();
        Expect(TokenType_Other, ")");
    }
    auto do_ = ParseStatement();
    return std::make_unique<ForStatement>(
        where,
        std::move(prefix),
        std::move(condition),
        std::move(suffix),
        std::move(do_));
}
