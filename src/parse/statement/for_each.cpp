#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseForEachStatement()
{
    auto where = Expect(TokenType::Symbol, "foreach").Where;
    Expect(TokenType::Other, "(");
    auto constant = SkipIf(TokenType::Symbol, "const") || (Expect(TokenType::Symbol, "let"), false);
    auto name     = Expect(TokenType::Symbol).Value;
    Expect(TokenType::Other, ":");
    auto iterable = ParseExpression();
    Expect(TokenType::Other, ")");
    auto do_ = ParseStatement();
    return std::make_unique<ForEachStatement>(where, constant, name, std::move(iterable), std::move(do_));
}
