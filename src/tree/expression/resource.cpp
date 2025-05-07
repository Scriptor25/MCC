#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::ResourceExpression::ResourceExpression(SourceLocation where, ResourceLocation location, ExpressionPtr nbt)
    : Expression(std::move(where)),
      Location(std::move(location)),
      NBT(std::move(nbt))
{
}

std::ostream &mcc::ResourceExpression::Print(std::ostream &stream) const
{
    Location.Print(stream);

    if (NBT)
        NBT->Print(stream);

    return stream;
}

mcc::ValuePtr mcc::ResourceExpression::Generate(Builder &builder, bool inline_) const
{
    auto location = Location;
    if (location.Namespace.empty())
        location.Namespace = builder.GetLocation().Namespace;

    ConstantPtr nbt;
    if (NBT)
    {
        nbt = std::dynamic_pointer_cast<Constant>(NBT->Generate(builder, true));
        Assert(!!nbt, "nbt must be constant");
    }

    return ConstantResource::Create(std::move(location), std::move(nbt));
}
