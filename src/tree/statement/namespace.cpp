#include <mcc/builder.hpp>
#include <mcc/context.hpp>
#include <mcc/statement.hpp>

mcc::NamespaceStatement::NamespaceStatement(const SourceLocation &where, const std::string &namespace_)
    : Statement(where),
      Namespace(namespace_)
{
}

std::ostream &mcc::NamespaceStatement::Print(std::ostream &stream) const
{
    return stream << "namespace " << Namespace;
}

void mcc::NamespaceStatement::Generate(Builder &builder, const Frame &frame) const
{
    builder.GetContext().Namespace = Namespace;
}
