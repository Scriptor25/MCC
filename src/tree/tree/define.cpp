#include <mcc/block.hpp>
#include <mcc/builder.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>
#include <mcc/statement.hpp>
#include <mcc/tree.hpp>
#include <mcc/type.hpp>
#include <mcc/value.hpp>

mcc::DefineNode::DefineNode(
        const SourceLocation &where,
        const bool is_operator,
        ResourceLocation location,
        ParameterList parameters,
        TypePtr result_type,
        const bool throws,
        const std::vector<ResourceTag> &tags,
        StatementPtr body)
    : TreeNode(where),
      Location(std::move(location)),
      IsOperator(is_operator),
      Parameters(std::move(parameters)),
      ResultType(std::move(result_type)),
      Throws(throws),
      Tags(tags),
      Body(std::move(body))
{
}

std::ostream &mcc::DefineNode::Print(std::ostream &stream) const
{
    stream << "define " << (IsOperator ? "operator" : "") << Location << '(';
    for (unsigned i = 0; i < Parameters.size(); ++i)
    {
        if (i)
            stream << ", ";
        switch (Parameters[i].FieldType)
        {
        case FieldType_::ImmutableReference:
            stream << "const ";
        case FieldType_::MutableReference:
            stream << "&";
        default:
            break;
        }
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
        if (i)
            stream << ", ";
        stream << Tags[i];
    }
    if (!Tags.empty())
        stream << ' ';
    return Body->Print(stream);
}

void mcc::DefineNode::Generate(Builder &builder) const
{
    auto function = builder.GetFunction(Location, Parameters);
    if (!function)
        function = builder.CreateFunction(Where, Location, Parameters, ResultType, Throws);
    else
        Check(function);

    for (auto [tag_namespace_, tag_path_] : Tags)
    {
        if (tag_namespace_.empty())
            tag_namespace_ = builder.GetNamespace();

        auto &[replace_, tags_] = builder.GetPackage().Tags[tag_namespace_][tag_path_];
        tags_.emplace_back(function->Mangle());
    }

    if (!Body)
        return;

    const auto entry_target = Block::Create(Body->Where, "entry", builder.GetContext(), function);
    builder.SetInsertBlock(entry_target);

    builder.PushVariables();

    for (auto &[name_, type_, field_type_] : function->Parameters)
    {
        ValuePtr value;
        switch (field_type_)
        {
        case FieldType_::Value:
            value = ArgumentValue::Create(Where, name_, type_);
            break;
        case FieldType_::MutableReference:
            value = GenericStorageReference::Create(
                    Where,
                    name_,
                    type_,
                    ArgumentValue::Create(Where, name_ + "_target", {}),
                    ArgumentValue::Create(Where, name_ + "_path", {}),
                    true);
            break;
        case FieldType_::ImmutableReference:
            value = GenericStorageReference::Create(
                    Where,
                    name_,
                    type_,
                    ArgumentValue::Create(Where, name_ + "_target", {}),
                    ArgumentValue::Create(Where, name_ + "_path", {}),
                    false);
            break;
        }
        builder.InsertVariable(Where, name_, value);
    }

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
                Assert(SameOrSpecialization(type, ResultType),
                       Where,
                       "cannot return value of type {} for result type {}",
                       type,
                       ResultType);
            }
        }
        else if (ResultType->IsVoid())
        {
            builder.SetInsertBlock(block);
            (void) builder.CreateReturn(Where, {});
        }
        else
            Error(Where, "not all paths return a value");

    builder.SetInsertBlock(nullptr);
}

void mcc::DefineNode::GenerateInclude(
        Builder &builder,
        std::set<std::filesystem::path> &include_chain) const
{
    auto function = builder.GetFunction(Location, Parameters);
    if (!function)
    {
        builder.CreateFunction(Where, Location, Parameters, ResultType, Throws);
        return;
    }

    Check(function);
}

void mcc::DefineNode::Check(const FunctionPtr &function) const
{
    Assert(function->ResultType == ResultType, Where, "cannot implement function with different result type");
    Assert(function->Throws == Throws, Where, "cannot implement function with different throw policy");
}
