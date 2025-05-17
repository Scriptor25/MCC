#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::CommandInstruction::Create(ResourceLocation location, CommandT command)
{
    return std::make_shared<CommandInstruction>(std::move(location), std::move(command));
}

mcc::CommandInstruction::CommandInstruction(ResourceLocation location, CommandT command)
    : Location(std::move(location)),
      Command(std::move(command))
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

    Assert(stack, "command instruction with result requires stack");
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
