#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::TreeNodePtr mcc::Parser::ParseGlobalNode()
{
    auto where    = Expect(TokenType::Symbol, "global").Where;
    auto location = ParseResourceLocation(true);
    Expect(TokenType::Operator, "=>");
    auto type = ParseType();

    return std::make_unique<GlobalNode>(where, location, type);
}
