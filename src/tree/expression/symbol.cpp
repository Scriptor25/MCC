#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/function.hpp>

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
    if (builder.HasVariable(Name))
        return builder.GetVariable(Where, Name);

    auto candidates = builder.FindFunctions({ Name }, true);
    Assert(!candidates.empty(), Where, "candidates must not be empty");
    Assert(candidates.size() == 1, Where, "ambiguous candidates");

    return candidates.front();
}

mcc::FunctionPtr mcc::SymbolExpression::GenerateCallee(
        Builder &builder,
        const ParameterRefList &parameters) const
{
    const auto candidates = builder.FindFunctions({ Name }, parameters, true);
    return builder.FindUnambiguousCandidate(Where, candidates, parameters);
}
