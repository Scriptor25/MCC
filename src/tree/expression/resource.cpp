#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::ResourceExpression::ResourceExpression(SourceLocation where, ResourceLocation location)
    : Expression(std::move(where)),
      Location(std::move(location))
{
}

std::ostream &mcc::ResourceExpression::Print(std::ostream &stream) const
{
    return Location.Print(stream);
}

mcc::ValuePtr mcc::ResourceExpression::Generate(Builder &builder, bool inline_) const
{
    auto location = Location;
    if (location.Namespace.empty())
        location.Namespace = builder.GetLocation().Namespace;

    return ConstantResource::Create(location);
}
