#include <mcc/parse.hpp>

mcc::StatementPtr mcc::Parser::ParseDefineStatement()
{
    auto where = Expect(TokenType_Symbol, "define").Where;

    auto location = ParseResourceLocation();

    std::vector<std::string> parameters;
    std::vector<ResourceLocation> tags;
    std::vector<ExpressionPtr> expressions;

    Expect(TokenType_Other, "(");
    while (!At(TokenType_Other, ")") && !At(TokenType_EOF))
    {
        parameters.emplace_back(Expect(TokenType_Symbol).Value);

        if (!At(TokenType_Other, ")"))
            Expect(TokenType_Other, ",");
    }
    Expect(TokenType_Other, ")");

    if (At(TokenType_Other, "#"))
    {
        while (!At(TokenType_Other, "{") && !At(TokenType_EOF))
        {
            Expect(TokenType_Other, "#");
            tags.emplace_back(ParseResourceLocation());

            if (!At(TokenType_Other, "{"))
                Expect(TokenType_Other, ",");
        }
    }

    Expect(TokenType_Other, "{");
    while (!At(TokenType_Other, "}") && !At(TokenType_EOF))
        expressions.emplace_back(ParseExpression());
    Expect(TokenType_Other, "}");

    return std::make_unique<DefineStatement>(where, location, parameters, tags, std::move(expressions));
}
