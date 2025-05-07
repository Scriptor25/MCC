#include <mcc/intermediate.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseIntegerExpression(const bool negative)
{
    auto token = Expect(TokenType_Integer);
    return std::make_unique<ConstantExpression>(
        token.Where,
        ConstantInteger::Create(negative ? -token.Integer : token.Integer),
        token.Value);
}
