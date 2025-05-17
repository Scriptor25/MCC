#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseSymbolExpression()
{
    auto token = Expect(TokenType_Symbol);
    return std::make_unique<SymbolExpression>(token.Where, token.Value);
}
