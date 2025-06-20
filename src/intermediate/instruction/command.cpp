#include <utility>
#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::CommandInstruction::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const ResourceLocation &location,
    const CommandT &command)
{
    return std::make_shared<CommandInstruction>(where, type, location, command);
}

mcc::CommandInstruction::CommandInstruction(
    const SourceLocation &where,
    const TypePtr &type,
    ResourceLocation location,
    CommandT command)
    : Instruction(where, type, FieldType_Value),
      Location(std::move(location)),
      Command(std::move(command))
{
}

void mcc::CommandInstruction::Generate(CommandVector &commands, const bool stack) const
{
    if (!UseCount)
    {
        commands.Append(Command);
        return;
    }

    auto command = Command;
    const auto macro = command.front() == '$';
    if (macro)
        command.erase(command.begin());

    Assert(stack, Where, "command instruction with result requires stack");
    commands.Append(
        "{}execute store result storage {} {} long 1 run {}",
        macro ? "$" : "",
        Location,
        GetStackPath(),
        command);
}

bool mcc::CommandInstruction::RequireStack() const
{
    return UseCount;
}

mcc::Result mcc::CommandInstruction::GenerateResult() const
{
    return {
        .Type = ResultType_Reference,
        .ReferenceType = ReferenceType_Storage,
        .Target = Location.String(),
        .Path = GetStackPath(),
    };
}
