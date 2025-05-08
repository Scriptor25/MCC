#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::StatementPtr mcc::Parser::ParseForStatement()
{
    auto where = Expect(TokenType_Symbol, "for").Where;
    Expect(TokenType_Other, "(");
    auto prefix = ParseStatement();
    Expect(TokenType_Other, ",");
    auto condition = ParseExpression();
    Expect(TokenType_Other, ",");
    auto suffix = ParseStatement();
    Expect(TokenType_Other, ")");
    auto do_ = ParseStatement();
    return std::make_unique<ForStatement>(
        std::move(where),
        std::move(prefix),
        std::move(condition),
        std::move(suffix),
        std::move(do_));
}
