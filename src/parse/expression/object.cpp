#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseObjectExpression()
{
    auto where = Expect(TokenType::Other, "{").Where;

    std::map<std::string, ExpressionPtr> elements;

    while (!SkipIf(TokenType::Other, "}"))
    {
        auto key = At(TokenType::String) ? Skip() : Expect(TokenType::Symbol);

        ExpressionPtr value;
        if (SkipIf(TokenType::Other, ":"))
            value = ParseExpression();
        else
            value = std::make_unique<SymbolExpression>(key.Where, key.Value);

        elements[key.Value] = std::move(value);

        if (!At(TokenType::Other, "}"))
            Expect(TokenType::Other, ",");
    }

    return std::make_unique<ObjectExpression>(where, std::move(elements));
}
