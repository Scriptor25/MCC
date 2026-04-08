#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParsePrimaryExpression()
{
    if (AtEnum("if", "unless"))
        return ParseIfUnlessExpression();
    if (At(TokenType::Symbol, "switch"))
        return ParseSwitchExpression();
    if (At(TokenType::Symbol, "ref"))
        return ParseRefExpression();
    if (At(TokenType::Number))
        return ParseNumberExpression();
    if (At(TokenType::String))
        return ParseStringExpression();
    if (At(TokenType::FormatString))
        return ParseFormatExpression();
    if (At(TokenType::Symbol))
        return ParseSymbolExpression();
    if (At(TokenType::Other, "?"))
        return ParseCommandExpression();
    if (At(TokenType::Other, "!"))
        return ParseMacroExpression();
    if (At(TokenType::Other, "["))
        return ParseArrayExpression();
    if (At(TokenType::Other, "{"))
        return ParseObjectExpression();

    if (SkipIf(TokenType::Other, "("))
    {
        auto expression = ParseExpression();
        Expect(TokenType::Other, ")");
        return expression;
    }

    if (At(TokenType::Operator))
    {
        auto token   = Skip();
        auto operand = ParseOperandExpression();
        return std::make_unique<UnaryExpression>(token.Where, token.Value, std::move(operand));
    }

    Error(m_Token.Where, "cannot parse {} '{}'", m_Token.Type, m_Token.Value);
}
