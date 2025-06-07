#include <utility>
#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>
#include <mcc/statement.hpp>
#include <mcc/tree.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::DefineNode::DefineNode(
    const SourceLocation &where,
    ResourceLocation location,
    ParameterList parameters,
    TypePtr result_type,
    const bool throws,
    const std::vector<ResourceTag> &tags,
    StatementPtr body)
    : TreeNode(where),
      Location(std::move(location)),
      Parameters(std::move(parameters)),
      ResultType(std::move(result_type)),
      Throws(throws),
      Tags(tags),
      Body(std::move(body))
{
}

std::ostream &mcc::DefineNode::Print(std::ostream &stream) const
{
    stream << "define " << Location << '(';
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        stream << Parameters[i].Name << ": " << Parameters[i].Type;
    }
    stream << ')';
    if (ResultType)
        stream << ": " << ResultType;
    if (Throws)
        stream << " throws";
    stream << ' ';
    for (unsigned i = 0; i < Tags.size(); ++i)
    {
        if (i > 0)
            stream << ", ";
        stream << Tags[i];
    }
    if (!Tags.empty())
        stream << ' ';
    return Body->Print(stream);
}

void mcc::DefineNode::Generate(Builder &builder) const
{
    FunctionPtr function;

    if (!builder.HasFunction(Location))
        function = builder.CreateFunction(Where, Location, Parameters, ResultType, Throws);
    else
    {
        function = builder.GetFunction(Where, Location);
        Assert(!Body || function->Blocks.empty(), Where, "already implemented function {}", Location);
        Assert(function->ResultType == ResultType, Where, "cannot implement function with different result type");
        Assert(function->Throws == Throws, Where, "cannot implement function with different throw policy");
        Assert(
            function->Parameters.size() == Parameters.size(),
            Where,
            "cannot implement function with different parameter count");
        for (unsigned i = 0; i < Parameters.size(); ++i)
            Assert(
                function->Parameters[i].Type == Parameters[i].Type,
                Where,
                "cannot implement function with different parameter type for offset {}",
                i);
    }

    for (auto [tag_namespace_, tag_path_] : Tags)
    {
        if (tag_namespace_.empty())
            tag_namespace_ = builder.GetNamespace();

        auto &[replace_, tags_] = builder.GetPackage().Tags[tag_namespace_][tag_path_];
        tags_.emplace_back(function->Location);
    }

    if (!Body)
        return;

    builder.SetInsertBlock(Block::Create(Where, builder.GetContext(), function));

    builder.PushVariables();

    for (auto &[name_, type_] : function->Parameters)
        builder.InsertVariable(Where, name_, ArgumentValue::Create(Where, type_, name_));

    Frame target_frame;
    target_frame.ResultType = ResultType;
    Body->Generate(builder, target_frame);

    builder.PopVariables();

    for (auto &block : function->Blocks)
        if (auto terminator = block->GetTerminator())
        {
            if (const auto instruction = std::dynamic_pointer_cast<ReturnInstruction>(terminator))
            {
                auto type = instruction->Value ? instruction->Value->Type : builder.GetContext().GetVoid();
                Assert(type == ResultType, Where, "cannot return value of type {} for result type {}", type, ResultType);
            }
        }
        else if (ResultType->IsVoid())
        {
            builder.SetInsertBlock(block);
            (void) builder.CreateReturn(Where);
        }
        else
            Error(Where, "not all paths return a value");

    builder.SetInsertBlock(nullptr);
}

void mcc::DefineNode::GenerateInclude(Builder &builder, std::set<std::filesystem::path> &include_chain) const
{
    if (!builder.HasFunction(Location))
    {
        builder.CreateFunction(Where, Location, Parameters, ResultType, Throws);
        return;
    }

    const auto function = builder.GetFunction(Where, Location);
    Assert(function->ResultType == ResultType, Where, "cannot implement function with different result type");
    Assert(function->Throws == Throws, Where, "cannot implement function with different throw policy");
    Assert(
        function->Parameters.size() == Parameters.size(),
        Where,
        "cannot implement function with different parameter count");
    for (unsigned i = 0; i < Parameters.size(); ++i)
        Assert(
            function->Parameters[i].Type == Parameters[i].Type,
            Where,
            "cannot implement function with different parameter type for offset {}",
            i);
}
