#include <mcc/error.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseCallExpression()
{
    auto expression = ParsePrimaryExpression();

    if (!At(TokenType_Other, "("))
        return expression;

    auto where = Skip().Where;

    std::string callee;
    bool builtin;

    if (const auto symbol = dynamic_cast<SymbolExpression *>(expression.get()))
    {
        callee = symbol->ID;
        builtin = true;
    }
    else if (const auto resource = dynamic_cast<ResourceExpression *>(expression.get()))
    {
        callee = resource->Location.String();
        builtin = false;
    }
    else
    {
        Error("callee must be a symbol or resource location");
    }

    std::vector<ExpressionPtr> arguments;
    while (!At(TokenType_Other, ")") && !At(TokenType_EOF))
    {
        arguments.emplace_back(ParseExpression());

        if (!At(TokenType_Other, ")"))
            Expect(TokenType_Other, ",");
    }
    Expect(TokenType_Other, ")");

    return std::make_unique<CallExpression>(std::move(where), std::move(callee), builtin, std::move(arguments));
}
