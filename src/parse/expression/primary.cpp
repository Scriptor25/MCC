#include <format>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParsePrimaryExpression()
{
    if (AtEnum("if", "unless"))
        return ParseIfUnlessExpression();

    if (At(TokenType_Symbol, "return"))
        return ParseReturnExpression();

    if (AtEnum("true", "false"))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            token.Where,
            ConstantBoolean::Create(token.Value == "true"),
            std::move(token.Value));
    }

    if (At(TokenType_Integer))
        return ParseIntegerExpression();

    if (At(TokenType_Float))
        return ParseFloatExpression();

    if (At(TokenType_Range))
        return ParseRangeExpression();

    if (At(TokenType_String))
        return ParseStringExpression();

    if (At(TokenType_FormatString))
        return ParseFormatExpression();

    if (At(TokenType_Target))
        return ParseTargetExpression(false);

    if (At(TokenType_TargetAttributes))
        return ParseTargetExpression(true);

    if (At(TokenType_Symbol))
        return ParseSymbolExpression();

    if (SkipIf(TokenType_Other, "$"))
        return ParseResourceExpression();

    if (At(TokenType_Other, "?"))
        return ParseCommandExpression();

    if (At(TokenType_Other, "["))
        return ParseArrayExpression();

    if (At(TokenType_Other, "{"))
        return ParseObjectExpression();

    if (SkipIf(TokenType_Operator, "-"))
    {
        if (At(TokenType_Integer))
            return ParseIntegerExpression(true);
        if (At(TokenType_Float))
            return ParseFloatExpression(true);
        return ParseRangeExpression(true);
    }

    if (At(TokenType_Other, "~") || At(TokenType_Other, "^"))
    {
        auto type_token = Skip();
        auto type = ToOffsetType(type_token.Value);

        auto negative = SkipIf(TokenType_Operator, "-");
        auto token = negative
                         ? std::optional(ExpectAny(TokenType_Integer, TokenType_Float))
                         : AtAny(TokenType_Integer, TokenType_Float)
                               ? std::optional(Skip())
                               : std::nullopt;

        auto offset = !token
                          ? 0
                          : token->Type == TokenType_Integer
                                ? token->Integer
                                : token->Float;

        return std::make_unique<ConstantExpression>(
            type_token.Where,
            ConstantOffset::Create(type, negative ? -offset : offset),
            ToString(type) + (token ? (negative ? "-" : "") + token->Value : ""));
    }

    Error(m_Token.Where, "cannot parse {} '{}'", m_Token.Type, m_Token.Value);
}
