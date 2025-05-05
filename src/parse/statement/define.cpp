#include <mcc/parse.hpp>

mcc::StatementPtr mcc::Parser::ParseDefineStatement()
{
    auto where = Expect(TokenType_Symbol, "define").Where;

    auto location = ParseResourceLocation();

    Expect(TokenType_Other, "(");

    std::vector<std::string> parameters;

    while (!SkipIf(TokenType_Other, ")"))
    {
        parameters.emplace_back(Expect(TokenType_Symbol).Value);
        if (!At(TokenType_Other, ")"))
            Expect(TokenType_Other, ",");
    }

    std::vector<ResourceLocation> tags;
    if (At(TokenType_Other, "#"))
    {
        while (!At(TokenType_Other, "{"))
        {
            Expect(TokenType_Other, "#");
            tags.emplace_back(ParseResourceLocation());

            if (!At(TokenType_Other, "{"))
                Expect(TokenType_Other, ",");
        }
    }

    Expect(TokenType_Other, "{");

    std::vector<ExpressionPtr> expressions;

    while (!SkipIf(TokenType_Other, "}"))
    {
        expressions.emplace_back(ParseExpression());
        Expect(TokenType_Other, ";");
    }

    return std::make_unique<DefineStatement>(where, location, parameters, tags, std::move(expressions));
}
