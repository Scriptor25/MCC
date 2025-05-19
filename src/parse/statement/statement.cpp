#include <format>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::StatementPtr mcc::Parser::ParseTopLevel()
{
    if (m_Token.Type == TokenType_EOF)
        return {};

    if (At(TokenType_Symbol, "define"))
        return ParseDefineStatement();
    if (At(TokenType_Symbol, "include"))
        return ParseIncludeStatement();
    if (At(TokenType_Symbol, "namespace"))
        return ParseNamespaceStatement();

    Error(
        m_Token.Where,
        "cannot parse {} '{}'",
        m_Token.Type,
        m_Token.Value);
}

mcc::StatementPtr mcc::Parser::ParseStatement()
{
    if (At(TokenType_Symbol, "break"))
        return ParseBreakStatement();
    if (At(TokenType_Symbol, "continue"))
        return ParseContinueStatement();
    if (At(TokenType_Symbol, "for"))
        return ParseForStatement();
    if (AtEnum("if", "unless"))
        return ParseIfUnlessStatement();
    if (At(TokenType_Other, "{"))
        return ParseMultiStatement();
    if (At(TokenType_Symbol, "return"))
        return ParseReturnStatement();
    if (At(TokenType_Symbol, "switch"))
        return ParseSwitchStatement();
    if (At(TokenType_Symbol, "throw"))
        return ParseThrowStatement();
    if (At(TokenType_Symbol, "try"))
        return ParseTryCatchStatement();
    if (AtEnum("const", "let"))
        return ParseVariableStatement();

    return ParseExpression();
}
