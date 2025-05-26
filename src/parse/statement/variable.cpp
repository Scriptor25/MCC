#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseVariableStatement()
{
    auto where = m_Token.Where;
    auto is_constant = SkipIf(TokenType_Symbol, "const") || (Expect(TokenType_Symbol, "let"), false);

    std::vector<std::string> names;
    do
        names.emplace_back(Expect(TokenType_Symbol).Value);
    while (SkipIf(TokenType_Other, ","));

    TypePtr type;
    if (SkipIf(TokenType_Other, ":"))
        type = ParseType();

    ExpressionPtr value;
    if (SkipIf(TokenType_Operator, "=") || (is_constant && (Expect(TokenType_Operator, "="), true)))
        value = ParseExpression();

    Assert(type || value, where, "variable definition must at least specify either a type or a value");

    return std::make_unique<VariableStatement>(where, is_constant, names, std::move(type), std::move(value));
}
