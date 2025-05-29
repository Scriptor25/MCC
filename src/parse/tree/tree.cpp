#include <mcc/error.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::TreeNodePtr mcc::Parser::ParseTreeNode()
{
    if (m_Token.Type == TokenType_EOF)
        return {};

    if (At(TokenType_Symbol, "define"))
        return ParseDefineNode();
    if (At(TokenType_Symbol, "include"))
        return ParseIncludeNode();
    if (At(TokenType_Symbol, "namespace"))
        return ParseNamespaceNode();
    if (At(TokenType_Symbol, "type"))
        return ParseTypeNode();

    Error(m_Token.Where, "cannot parse {} '{}'", m_Token.Type, m_Token.Value);
}
