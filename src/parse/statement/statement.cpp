#include <format>
#include <mcc/error.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::StatementPtr mcc::Parser::ParseTopLevel()
{
    if (m_Token.Type == TokenType_EOF)
        return {};

    if (At(TokenType_Symbol, "namespace"))
        return ParseNamespaceStatement();
    if (At(TokenType_Symbol, "define"))
        return ParseDefineStatement();

    Error(
        m_Token.Where,
        "cannot parse {} '{}'",
        m_Token.Type,
        m_Token.Value);
}

mcc::StatementPtr mcc::Parser::ParseStatement()
{
    if (At(TokenType_Other, "{"))
        return ParseMultiStatement();
    if (AtEnum("if", "unless"))
        return ParseIfUnlessStatement();
    if (At(TokenType_Symbol, "for"))
        return ParseForStatement();
    if (At(TokenType_Symbol, "return"))
        return ParseReturnStatement();

    return ParseExpression();
}
