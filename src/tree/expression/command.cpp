#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::CommandExpression::CommandExpression(
    SourceLocation where,
    std::string command,
    std::vector<ExpressionPtr> arguments)
    : Expression(std::move(where)),
      Command(std::move(command)),
      Arguments(std::move(arguments))
{
}

std::ostream &mcc::CommandExpression::Print(std::ostream &stream) const
{
    stream << Command << '(';
    for (unsigned i = 0; i < Arguments.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        Arguments[i]->Print(stream);
    }
    return stream << ')';
}

mcc::ValuePtr mcc::CommandExpression::Generate(Builder &builder, const bool inline_) const
{
    std::vector<ValuePtr> arguments;

    for (auto &argument: Arguments)
        arguments.emplace_back(argument->Generate(builder, inline_));

    std::vector<std::string> path;

    auto command = Command;
    for (size_t pos; (pos = command.find('.')) != std::string::npos;)
    {
        path.emplace_back(command.substr(0, pos));
        command = command.substr(pos + 1);
    }

    if (!command.empty())
        path.emplace_back(command);

    return builder.CreateCommand(
        std::move(path),
        std::move(arguments),
        inline_);
}
