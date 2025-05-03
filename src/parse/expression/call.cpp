#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseCallExpression()
{
    auto expression = ParsePrimaryExpression();

    if (!SkipIf(TokenType_Other, "("))
        return expression;

    const auto callee = dynamic_cast<ResourceExpression *>(expression.get());
    if (!callee)
        throw std::runtime_error("function callee must be a resource location of any kind");

    std::vector<ExpressionPtr> arguments;

    while (!SkipIf(TokenType_Other, ")"))
    {
        arguments.emplace_back(ParseExpression());
        if (!At(TokenType_Other, ")"))
            Expect(TokenType_Other, ",");
    }

    return std::make_unique<CallExpression>(callee->Location.Path, std::move(arguments));
}
