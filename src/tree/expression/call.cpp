#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/instruction.hpp>
#include <mcc/value.hpp>

mcc::CallExpression::CallExpression(
    const SourceLocation &where,
    ExpressionPtr callee,
    std::vector<ExpressionPtr> arguments)
    : Expression(where),
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

mcc::ValuePtr mcc::CallExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    std::vector<ValuePtr> arguments;
    for (auto &argument: Arguments)
        arguments.emplace_back(argument->GenerateValue(builder, frame));

    if (const auto macro = dynamic_cast<MacroExpression *>(Callee.get()))
        return builder.CreateMacro(Where, macro->Name, arguments);

    const auto default_namespace = builder.GetInsertBlock()->Parent->Location.Namespace;

    ResourceLocation callee;
    if (const auto symbol = dynamic_cast<SymbolExpression *>(Callee.get()))
    {
        callee = {default_namespace, symbol->Name};
    }
    else if (const auto resource = dynamic_cast<ResourceExpression *>(Callee.get()))
    {
        callee = resource->Location;
        if (callee.Namespace.empty())
            callee.Namespace = default_namespace;
    }
    else
    {
        Error(Callee->Where, "invalid callee, must be a symbol or resource");
    }

    return builder.CreateCall(Where, callee, arguments, frame.LandingPad);
}
