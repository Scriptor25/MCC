#include <mcc/parse.hpp>

mcc::ResourceLocation mcc::Parser::ParseResourceLocation(const std::string &default_namespace)
{
    const auto use_default = !SkipIf(TokenType_Other, ":");
    auto namespace_ = use_default ? default_namespace : "";

    auto path = Expect(TokenType_Symbol).Value;
    while (SkipIf(TokenType_Operator, "/"))
        path += '/' + Expect(TokenType_Symbol).Value;

    if (use_default && SkipIf(TokenType_Other, ":"))
    {
        namespace_ = path;

        path = Expect(TokenType_Symbol).Value;
        while (SkipIf(TokenType_Operator, "/"))
        {
            path += '/';
            path += Expect(TokenType_Symbol).Value;
        }
    }

    return { namespace_, path };
}
