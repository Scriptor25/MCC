#include <mcc/parse.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>

mcc::TreeNodePtr mcc::Parser::ParseTypeNode()
{
    auto where      = Expect(TokenType::Symbol, "type").Where;
    const auto name = Expect(TokenType::Symbol).Value;
    Expect(TokenType::Operator, "=");
    const auto type = ParseType();
    m_Context.SetNamed(name, type);
    return nullptr;
}
