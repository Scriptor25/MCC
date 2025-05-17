#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/value.hpp>

mcc::ExpressionPtr mcc::Parser::ParseIntegerExpression()
{
    auto token = Expect(TokenType_Integer);
    return std::make_unique<ConstantExpression>(
        token.Where,
        ConstantInteger::Create(token.Where, token.Integer),
        token.Value);
}
