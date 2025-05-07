#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseObjectExpression()
{
    auto where = Expect(TokenType_Other, "{").Where;

    std::map<std::string, ExpressionPtr> elements;

    while (!SkipIf(TokenType_Other, "}"))
    {
        auto key = At(TokenType_String) ? Skip().Value : Expect(TokenType_Symbol).Value;

        Expect(TokenType_Other, ":");

        auto value = ParseExpression();

        elements.emplace(std::move(key), std::move(value));

        if (!At(TokenType_Other, "}"))
            Expect(TokenType_Other, ",");
    }

    return std::make_unique<ObjectExpression>(where, std::move(elements));
}
