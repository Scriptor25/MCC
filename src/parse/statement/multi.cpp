#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::StatementPtr mcc::Parser::ParseMultiStatement()
{
    std::vector<StatementPtr> statements;

    auto where = Expect(TokenType_Other, "{").Where;
    while (!At(TokenType_Other, "}") && !At(TokenType_EOF))
        statements.emplace_back(ParseStatement());
    Expect(TokenType_Other, "}");

    return std::make_unique<MultiStatement>(std::move(where), std::move(statements));
}
