#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::TreeNodePtr mcc::Parser::ParseNamespaceNode()
{
    auto where = Expect(TokenType_Symbol, "namespace").Where;

    auto namespace_ = Expect(TokenType_Symbol).Value;

    return std::make_unique<NamespaceNode>(where, namespace_);
}
