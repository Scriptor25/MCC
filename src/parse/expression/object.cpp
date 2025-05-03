#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseObjectExpression()
{
    Expect(TokenType_Other, "{");

    std::map<std::string, ExpressionPtr> elements;

    while (!SkipIf(TokenType_Other, "}"))
    {
        auto key = Expect(TokenType_Symbol).Value;

        Expect(TokenType_Other, ":");

        auto value = ParseExpression();

        elements.emplace(std::move(key), std::move(value));

        if (!At(TokenType_Other, "}"))
            Expect(TokenType_Other, ",");
    }

    return std::make_unique<ObjectExpression>(std::move(elements));
}
