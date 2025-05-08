#include <mcc/builder.hpp>
#include <mcc/context.hpp>
#include <mcc/tree.hpp>

mcc::NamespaceStatement::NamespaceStatement(SourceLocation where, std::string namespace_)
    : Statement(std::move(where)),
      Namespace(std::move(namespace_))
{
}

std::ostream &mcc::NamespaceStatement::Print(std::ostream &stream) const
{
    return stream << "namespace " << Namespace;
}

void mcc::NamespaceStatement::Generate(Builder &builder) const
{
    builder.GetContext().Namespace = Namespace;
}
