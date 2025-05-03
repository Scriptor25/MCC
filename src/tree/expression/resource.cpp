#include <mcc/builder.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::ResourceExpression::ResourceExpression(std::string namespace_, std::string path)
    : Location(std::move(namespace_), std::move(path))
{
}

std::ostream &mcc::ResourceExpression::Print(std::ostream &stream) const
{
    return Location.Print(stream);
}

mcc::ValuePtr mcc::ResourceExpression::Gen(Builder &builder, bool inline_) const
{
    auto location = Location;
    if (location.Namespace.empty())
        location.Namespace = builder.GetLocation().Namespace;

    return ConstantResource::Create(location);
}
