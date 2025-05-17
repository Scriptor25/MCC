#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::CommandInstruction::Create(
    const SourceLocation &where,
    const ResourceLocation &location,
    const CommandT &command)
{
    return std::make_shared<CommandInstruction>(where, location, command);
}

mcc::CommandInstruction::CommandInstruction(
    const SourceLocation &where,
    const ResourceLocation &location,
    const CommandT &command)
    : Instruction(where, TypeID_Any),
      Location(location),
      Command(command)
{
}

void mcc::CommandInstruction::Generate(CommandVector &commands, bool stack) const
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
        "{}execute store result storage {} {} double 1 run {}",
        macro ? "$" : "",
        Location,
        GetStackPath(),
        command);
}

bool mcc::CommandInstruction::RequireStack() const
{
    return !!UseCount;
}

mcc::Result mcc::CommandInstruction::GenerateResult(const bool stringify) const
{
    if (!UseCount)
        return {.Type = ResultType_None};

    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = GetStackPath(),
    };
}
