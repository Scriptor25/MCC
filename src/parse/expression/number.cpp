#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/value.hpp>

mcc::ExpressionPtr mcc::Parser::ParseNumberExpression()
{
    auto token = Expect(TokenType_Number);
    return std::make_unique<ConstantExpression>(
        token.Where,
        ConstantNumber::Create(token.Where, m_Context, token.Number),
        token.Value);
}
