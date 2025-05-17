#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

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

    auto body = ParseMultiStatement();

    return std::make_unique<DefineStatement>(
        std::move(where),
        std::move(location),
        std::move(parameters),
        std::move(tags),
        std::move(body));
}
