#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseContinueStatement()
{
    auto where = Expect(TokenType::Symbol, "continue").Where;
    return std::make_unique<ContinueStatement>(where);
}
