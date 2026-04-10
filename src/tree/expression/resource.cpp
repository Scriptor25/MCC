#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/enums.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/function.hpp>

mcc::ResourceExpression::ResourceExpression(
        const SourceLocation &where,
        ResourceLocation location)
    : Expression(where),
      Location(std::move(location))
{
}

std::ostream &mcc::ResourceExpression::Print(std::ostream &stream) const
{
    return stream << Location;
}

mcc::ValuePtr mcc::ResourceExpression::GenerateValue(
        Builder &builder,
        const Frame &frame) const
{
    return builder.GetGlobal(Where, Location);
}

mcc::FunctionPtr mcc::ResourceExpression::GenerateCallee(
        Builder &builder,
        const ParameterRefList &parameters) const
{
    auto location = Location;
    if (location.Namespace.empty())
        location.Namespace = builder.GetInsertBlock()->Parent->Location.Namespace;

    auto candidates = builder.FindCandidates(location, parameters);
    return builder.FindUnambiguousCandidate(Where, candidates, parameters);
}
