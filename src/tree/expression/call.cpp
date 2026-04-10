#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/common.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>
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
    for (auto &argument : Arguments)
    {
        if (first)
            first = false;
        else
            stream << ", ";
        argument->Print(stream);
    }
    return stream << ')';
}

mcc::ValuePtr mcc::CallExpression::GenerateValue(
        Builder &builder,
        const Frame &frame) const
{
    ParameterRefList parameters(Arguments.size());
    std::vector<ValuePtr> arguments(Arguments.size());

    for (unsigned i = 0; i < Arguments.size(); ++i)
    {
        auto &argument = Arguments[i];
        auto value     = argument->GenerateValue(builder, frame);

        parameters[i] = { value->Type, value->FieldType };
        arguments[i]  = std::move(value);
    }

    if (const auto macro = dynamic_cast<MacroExpression *>(Callee.get()))
        return builder.CreateMacro(Where, {}, macro->Name, arguments);

    auto callee = Callee->GenerateCallee(builder, parameters);

    return builder.CreateCall(Where, {}, callee, arguments, frame.LandingPad);
}
