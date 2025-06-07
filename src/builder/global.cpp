#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/value.hpp>

mcc::ValuePtr mcc::Builder::CreateGlobal(const SourceLocation &where, ResourceLocation location, const TypePtr &type)
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    auto &global = m_Globals[location.Namespace][location.Path];
    Assert(!global, where, "already defined global {}", location);
    return global = GenericStorageReference::Create(
               where,
               type,
               { location.Namespace, { "__storage__" } },
               location.Path.front(),
               true);
}

bool mcc::Builder::HasGlobal(ResourceLocation location) const
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    return m_Globals.contains(location.Namespace) && m_Globals.at(location.Namespace).contains(location.Path);
}

mcc::ValuePtr mcc::Builder::GetGlobal(const SourceLocation &where, ResourceLocation location) const
{
    if (location.Namespace.empty())
        location.Namespace = m_Namespace;

    Assert(m_Globals.contains(location.Namespace), where, "undefined namespace {}", location.Namespace);
    Assert(m_Globals.at(location.Namespace).contains(location.Path), where, "undefined global {}", location);

    return m_Globals.at(location.Namespace).at(location.Path);
}
