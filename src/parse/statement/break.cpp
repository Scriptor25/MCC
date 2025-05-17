#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseBreakStatement()
{
    auto where = Expect(TokenType_Symbol, "break").Where;
    return std::make_unique<BreakStatement>(where);
}
