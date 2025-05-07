#include <mcc/intermediate.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseFloatExpression(const bool negative)
{
    if (At(TokenType_Integer))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            token.Where,
            ConstantFloat::Create(negative ? -token.Integer : token.Integer),
            token.Value);
    }

    auto token = Expect(TokenType_Float);
    return std::make_unique<ConstantExpression>(
        token.Where,
        ConstantFloat::Create(negative ? -token.Float : token.Float),
        token.Value);
}
