#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::TreeNodePtr mcc::Parser::ParseNamespaceNode()
{
    auto where = Expect(TokenType::Symbol, "namespace").Where;

    auto namespace_ = Expect(TokenType::Symbol).Value;

    return std::make_unique<NamespaceNode>(where, namespace_);
}
