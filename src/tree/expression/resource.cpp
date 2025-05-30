#include <utility>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/expression.hpp>

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

    auto location = Location;
    if (location.Namespace.empty())
        location.Namespace = builder.GetInsertBlock()->Parent->Location.Namespace;

    return ConstantResource::Create(Where, builder.GetContext(), location);
}
