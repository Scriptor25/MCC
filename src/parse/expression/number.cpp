#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseNumberExpression()
{
    auto token = Expect(TokenType::Number);
    return std::make_unique<ConstantExpression>(
            token.Where,
            ConstantNumber::Create(token.Where, m_Context, static_cast<IntegerT>(token.Number)),
            token.Value);
}
