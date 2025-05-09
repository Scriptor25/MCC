#include <mcc/intermediate.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseIntegerExpression()
{
    auto token = Expect(TokenType_Integer);
    return std::make_unique<ConstantExpression>(
        std::move(token.Where),
        ConstantInteger::Create(token.Integer),
        std::move(token.Value));
}
