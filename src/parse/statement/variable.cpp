#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/parse.hpp>
#include <mcc/statement.hpp>

mcc::StatementPtr mcc::Parser::ParseVariableStatement()
{
    auto token = ExpectEnum("let", "const", "constexpr");

    auto declarator   = *ToDeclarator(token.Value);
    auto is_reference = declarator != Declarator_ConstExpr && SkipIf(TokenType::Other, "&");

    std::vector<std::string> names;
    do
        names.emplace_back(Expect(TokenType::Symbol).Value);
    while (SkipIf(TokenType::Other, ","));

    TypePtr type;
    if (SkipIf(TokenType::Other, ":"))
        type = ParseType();

    ExpressionPtr value;
    if (SkipIf(TokenType::Operator, "=")
        || ((is_reference || declarator != Declarator_Let) && (Expect(TokenType::Operator, "="), true)))
        value = ParseExpression();

    Assert(type || value, token.Where, "variable definition must at least specify either its type or initializer");

    return std::make_unique<
            VariableStatement
    >(token.Where, declarator, is_reference, names, std::move(type), std::move(value));
}
