#include <mcc/error.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseCallExpression()
{
    auto expression = ParsePrimaryExpression();

    auto where = m_Token.Where;
    if (!SkipIf(TokenType_Other, "("))
        return expression;

    const auto callee = dynamic_cast<SymbolExpression *>(expression.get());
    Assert(callee, "function callee must be a valid command symbol");

    std::vector<ExpressionPtr> arguments;

    while (!SkipIf(TokenType_Other, ")"))
    {
        arguments.emplace_back(ParseExpression());
        if (!At(TokenType_Other, ")"))
            Expect(TokenType_Other, ",");
    }

    return std::make_unique<CallExpression>(where, callee->ID, std::move(arguments));
}
