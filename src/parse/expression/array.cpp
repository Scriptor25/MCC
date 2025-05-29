#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseArrayExpression()
{
    auto where = Expect(TokenType_Other, "[").Where;

    std::vector<ExpressionPtr> elements;

    while (!At(TokenType_Other, "]") && !At(TokenType_EOF))
    {
        elements.emplace_back(ParseExpression());
        if (!At(TokenType_Other, "]"))
            Expect(TokenType_Other, ",");
    }
    Expect(TokenType_Other, "]");

    TypePtr type;
    if (SkipIf(TokenType_Other, ":"))
        type = ParseType();

    return std::make_unique<ArrayExpression>(where, std::move(elements), type);
}
