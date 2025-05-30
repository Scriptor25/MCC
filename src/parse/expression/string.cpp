#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/value.hpp>

mcc::ExpressionPtr mcc::Parser::ParseStringExpression()
{
    auto token = Expect(TokenType_String);
    return std::make_unique<ConstantExpression>(
        token.Where,
        ConstantString::Create(token.Where, m_Context, token.Value),
        '"' + token.Value + '"');
}
