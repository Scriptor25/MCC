#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseDefineStatement()
{
    auto where = Expect(TokenType_Symbol, "define").Where;

    auto location = ParseResourceLocation();

    ParameterList parameters;
    std::vector<ResourceLocation> tags;
    std::vector<ExpressionPtr> expressions;

    Expect(TokenType_Other, "(");
    while (!At(TokenType_Other, ")") && !At(TokenType_EOF))
    {
        auto name = Expect(TokenType_Symbol).Value;
        Expect(TokenType_Other, ":");
        auto type = ParseType();

        parameters.emplace_back(name, type);

        if (!At(TokenType_Other, ")"))
            Expect(TokenType_Other, ",");
    }
    Expect(TokenType_Other, ")");

    TypePtr type;
    if (SkipIf(TokenType_Other, ":"))
        type = ParseType();

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

    auto body = ParseMultiStatement();

    return std::make_unique<DefineStatement>(where, location, parameters, type, tags, std::move(body));
}
