#include <format>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParsePrimaryExpression()
{
    if (SkipIf(TokenType_Symbol, "if"))
        return ParseIfUnlessExpression(false);

    if (SkipIf(TokenType_Symbol, "unless"))
        return ParseIfUnlessExpression(true);

    if (At(TokenType_Symbol, "return"))
        return ParseReturnExpression();

    if (AtEnum("true", "false"))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            ConstantBoolean::Create(token.Value == "true"),
            std::move(token.Value));
    }

    if (At(TokenType_Integer))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            ConstantInteger::Create(token.Integer),
            token.Value);
    }

    if (At(TokenType_Float))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(
            ConstantFloat::Create(token.Float),
            token.Value);
    }

    if (At(TokenType_String))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(ConstantString::Create(token.Value), '"' + token.Value + '"');
    }

    if (At(TokenType_Symbol) || At(TokenType_Other, ":"))
        return ParseResourceExpression();

    if (At(TokenType_Target))
        return ParseTargetExpression();

    if (At(TokenType_Other, "["))
        return ParseArrayExpression();

    if (At(TokenType_Other, "{"))
        return ParseObjectExpression();

    if (At(TokenType_FormatString))
        return ParseFormatExpression();

    if (SkipIf(TokenType_Operator, "-"))
    {
        if (At(TokenType_Integer))
        {
            auto token = Skip();
            return std::make_unique<ConstantExpression>(
                ConstantInteger::Create(-token.Integer),
                '-' + token.Value);
        }

        auto token = Expect(TokenType_Float);
        return std::make_unique<ConstantExpression>(
            ConstantFloat::Create(-token.Float),
            '-' + token.Value);
    }

    if (SkipIf(TokenType_Operator, "%"))
    {
        auto id = Expect(TokenType_Symbol).Value;
        return std::make_unique<SymbolExpression>(id);
    }

    if (At(TokenType_Other, "~") || At(TokenType_Other, "^"))
    {
        auto type = ToOffsetType(Skip().Value);

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
            ConstantOffset::Create(type, offset),
            ToString(type) + (token ? (negative ? "-" : "") + token->Value : ""));
    }

    Error(m_Token.Where, "cannot parse {} '{}'", m_Token.Type, m_Token.Value);
}
