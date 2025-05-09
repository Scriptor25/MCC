#include <format>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParsePrimaryExpression()
{
    if (AtEnum("if", "unless"))
        return ParseIfUnlessExpression();

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
        auto token = Skip();
        const auto type = ToOffsetType(token.Value);

        const auto negative = SkipIf(TokenType_Operator, "-");

        ExpressionPtr offset;
        ConstantExpression *offset_constant = nullptr;
        if (negative || AtAny(TokenType_Integer, TokenType_Float))
        {
            offset = ParseFloatExpression(negative);
            offset_constant = dynamic_cast<ConstantExpression *>(offset.get());
            Assert(!!offset_constant, token.Where, "offset must be constant");
        }

        return std::make_unique<ConstantExpression>(
            token.Where,
            ConstantOffset::Create(type, offset_constant ? offset_constant->Value : nullptr),
            ToString(type) + (offset_constant ? offset_constant->View : ""));
    }

    Error(m_Token.Where, "cannot parse {} '{}'", m_Token.Type, m_Token.Value);
}
