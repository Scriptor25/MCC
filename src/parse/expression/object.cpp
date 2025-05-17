#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseObjectExpression()
{
    auto where = Expect(TokenType_Other, "{").Where;

    std::map<std::string, ExpressionPtr> elements;

    while (!SkipIf(TokenType_Other, "}"))
    {
        auto key = At(TokenType_String) ? Skip() : Expect(TokenType_Symbol);

        ExpressionPtr value;
        if (SkipIf(TokenType_Other, ":"))
            value = ParseExpression();
        else
            value = std::make_unique<SymbolExpression>(key.Where, key.Value);

        elements.emplace(std::move(key.Value), std::move(value));

        if (!At(TokenType_Other, "}"))
            Expect(TokenType_Other, ",");
    }

    return std::make_unique<ObjectExpression>(where, std::move(elements));
}
