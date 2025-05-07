#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseReturnExpression()
{
    auto where = Expect(TokenType_Symbol, "return").Where;
    auto expression = ParseExpression();
    return std::make_unique<ReturnExpression>(where, std::move(expression));
}
