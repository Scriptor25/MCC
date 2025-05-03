#include <mcc/parse.hpp>

mcc::ResourceLocation mcc::Parser::ParseResourceLocation()
{
    auto namespace_ = std::string(SkipIf(TokenType_Other, ":") ? "" : "minecraft");
    auto path = Expect(TokenType_Symbol).Value;
    while (SkipIf(TokenType_Operator, "/"))
        path += '/' + Expect(TokenType_Symbol).Value;

    if (!namespace_.empty() && SkipIf(TokenType_Other, ":"))
    {
        namespace_ = path;
        path = Expect(TokenType_Symbol).Value;
        while (SkipIf(TokenType_Operator, "/"))
            path += '/' + Expect(TokenType_Symbol).Value;
    }

    return {namespace_, path};
}
