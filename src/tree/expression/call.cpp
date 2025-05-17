#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::CallExpression::CallExpression(
    SourceLocation where,
    ExpressionPtr callee,
    std::vector<ExpressionPtr> arguments)
    : Expression(std::move(where)),
      Callee(std::move(callee)),
      Arguments(std::move(arguments))
{
}

std::ostream &mcc::CallExpression::Print(std::ostream &stream) const
{
    Callee->Print(stream) << '(';
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

mcc::ValuePtr mcc::CallExpression::GenerateValue(Builder &builder) const
{
    std::string callee;
    bool builtin;

    if (const auto symbol = dynamic_cast<SymbolExpression *>(Callee.get()))
    {
        callee = symbol->Name;
        builtin = true;
    }
    else if (const auto resource = dynamic_cast<ResourceExpression *>(Callee.get()))
    {
        auto location = resource->Location;
        if (location.Namespace.empty())
            location.Namespace = builder.GetLocation().Namespace;
        callee = location.String();
        builtin = false;
    }
    else
    {
        Error(Where, "invalid callee expression, must be a builtin symbol or function resource");
    }

    std::vector<ValuePtr> arguments;
    for (auto &argument: Arguments)
        arguments.emplace_back(argument->GenerateValue(builder));

    return builder.CreateCall(callee, builtin, std::move(arguments));
}
