#include <mcc/builder.hpp>
#include <mcc/tree.hpp>
#include <utility>

mcc::NamespaceNode::NamespaceNode(const SourceLocation &where, std::string namespace_)
    : TreeNode(where), Namespace(std::move(namespace_))
{
}

std::ostream &mcc::NamespaceNode::Print(std::ostream &stream) const
{
    return stream << "namespace " << Namespace;
}

void mcc::NamespaceNode::Generate(Builder &builder) const
{
    builder.SetNamespace(Namespace);
}

void mcc::NamespaceNode::GenerateInclude(Builder &builder, std::set<std::filesystem::path> &include_chain) const
{
    builder.SetNamespace(Namespace);
}
