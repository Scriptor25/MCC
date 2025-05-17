#include <mcc/builder.hpp>
#include <mcc/context.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::DefineStatement::DefineStatement(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ParameterList &parameters,
    TypePtr type,
    const std::vector<ResourceLocation> &tags,
    StatementPtr body)
    : Statement(where),
      Location(location),
      Parameters(parameters),
      Type(type),
      Tags(tags),
      Body(std::move(body))
{
}

std::ostream &mcc::DefineStatement::Print(std::ostream &stream) const
{
    Location.Print(stream << "define ") << '(';
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        stream << Parameters[i].Name << ": " << Parameters[i].Type;
    }
    stream << ')';
    if (Type)
        stream << ": " << Type;
    stream << ' ';
    for (unsigned i = 0; i < Tags.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        Tags[i].Print(stream << '#');
    }
    if (!Tags.empty())
        stream << ' ';
    return Body->Print(stream);
}

void mcc::DefineStatement::Generate(Builder &builder, const Frame &frame) const
{
    const auto block = builder.CreateFunction(Where, Type ? Type : TypeContext::GetVoid(), Location);

    const auto &[pkg_, namespace_] = builder.GetContext();
    for (auto tag: Tags)
    {
        if (tag.Namespace.empty())
            tag.Namespace = namespace_;

        auto &[replace_, values_] = pkg_.Tags[tag];
        values_.emplace_back(block->Location);
    }

    builder.SetInsertBlock(block);

    builder.PushVariables();
    for (const auto &[name_, type_]: Parameters)
        block->Parameters.emplace_back(name_, builder.CreateVariable(Where, type_, name_));
    Body->Generate(builder, {});
    builder.PopVariables();

    if (!block->GetTerminator())
    {
        builder.SetInsertBlock(block);
        (void) builder.CreateReturnVoid(Where);
    }

    for (const auto &child: block->Children)
        if (!child->GetTerminator())
        {
            builder.SetInsertBlock(child);
            (void) builder.CreateReturnVoid(Where);
        }

    builder.SetInsertBlock(nullptr);
}
