#include <utility>
#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/statement.hpp>
#include <mcc/tree.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::DefineNode::DefineNode(
    const SourceLocation &where,
    ResourceLocation location,
    ParameterList parameters,
    TypePtr result,
    const bool throws,
    const std::vector<ResourceLocation> &tags,
    StatementPtr body)
    : TreeNode(where),
      Location(std::move(location)),
      Parameters(std::move(parameters)),
      Result(std::move(result)),
      Throws(throws),
      Tags(tags),
      Body(std::move(body))
{
}

std::ostream &mcc::DefineNode::Print(std::ostream &stream) const
{
    Location.Print(stream << "define ") << '(';
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        stream << Parameters[i].Name << ": " << Parameters[i].Type;
    }
    stream << ')';
    if (Result)
        stream << ": " << Result;
    if (Throws)
        stream << " throws";
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

void mcc::DefineNode::Generate(Builder &builder) const
{
    const auto function = builder.HasFunction(Location)
                              ? builder.GetFunction(Where, Location)
                              : builder.CreateFunction(
                                  Where,
                                  Location,
                                  Parameters,
                                  Result ? Result : builder.GetContext().GetVoid(),
                                  Throws);

    // TODO: assert that existing function has same parameters, result type and if it also throws

    for (auto [tag_namespace_, tag_path_]: Tags)
    {
        if (tag_namespace_.empty())
            tag_namespace_ = builder.GetNamespace();

        auto &[replace_, tags_] = builder.GetPackage().Tags[tag_namespace_][tag_path_];
        tags_.emplace_back(function->Location);
    }

    builder.SetInsertBlock(Block::Create(Where, builder.GetContext(), function));

    builder.PushVariables();
    for (auto &[name_, value_]: function->Parameters)
        builder.CreateVariable(Where, value_->Type, name_);
    Body->Generate(builder, {});
    builder.PopVariables();

    for (auto &block: function->Blocks)
        if (!block->GetTerminator())
        {
            builder.SetInsertBlock(block);
            (void) builder.CreateReturnVoid(Where);
        }

    builder.SetInsertBlock(nullptr);
}

void mcc::DefineNode::GenerateInclude(Builder &builder, std::set<std::filesystem::path>& include_chain) const
{
    if (!builder.HasFunction(Location))
    {
        builder.CreateFunction(
            Where,
            Location,
            Parameters,
            Result ? Result : builder.GetContext().GetVoid(),
            Throws);
        return;
    }

    const auto function = builder.GetFunction(Where, Location);

    // TODO: assert that existing function has same parameters, result type and if it also throws
}
