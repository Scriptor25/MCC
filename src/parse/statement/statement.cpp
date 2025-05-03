#include <format>
#include <mcc/parse.hpp>

mcc::StatementPtr mcc::Parser::ParseStatement()
{
    if (m_Token.Type == TokenType_EOF)
        return {};

    if (At(TokenType_Symbol, "namespace"))
        return ParseNamespaceStatement();
    if (At(TokenType_Symbol, "define"))
        return ParseDefineStatement();

    throw std::runtime_error(
        std::format(
            "{}({},{}): cannot parse {} '{}'",
            m_Token.Where.Filename,
            m_Token.Where.Row,
            m_Token.Where.Col,
            m_Token.Type,
            m_Token.Value));
}
