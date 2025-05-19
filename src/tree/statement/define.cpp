#include <mcc/builder.hpp>
#include <mcc/context.hpp>
#include <mcc/error.hpp>
#include <mcc/statement.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::DefineStatement::DefineStatement(
    const SourceLocation &where,
    const ResourceLocation &location,
    const ParameterList &parameters,
    const TypePtr &result,
    const bool throws,
    const std::vector<ResourceLocation> &tags,
    StatementPtr body)
    : Statement(where),
      Location(location),
      Parameters(parameters),
      Result(result),
      Throws(throws),
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

void mcc::DefineStatement::Generate(Builder &builder, const Frame &frame) const
{
    auto function = builder.GetFunction(Where, Location);
    if (!function)
        function = builder.CreateFunction(Where, Location, Parameters, Result ? Result : TypeContext::GetVoid());

    // TODO: assert that existing function has same parameters, result type and if it also throws

    auto &[package_, namespace_] = builder.GetContext();
    for (auto [tag_namespace_, tag_path_]: Tags)
    {
        if (tag_namespace_.empty())
            tag_namespace_ = namespace_;

        auto &[replace_, tags_] = package_.Tags[tag_namespace_][tag_path_];
        tags_.emplace_back(function->Location);
    }

    builder.SetInsertBlock(Block::Create(Where, function));

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

void mcc::DefineStatement::GenerateInclude(Builder &builder) const
{
    auto function = builder.GetFunction(Where, Location);
    if (!function)
        function = builder.CreateFunction(Where, Location, Parameters, Result ? Result : TypeContext::GetVoid());

    // TODO: assert that existing function has same parameters, result type and if it also throws
}
