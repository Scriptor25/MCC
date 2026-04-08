#include <mcc/error.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::TreeNodePtr mcc::Parser::ParseTreeNode()
{
    if (m_Token.Type == TokenType::EoF)
        return {};

    if (At(TokenType::Symbol, "define"))
        return ParseDefineNode();
    if (At(TokenType::Symbol, "global"))
        return ParseGlobalNode();
    if (At(TokenType::Symbol, "include"))
        return ParseIncludeNode();
    if (At(TokenType::Symbol, "namespace"))
        return ParseNamespaceNode();
    if (At(TokenType::Symbol, "type"))
        return ParseTypeNode();

    Error(m_Token.Where, "cannot parse {} '{}'", m_Token.Type, m_Token.Value);
}
