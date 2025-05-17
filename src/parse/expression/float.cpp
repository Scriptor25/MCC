#include <mcc/constant.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>
#include <mcc/value.hpp>

mcc::ExpressionPtr mcc::Parser::ParseFloatExpression()
{
    if (At(TokenType_Integer))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            std::move(token.Where),
            ConstantFloat::Create(token.Integer),
            std::move(token.Value));
    }

    auto token = Expect(TokenType_Float);
    return std::make_unique<ConstantExpression>(
        std::move(token.Where),
        ConstantFloat::Create(token.Float),
        std::move(token.Value));
}
