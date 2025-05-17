#include <mcc/expression.hpp>
#include <mcc/value.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseCommandExpression()
{
    auto where = Expect(TokenType_Other, "?").Where;
    auto command = Expect(TokenType_FormatString).Value;

    return std::make_unique<CommandExpression>(std::move(where), std::move(command));
}
