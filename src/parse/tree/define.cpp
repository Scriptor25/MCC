#include <mcc/expression.hpp>
#include <mcc/lex.hpp>
#include <mcc/parse.hpp>
#include <mcc/resource.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>

mcc::TreeNodePtr mcc::Parser::ParseDefineNode()
{
    auto where       = Expect(TokenType::Symbol, "define").Where;
    auto is_operator = SkipIf(TokenType::Symbol, "operator");

    ResourceLocation location;
    if (is_operator)
        location.Path = { Expect(TokenType::Operator).Value };
    else
        location = ParseResourceLocation();

    ParameterList parameters;
    std::vector<ResourceTag> tags;

    Expect(TokenType::Other, "(");
    while (!At(TokenType::Other, ")") && !At(TokenType::EoF))
    {
        const auto is_constant = SkipIf(TokenType::Symbol, "const");
        const auto is_reference =
                SkipIf(TokenType::Other, "&") || (is_constant && (Expect(TokenType::Other, "&"), true));

        auto name = Expect(TokenType::Symbol).Value;

        Expect(TokenType::Other, ":");

        auto type = ParseType();

        parameters.emplace_back(
                name,
                type,
                is_constant    ? FieldType_::ImmutableReference
                : is_reference ? FieldType_::MutableReference
                               : FieldType_::Value);

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

    return std::make_unique<DefineNode>(
            where,
            is_operator,
            location,
            parameters,
            result_type,
            throws,
            tags,
            std::move(body));
}
