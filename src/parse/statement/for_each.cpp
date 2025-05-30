#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseForEachStatement()
{
    auto where = Expect(TokenType_Symbol, "foreach").Where;
    Expect(TokenType_Other, "(");
    auto constant = SkipIf(TokenType_Symbol, "const") || (Expect(TokenType_Symbol, "let"), false);
    auto name = Expect(TokenType_Symbol).Value;
    Expect(TokenType_Other, ":");
    auto iterable = ParseExpression();
    Expect(TokenType_Other, ")");
    auto do_ = ParseStatement();
    return std::make_unique<ForEachStatement>(where, constant, name, std::move(iterable), std::move(do_));
}
