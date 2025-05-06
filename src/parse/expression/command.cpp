#include <mcc/context.hpp>
#include <mcc/error.hpp>
#include <mcc/parse.hpp>

mcc::ExpressionPtr mcc::Parser::ParseCommandExpression()
{
    auto base = Expect(TokenType_Symbol).Value;
    auto &definitions = m_Context.Definitions;
    Assert(definitions.contains(base), "undefined command {}", base);

    auto &definition = definitions.at(base);

    for (auto &node: definition.Nodes);
}
