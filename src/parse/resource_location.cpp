#include <mcc/parse.hpp>

mcc::ResourceLocation mcc::Parser::ParseResourceLocation()
{
    auto ns = std::string(SkipIf(TokenType_Other, ":") ? "" : "minecraft");
    auto id = Expect(TokenType_Symbol).Value;

    if (!ns.empty() && SkipIf(TokenType_Other, ":"))
    {
        ns = id;
        id = Expect(TokenType_Symbol).Value;
    }

    return {ns, id};
}
