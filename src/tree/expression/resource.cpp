#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::ResourceExpression::ResourceExpression(
    SourceLocation where,
    ResourceLocation location,
    ExpressionPtr state,
    ExpressionPtr data)
    : Expression(std::move(where)),
      Location(std::move(location)),
      State(std::move(state)),
      Data(std::move(data))
{
}

std::ostream &mcc::ResourceExpression::Print(std::ostream &stream) const
{
    Location.Print(stream);

    if (State)
        State->Print(stream);

    if (Data)
        Data->Print(stream);

    return stream;
}

mcc::ValuePtr mcc::ResourceExpression::Generate(Builder &builder, bool inline_) const
{
    auto location = Location;
    if (location.Namespace.empty())
        location.Namespace = builder.GetLocation().Namespace;

    ConstantPtr state;
    if (State)
    {
        state = std::dynamic_pointer_cast<Constant>(State->Generate(builder, true));
        Assert(!!state, "state must be constant");
    }

    ConstantPtr data;
    if (Data)
    {
        data = std::dynamic_pointer_cast<Constant>(Data->Generate(builder, true));
        Assert(!!data, "data must be constant");
    }

    return ConstantResource::Create(std::move(location), std::move(state), std::move(data));
}
