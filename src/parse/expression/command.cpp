#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/value.hpp>

mcc::ExpressionPtr mcc::Parser::ParseCommandExpression()
{
    auto where = Expect(TokenType::Other, "?").Where;
    TypePtr type;
    if (SkipIf(TokenType::Other, ":"))
        type = ParseType();
    auto command = Expect(TokenType::FormatString).Value;

    return std::make_unique<CommandExpression>(where, type, command);
}
