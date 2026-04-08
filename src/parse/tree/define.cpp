#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>

mcc::TreeNodePtr mcc::Parser::ParseDefineNode()
{
    auto where    = Expect(TokenType::Symbol, "define").Where;
    auto location = ParseResourceLocation();

    ParameterList parameters;
    std::vector<ResourceTag> tags;
    std::vector<ExpressionPtr> expressions;

    Expect(TokenType::Other, "(");
    while (!At(TokenType::Other, ")") && !At(TokenType::EoF))
    {
        const auto constant  = SkipIf(TokenType::Symbol, "const");
        const auto reference = SkipIf(TokenType::Other, "&") || (constant && (Expect(TokenType::Other, "&"), true));

        auto name = Expect(TokenType::Symbol).Value;

        Expect(TokenType::Other, ":");

        auto type = ParseType();

        parameters.emplace_back(
                name,
                type,
                constant    ? FieldType_ImmutableReference
                : reference ? FieldType_MutableReference
                            : FieldType_Value);

        if (!At(TokenType::Other, ")"))
            Expect(TokenType::Other, ",");
    }
    Expect(TokenType::Other, ")");

    auto result_type = SkipIf(TokenType::Other, ":") ? ParseType() : m_Context.GetVoid();

    auto throws = SkipIf(TokenType::Symbol, "throws");

    if (At(TokenType::Other, "#"))
        while (!At(TokenType::Other, "{") && !At(TokenType::EoF))
        {
            Expect(TokenType::Other, "#");
            tags.emplace_back(ParseResourceLocation());

            if (!At(TokenType::Other, "{"))
                Expect(TokenType::Other, ",");
        }

    StatementPtr body;
    if (At(TokenType::Other, "{"))
        body = ParseMultiStatement();

    return std::make_unique<DefineNode>(where, location, parameters, result_type, throws, tags, std::move(body));
}
