#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseMacroExpression()
{
    auto where = Expect(TokenType::Other, "!").Where;
    auto name  = Expect(TokenType::Symbol).Value;
    return std::make_unique<MacroExpression>(where, name);
}
