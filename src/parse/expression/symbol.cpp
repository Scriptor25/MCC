#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseSymbolExpression()
{
    auto token = Expect(TokenType_Symbol);
    if (!SkipIf(TokenType_Other, ":"))
        return std::make_unique<SymbolExpression>(token.Where, token.Value);

    auto path = Expect(TokenType_Symbol).Value;
    while (SkipIf(TokenType_Operator, "/"))
    {
        path += '/';
        path += Expect(TokenType_Symbol).Value;
    }

    return std::make_unique<ResourceExpression>(token.Where, ResourceLocation(token.Value, path));
}
