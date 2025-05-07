#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseSymbolExpression(bool placeholder)
{
    auto token = Expect(TokenType_Symbol);
    return std::make_unique<SymbolExpression>(token.Where, placeholder, token.Value);
}
