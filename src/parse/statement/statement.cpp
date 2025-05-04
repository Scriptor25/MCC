#include <format>
#include <mcc/error.hpp>
#include <mcc/parse.hpp>

mcc::StatementPtr mcc::Parser::ParseStatement()
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
