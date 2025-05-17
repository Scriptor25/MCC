#include <mcc/constant.hpp>
#include <mcc/expression.hpp>
#include <mcc/value.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseStringExpression()
{
    auto token = Expect(TokenType_String);
    return std::make_unique<ConstantExpression>(
        token.Where,
        ConstantString::Create(token.Where, token.Value),
        '"' + token.Value + '"');
}
