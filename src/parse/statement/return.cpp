#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseReturnStatement()
{
    auto where = Expect(TokenType_Symbol, "return").Where;
    auto value = SkipIf(TokenType_Symbol, "void") ? nullptr : ParseExpression();
    return std::make_unique<ReturnStatement>(where, std::move(value));
}
