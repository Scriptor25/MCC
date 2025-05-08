#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::CommandInstruction::Create(ResourceLocation location, CommandT command)
{
    return std::make_shared<CommandInstruction>(std::move(location), std::move(command));
}

mcc::CommandInstruction::CommandInstruction(ResourceLocation location, CommandT command)
    : Location(std::move(location)),
      Command(std::move(command))
{
}

void mcc::CommandInstruction::Generate(CommandVector &commands) const
{
    if (!UseCount)
    {
        commands.Append(Command);
        return;
    }

    commands.Append(
        "execute store result storage {} {} double 1 run {}",
        Location,
        GetStackPath(),
        Command);
}

mcc::CommandT mcc::CommandInstruction::GenerateInline() const
{
    if (!UseCount)
        return Command;

    return std::format(
        "execute store result storage {} {} double 1 run {}",
        Location,
        GetStackPath(),
        Command);
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
