#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseMacroExpression()
{
    auto where = Expect(TokenType_Other, "!").Where;
    auto name = Expect(TokenType_Symbol).Value;
    return std::make_unique<MacroExpression>(where, name);
}
