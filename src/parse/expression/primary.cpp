#include <format>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParsePrimaryExpression()
{
    if (SkipIf(TokenType_Symbol, "if"))
        return ParseIfUnlessExpression(false);

    if (SkipIf(TokenType_Symbol, "unless"))
        return ParseIfUnlessExpression(true);

    if (At(TokenType_Symbol, "return"))
        return ParseReturnExpression();

    if (At(TokenType_Symbol, "true") || At(TokenType_Symbol, "false"))
        return std::make_unique<BooleanExpression>(ExpectEnum("true", "false").Value == "true");

    if (At(TokenType_Integer))
        return std::make_unique<IntegerExpression>(Expect(TokenType_Integer).Integer);

    if (At(TokenType_Float))
        return std::make_unique<FloatExpression>(Expect(TokenType_Float).Float);

    if (At(TokenType_String))
        return std::make_unique<StringExpression>(Expect(TokenType_String).Value);

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

    if (SkipIf(TokenType_Other, "~"))
    {
        auto offset = At(TokenType_Float)
                          ? Skip().Float
                          : At(TokenType_Integer)
                                ? static_cast<double>(Skip().Integer)
                                : 0.0;
        return std::make_unique<RelativeOffsetExpression>(offset);
    }

    if (SkipIf(TokenType_Other, "^"))
    {
        auto offset = At(TokenType_Float)
                          ? Skip().Float
                          : At(TokenType_Integer)
                                ? static_cast<double>(Skip().Integer)
                                : 0.0;
        return std::make_unique<LocalOffsetExpression>(offset);
    }

    if (SkipIf(TokenType_Operator, "%"))
    {
        auto id = Expect(TokenType_Symbol).Value;
        return std::make_unique<SymbolExpression>(id);
    }

    throw std::runtime_error(
        std::format(
            "{}({},{}): cannot parse {} '{}'",
            m_Token.Where.Filename,
            m_Token.Where.Row,
            m_Token.Where.Col,
            m_Token.Type,
            m_Token.Value));
}
