#include <utility>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

mcc::InstructionPtr mcc::NotNullInstruction::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const ValuePtr &value)
{
    return std::make_shared<NotNullInstruction>(where, context, location, value);
}

mcc::NotNullInstruction::NotNullInstruction(
    const SourceLocation &where,
    TypeContext &context,
    ResourceLocation location,
    ValuePtr value)
    : Instruction(where, context.GetBoolean(), false),
      Location(std::move(location)),
      Value(std::move(value))
{
    Value->Use();
}

mcc::NotNullInstruction::~NotNullInstruction()
{
    Value->Drop();
}

void mcc::NotNullInstruction::Generate(CommandVector &commands, bool stack) const
{
    auto value = Value->GenerateResult(false);
    Assert(value.Type == ResultType_Storage, Where, "value must be {}, but is {}", ResultType_Storage, value.Type);

    auto stack_path = GetStackPath();

    commands.Append("data remove storage {} {}", Location, stack_path);
    commands.Append(
        "execute if data storage {} {} run data modify storage {} {} set value 1",
        value.Location,
        value.Path,
        Location,
        stack_path);
}

bool mcc::NotNullInstruction::RequireStack() const
{
    return UseCount || Value->RequireStack();
}

mcc::Result mcc::NotNullInstruction::GenerateResult(bool stringify) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = GetStackPath(),
    };
}
