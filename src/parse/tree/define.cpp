#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>

mcc::TreeNodePtr mcc::Parser::ParseDefineNode()
{
    auto where = Expect(TokenType_Symbol, "define").Where;
    auto location = ParseResourceLocation();

    ParameterList parameters;
    std::vector<ResourceTag> tags;
    std::vector<ExpressionPtr> expressions;

    Expect(TokenType_Other, "(");
    while (!At(TokenType_Other, ")") && !At(TokenType_EOF))
    {
        const auto constant = SkipIf(TokenType_Symbol, "const");
        const auto reference = SkipIf(TokenType_Other, "&") || (constant && (Expect(TokenType_Other, "&"), true));

        auto name = Expect(TokenType_Symbol).Value;

        Expect(TokenType_Other, ":");

        auto type = ParseType();

        parameters.emplace_back(
            name,
            type,
            constant
                ? FieldType_ImmutableReference
                : reference
                ? FieldType_MutableReference
                : FieldType_Value);

        if (!At(TokenType_Other, ")"))
            Expect(TokenType_Other, ",");
    }
    Expect(TokenType_Other, ")");

    auto result_type = SkipIf(TokenType_Other, ":")
                           ? ParseType()
                           : m_Context.GetVoid();

    auto throws = SkipIf(TokenType_Symbol, "throws");

    if (At(TokenType_Other, "#"))
        while (!At(TokenType_Other, "{") && !At(TokenType_EOF))
        {
            Expect(TokenType_Other, "#");
            tags.emplace_back(ParseResourceLocation());

            if (!At(TokenType_Other, "{"))
                Expect(TokenType_Other, ",");
        }

    StatementPtr body;
    if (At(TokenType_Other, "{"))
        body = ParseMultiStatement();

    return std::make_unique<DefineNode>(
        where,
        location,
        parameters,
        result_type,
        throws,
        tags,
        std::move(body));
}
