#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::CallExpression::CallExpression(SourceLocation where, std::string callee, std::vector<ExpressionPtr> arguments)
    : Expression(std::move(where)),
      Callee(std::move(callee)),
      Arguments(std::move(arguments))
{
}

std::ostream &mcc::CallExpression::Print(std::ostream &stream) const
{
    stream << Callee << '(';
    for (unsigned i = 0; i < Arguments.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        Arguments[i]->Print(stream);
    }
    return stream << ')';
}

mcc::ValuePtr mcc::CallExpression::Generate(Builder &builder, const bool inline_) const
{
    std::vector<ValuePtr> arguments;

    for (auto &argument: Arguments)
        arguments.emplace_back(argument->Generate(builder, inline_));

    std::vector<std::string> path;

    auto callee = Callee;
    for (size_t pos; (pos = callee.find('.')) != std::string::npos;)
    {
        path.emplace_back(callee.substr(0, pos));
        callee = callee.substr(pos + 1);
    }

    if (!callee.empty())
        path.emplace_back(callee);

    return builder.CreateCall(
        std::move(path),
        std::move(arguments),
        inline_);
}
