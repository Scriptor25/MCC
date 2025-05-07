#include <mcc/context.hpp>
#include <mcc/error.hpp>
#include <mcc/parse.hpp>
#include <mcc/tree.hpp>

mcc::ExpressionPtr mcc::Parser::ParseCommandExpression()
{
    auto where = m_Token.Where;
    auto command = Expect(TokenType_Symbol).Value;
    auto &definitions = m_Context.Definitions;
    Assert(definitions.contains(command), where, "undefined command {}", command);

    std::vector<ExpressionPtr> arguments;
    for (auto &definition = definitions.at(command); auto &node: definition.Nodes)
    {
        auto argument = node->Parse(*this, definition);
        if (argument.empty())
            break;

        for (auto &a: argument)
            arguments.emplace_back(std::move(a));
    }
    Expect(TokenType_Other, ";");

    return std::make_unique<CommandExpression>(std::move(where), std::move(command), std::move(arguments));
}
