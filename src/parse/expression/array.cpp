#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseArrayExpression()
{
    Expect(TokenType_Other, "[");

    std::vector<ExpressionPtr> elements;

    while (!SkipIf(TokenType_Other, "]"))
    {
        elements.emplace_back(ParseExpression());
        if (!At(TokenType_Other, "]"))
            Expect(TokenType_Other, ",");
    }

    return std::make_unique<ArrayExpression>(std::move(elements));
}
