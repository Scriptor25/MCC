#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/value.hpp>

mcc::ExpressionPtr mcc::Parser::ParseFloatExpression()
{
    if (At(TokenType_Integer))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            token.Where,
            ConstantNumber::Create(token.Where, m_Context, token.Integer),
            token.Value);
    }

    auto token = Expect(TokenType_Float);
    return std::make_unique<ConstantExpression>(
        token.Where,
        ConstantNumber::Create(token.Where, m_Context, token.Float),
        token.Value);
}
