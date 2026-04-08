#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseMultiStatement()
{
    std::vector<StatementPtr> statements;

    auto where = Expect(TokenType::Other, "{").Where;
    while (!At(TokenType::Other, "}") && !At(TokenType::EoF))
        statements.emplace_back(ParseStatement());
    Expect(TokenType::Other, "}");

    return std::make_unique<MultiStatement>(where, std::move(statements));
}
