#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/tree.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::GlobalNode::GlobalNode(const SourceLocation &where, const ResourceLocation &location, const TypePtr &type)
    : TreeNode(where),
      Location(location),
      Type(type)
{
}

std::ostream &mcc::GlobalNode::Print(std::ostream &stream) const
{
    return stream << "global " << Location << " => " << Type;
}

void mcc::GlobalNode::Generate(Builder &builder) const
{
    if (!builder.HasGlobal(Location))
        builder.CreateGlobal(Where, Location, Type);
    else
    {
        const auto global = builder.GetGlobal(Where, Location);
        Assert(global->Type == Type, Where, "cannot define global with different type");
    }
}

void mcc::GlobalNode::GenerateInclude(Builder &builder, std::set<std::filesystem::path> &include_chain) const
{
    if (!builder.HasGlobal(Location))
        builder.CreateGlobal(Where, Location, Type);
    else
    {
        const auto global = builder.GetGlobal(Where, Location);
        Assert(global->Type == Type, Where, "cannot define global with different type");
    }
}
