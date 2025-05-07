#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::CallExpression::CallExpression(
    SourceLocation where,
    std::string callee,
    const bool builtin,
    std::vector<ExpressionPtr> arguments)
    : Expression(std::move(where)),
      Callee(std::move(callee)),
      Builtin(builtin),
      Arguments(std::move(arguments))
{
}

std::ostream &mcc::CallExpression::Print(std::ostream &stream) const
{
    stream << Callee << '(';
    auto first = true;
    for (auto &argument: Arguments)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        argument->Print(stream);
    }
    return stream << ')';
}

mcc::ValuePtr mcc::CallExpression::Generate(Builder &builder, const bool inline_) const
{
    std::vector<ValuePtr> arguments;
    for (auto &argument: Arguments)
        arguments.emplace_back(argument->Generate(builder, inline_));

    return builder.CreateCall(Callee, Builtin, std::move(arguments), inline_);
}
