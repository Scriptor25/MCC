#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseDeleteStatement()
{
    auto where = Expect(TokenType_Symbol, "delete").Where;
    auto value = ParseExpression();
    return std::make_unique<DeleteStatement>(where, std::move(value));
}
