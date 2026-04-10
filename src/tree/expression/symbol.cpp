#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/expression.hpp>
#include <mcc/function.hpp>
#include <mcc/value.hpp>

mcc::SymbolExpression::SymbolExpression(
        const SourceLocation &where,
        std::string name)
    : Expression(where),
      Name(std::move(name))
{
}

std::ostream &mcc::SymbolExpression::Print(std::ostream &stream) const
{
    return stream << Name;
}

mcc::ValuePtr mcc::SymbolExpression::GenerateValue(
        Builder &builder,
        const Frame &frame) const
{
    return builder.GetVariable(Where, Name);
}

mcc::FunctionPtr mcc::SymbolExpression::GenerateCallee(
        Builder &builder,
        const ParameterRefList &parameters) const
{
    auto candidates = builder.FindCandidates(Name, parameters);
    return builder.FindUnambiguousCandidate(Where, candidates, parameters);
}
