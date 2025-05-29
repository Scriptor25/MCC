#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParsePrimaryExpression()
{
    if (AtEnum("if", "unless"))
        return ParseIfUnlessExpression();
    if (At(TokenType_Symbol, "switch"))
        return ParseSwitchExpression();
    if (At(TokenType_Symbol, "ref"))
        return ParseRefExpression();
    if (AtEnum("true", "false"))
    {
        auto token = Skip();
        return std::make_unique<ConstantExpression>(token.Where,
                                                    ConstantBoolean::Create(token.Where, m_Context, token.Value == "true"),
                                                    token.Value);
    }
    if (At(TokenType_Number))
        return ParseNumberExpression();
    if (At(TokenType_String))
        return ParseStringExpression();
    if (At(TokenType_FormatString))
        return ParseFormatExpression();
    if (At(TokenType_Symbol))
        return ParseSymbolExpression();
    if (At(TokenType_Other, "?"))
        return ParseCommandExpression();
    if (At(TokenType_Other, "!"))
        return ParseMacroExpression();
    if (At(TokenType_Other, "["))
        return ParseArrayExpression();
    if (At(TokenType_Other, "{"))
        return ParseObjectExpression();
    if (SkipIf(TokenType_Other, "("))
    {
        auto expression = ParseExpression();
        Expect(TokenType_Other, ")");
        return expression;
    }
    if (At(TokenType_Operator))
    {
        auto token   = Skip();
        auto operand = ParseOperandExpression();
        return std::make_unique<UnaryExpression>(token.Where, token.Value, std::move(operand));
    }
    Error(m_Token.Where, "cannot parse {} '{}'", m_Token.Type, m_Token.Value);
}
