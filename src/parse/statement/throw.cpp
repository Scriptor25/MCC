#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseThrowStatement()
{
    auto where = Expect(TokenType_Symbol, "throw").Where;
    auto value = SkipIf(TokenType_Symbol, "void") ? nullptr : ParseExpression();
    return std::make_unique<ThrowStatement>(where, std::move(value));
}
