#include <mcc/parse.hpp>

mcc::ResourceLocation mcc::Parser::ParseResourceLocation(const bool simple_path)
{
    std::string namespace_;
    std::vector<std::string> path;

    const auto use_default = !SkipIf(TokenType_Other, ":");

    do
        path.emplace_back(Expect(TokenType_Symbol).Value);
    while (!simple_path && SkipIf(TokenType_Operator, "/"));

    if (use_default && SkipIf(TokenType_Other, ":"))
    {
        namespace_ = path.front();

        path.clear();
        do
            path.emplace_back(Expect(TokenType_Symbol).Value);
        while (!simple_path && SkipIf(TokenType_Operator, "/"));
    }

    return { namespace_, path };
}
