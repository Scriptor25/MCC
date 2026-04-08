#include <mcc/parse.hpp>

mcc::ResourceLocation mcc::Parser::ParseResourceLocation(const bool simple_path)
{
    std::string namespace_;
    std::vector<std::string> path;

    const auto use_default = !SkipIf(TokenType::Other, ":");

    do
        path.emplace_back(Expect(TokenType::Symbol).Value);
    while (!simple_path && SkipIf(TokenType::Operator, "/"));

    if (use_default && SkipIf(TokenType::Other, ":"))
    {
        namespace_ = path.front();

        path.clear();
        do
            path.emplace_back(Expect(TokenType::Symbol).Value);
        while (!simple_path && SkipIf(TokenType::Operator, "/"));
    }

    return { namespace_, path };
}
