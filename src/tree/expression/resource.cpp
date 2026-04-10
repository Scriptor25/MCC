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
        location.Namespace = builder.GetNamespace();

    const auto candidates = builder.FindFunctions(location, parameters);
    return Builder::FindUnambiguousCandidate(Where, candidates, parameters);
}
