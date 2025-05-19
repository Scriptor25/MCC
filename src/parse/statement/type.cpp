#include <mcc/parse.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>

mcc::StatementPtr mcc::Parser::ParseTypeStatement()
{
    auto where = Expect(TokenType_Symbol, "type").Where;
    const auto name = Expect(TokenType_Symbol).Value;
    Expect(TokenType_Operator, "=");
    const auto type = ParseType();
    m_Context.SetNamed(name, type);
    return nullptr;
}
