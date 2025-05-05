#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseArrayExpression()
{
    auto where = Expect(TokenType_Other, "[").Where;

    std::vector<ExpressionPtr> elements;

    while (!SkipIf(TokenType_Other, "]"))
    {
        elements.emplace_back(ParseExpression());
        if (!At(TokenType_Other, "]"))
            Expect(TokenType_Other, ",");
    }

    return std::make_unique<ArrayExpression>(where, std::move(elements));
}
