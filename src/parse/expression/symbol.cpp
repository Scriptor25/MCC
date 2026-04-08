#include <mcc/expression.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseSymbolExpression()
{
    auto token = Expect(TokenType::Symbol);
    if (!SkipIf(TokenType::Other, ":"))
        return std::make_unique<SymbolExpression>(token.Where, token.Value);

    std::vector<std::string> path;
    do
        path.push_back(Expect(TokenType::Symbol).Value);
    while (SkipIf(TokenType::Operator, "/"));

    return std::make_unique<ResourceExpression>(token.Where, ResourceLocation(token.Value, path));
}
