#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::CallExpression::CallExpression(std::string callee, std::vector<ExpressionPtr> arguments)
    : Callee(std::move(callee)),
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

mcc::ValuePtr mcc::CallExpression::Gen(Builder &builder, const bool inline_) const
{
    std::vector<ValuePtr> arguments;

    for (auto &argument: Arguments)
        arguments.emplace_back(argument->Gen(builder, inline_));

    return builder.CreateCall(ToCallee(Callee), std::move(arguments), inline_);
}
