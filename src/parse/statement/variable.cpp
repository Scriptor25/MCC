#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseVariableStatement()
{
    auto token = ExpectEnum("let", "const", "constexpr");

    auto declarator = *ToDeclarator(token.Value);
    auto is_reference = declarator == Declarator_Let && SkipIf(TokenType_Other, "&");

    std::vector<std::string> names;
    do
        names.emplace_back(Expect(TokenType_Symbol).Value);
    while (SkipIf(TokenType_Other, ","));

    TypePtr type;
    if (SkipIf(TokenType_Other, ":"))
        type = ParseType();

    ExpressionPtr value;
    if (SkipIf(TokenType_Operator, "=") || (declarator != Declarator_Let && (Expect(TokenType_Operator, "="), true)))
        value = ParseExpression();

    Assert(type || value, token.Where, "variable definition must at least specify either its type or initializer");

    return std::make_unique<VariableStatement>(
        token.Where,
        declarator,
        is_reference,
        names,
        std::move(type),
        std::move(value));
}
