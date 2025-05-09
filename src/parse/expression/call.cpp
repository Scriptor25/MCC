#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseCallExpression()
{
    auto callee = ParsePrimaryExpression();

    if (!At(TokenType_Other, "("))
        return callee;

    auto where = Skip().Where;

    std::vector<ExpressionPtr> arguments;
    while (!At(TokenType_Other, ")") && !At(TokenType_EOF))
    {
        arguments.emplace_back(ParseExpression());

        if (!At(TokenType_Other, ")"))
            Expect(TokenType_Other, ",");
    }
    Expect(TokenType_Other, ")");

    return std::make_unique<CallExpression>(std::move(where), std::move(callee), std::move(arguments));
}
