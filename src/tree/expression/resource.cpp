#include <utility>
#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/expression.hpp>
#include <mcc/function.hpp>
#include <mcc/type.hpp>

mcc::ResourceExpression::ResourceExpression(const SourceLocation &where, ResourceLocation location)
    : Expression(where),
      Location(std::move(location))
{
}

std::ostream &mcc::ResourceExpression::Print(std::ostream &stream) const
{
    return stream << Location;
}

mcc::ValuePtr mcc::ResourceExpression::GenerateValue(Builder &builder, const Frame &frame) const
{
    if (builder.HasGlobal(Location))
        return builder.GetGlobal(Where, Location);

    if (builder.HasFunction(Location))
        return builder.GetFunction(Where, Location);

    Error(Where, "no global symbol or function at resource location {}", Location);
}
