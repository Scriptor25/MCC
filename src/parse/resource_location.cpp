#include <mcc/parse.hpp>

mcc::ResourceLocation mcc::Parser::ParseResourceLocation()
{
    std::string namespace_(SkipIf(TokenType_Other, ":") ? "" : "minecraft");

    auto path = Expect(TokenType_Symbol).Value;
    while (SkipIf(TokenType_Operator, "/"))
        path += '/' + Expect(TokenType_Symbol).Value;

    if (!namespace_.empty() && SkipIf(TokenType_Other, ":"))
    {
        namespace_ = std::move(path);

        path = Expect(TokenType_Symbol).Value;
        while (SkipIf(TokenType_Operator, "/"))
            path += '/' + Expect(TokenType_Symbol).Value;
    }

    return {std::move(namespace_), std::move(path)};
}
