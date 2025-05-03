#include <mcc/parse.hpp>

mcc::StatementPtr mcc::Parser::ParseNamespaceStatement()
{
    Expect(TokenType_Symbol, "namespace");

    auto namespace_ = Expect(TokenType_Symbol).Value;

    return std::make_unique<NamespaceStatement>(namespace_);
}
