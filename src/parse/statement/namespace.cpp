#include <mcc/parse.hpp>

mcc::StatementPtr mcc::Parser::ParseNamespaceStatement()
{
    auto where = Expect(TokenType_Symbol, "namespace").Where;

    auto namespace_ = Expect(TokenType_Symbol).Value;

    return std::make_unique<NamespaceStatement>(where, namespace_);
}
