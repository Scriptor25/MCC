#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseVariableStatement()
{
    auto where = m_Token.Where;
    auto constant = SkipIf(TokenType_Symbol, "const") || (Expect(TokenType_Symbol, "let"), false);
    auto name = Expect(TokenType_Symbol).Value;
    Expect(TokenType_Operator, "=");
    auto value = ParseExpression();
    return std::make_unique<VariableStatement>(where, constant, name, std::move(value));
}
