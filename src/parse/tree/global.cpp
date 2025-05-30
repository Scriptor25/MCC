#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::TreeNodePtr mcc::Parser::ParseGlobalNode()
{
    auto where = Expect(TokenType_Symbol, "global").Where;
    auto location = ParseResourceLocation(true);
    Expect(TokenType_Operator, "=>");
    auto type = ParseType();

    return std::make_unique<GlobalNode>(where, location, type);
}
