#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseArrayExpression()
{
    auto where = Expect(TokenType::Other, "[").Where;

    std::vector<ExpressionPtr> elements;

    while (!At(TokenType::Other, "]") && !At(TokenType::EoF))
    {
        elements.push_back(ParseExpression());
        if (!At(TokenType::Other, "]"))
            Expect(TokenType::Other, ",");
    }
    Expect(TokenType::Other, "]");

    TypePtr type;
    if (SkipIf(TokenType::Other, ":"))
        type = ParseType();

    return std::make_unique<ArrayExpression>(where, std::move(elements), type);
}
