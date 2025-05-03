#include <mcc/context.hpp>
#include <mcc/tree.hpp>

mcc::NamespaceStatement::NamespaceStatement(std::string namespace_)
    : Namespace(std::move(namespace_))
{
}

std::ostream &mcc::NamespaceStatement::Print(std::ostream &stream) const
{
    return stream << "namespace " << Namespace;
}

void mcc::NamespaceStatement::Gen(Context &context) const
{
    context.Namespace = Namespace;
}
