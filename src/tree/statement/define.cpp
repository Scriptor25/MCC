#include <mcc/builder.hpp>
#include <mcc/context.hpp>
#include <mcc/intermediate.hpp>
#include <mcc/tree.hpp>

mcc::DefineStatement::DefineStatement(
    SourceLocation where,
    ResourceLocation location,
    std::vector<std::string> parameters,
    std::vector<ResourceLocation> tags,
    StatementPtr body)
    : Statement(std::move(where)),
      Location(std::move(location)),
      Parameters(std::move(parameters)),
      Tags(std::move(tags)),
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
        stream << Parameters[i];
    }
    stream << ") ";
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

void mcc::DefineStatement::Generate(Builder &builder) const
{
    const auto block = builder.CreateFunction(Location, Parameters);

    const auto &[pkg_, namespace_] = builder.GetContext();
    for (auto tag: Tags)
    {
        if (tag.Namespace.empty())
            tag.Namespace = namespace_;

        auto &[replace_, values_] = pkg_.Tags[tag];
        values_.emplace_back(block->Location);
    }

    builder.SetInsertBlock(block);
    Body->Generate(builder);

    if (!block->GetTerminator())
    {
        builder.SetInsertBlock(block);
        (void) builder.CreateReturn(nullptr);
    }

    for (const auto &child: block->Children)
        if (!child->GetTerminator())
        {
            builder.SetInsertBlock(child);
            (void) builder.CreateReturn(nullptr);
        }

    builder.SetInsertBlock(nullptr);
}
