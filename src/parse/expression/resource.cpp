#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseResourceExpression()
{
    std::string ns;

    if (!SkipIf(TokenType_Other, ":"))
        ns = "minecraft";

    auto id = Expect(TokenType_Symbol).Value;

    if (!ns.empty() && SkipIf(TokenType_Other, ":"))
    {
        ns = id;
        id = Expect(TokenType_Symbol).Value;
    }

    return std::make_unique<ResourceExpression>(ns, id);
}
