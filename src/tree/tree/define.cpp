#include <utility>
#include <mcc/builder.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
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
        stream << Parameters.at(i).Name << ": " << Parameters.at(i).Type;
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
        Tags.at(i).Print(stream << '#');
    }
    if (!Tags.empty())
        stream << ' ';
    return Body->Print(stream);
}

void mcc::DefineNode::Generate(Builder &builder) const
{
    FunctionPtr function;

    if (!builder.HasFunction(Location))
    {
        function = builder.CreateFunction(Where, Location, Parameters, Result, Throws);
    }
    else
    {
        function = builder.GetFunction(Where, Location);
        Assert(!Body || function->Blocks.empty(), Where, "already implemented function {}", Location);
        Assert(function->Result == Result, Where, "cannot implement function with different result type");
        Assert(function->Throws == Throws, Where, "cannot implement function with different throw policy");
        Assert(
            function->Parameters.size() == Parameters.size(),
            Where,
            "cannot implement function with different parameter count");
        for (unsigned i = 0; i < Parameters.size(); ++i)
        {
            Assert(
                function->Parameters.at(i).Type == Parameters.at(i).Type,
                Where,
                "cannot implement function with different parameter type for offset {}",
                i);
        }
    }

    for (auto [tag_namespace_, tag_path_]: Tags)
    {
        if (tag_namespace_.empty())
            tag_namespace_ = builder.GetNamespace();

        auto &[replace_, tags_] = builder.GetPackage().Tags[tag_namespace_][tag_path_];
        tags_.emplace_back(function->Location);
    }

    if (!Body)
    {
        return;
    }

    builder.SetInsertBlock(Block::Create(Where, builder.GetContext(), function));

    builder.PushVariables();

    for (auto &[name_, type_]: function->Parameters)
    {
        builder.InsertVariable(Where, name_, ArgumentValue::Create(Where, type_, name_));
    }

    Frame target_frame;
    Body->Generate(builder, target_frame);

    builder.PopVariables();

    auto result = Result;
    for (auto &block: function->Blocks)
    {
        if (auto terminator = block->GetTerminator())
        {
            if (const auto instruction = std::dynamic_pointer_cast<ReturnInstruction>(terminator))
            {
                auto type = instruction->Value ? instruction->Value->Type : builder.GetContext().GetVoid();
                Assert(
                    type == result,
                    Where,
                    "cannot return value of type {} for result type {}",
                    type,
                    result);
            }
        }
        else if (result == builder.GetContext().GetVoid())
        {
            builder.SetInsertBlock(block);
            (void) builder.CreateReturnVoid(Where);
        }
        else
        {
            Error(Where, "not all paths return");
        }
    }

    builder.SetInsertBlock(nullptr);
}

void mcc::DefineNode::GenerateInclude(Builder &builder, std::set<std::filesystem::path> &include_chain) const
{
    if (!builder.HasFunction(Location))
    {
        builder.CreateFunction(Where, Location, Parameters, Result, Throws);
        return;
    }

    const auto function = builder.GetFunction(Where, Location);
    Assert(function->Result == Result, Where, "cannot implement function with different result type");
    Assert(function->Throws == Throws, Where, "cannot implement function with different throw policy");
    Assert(
        function->Parameters.size() == Parameters.size(),
        Where,
        "cannot implement function with different parameter count");
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        Assert(
            function->Parameters.at(i).Type == Parameters.at(i).Type,
            Where,
            "cannot implement function with different parameter type for offset {}",
            i);
    }
}
