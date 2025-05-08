#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::StatementPtr mcc::Parser::ParseReturnStatement()
{
    auto where = Expect(TokenType_Symbol, "return").Where;
    auto expression = ParseExpression();
    return std::make_unique<ReturnStatement>(where, std::move(expression));
}
