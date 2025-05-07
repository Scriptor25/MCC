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

void mcc::CommandInstruction::Generate(CommandVector &commands, const bool use_stack) const
{
    Assert(!UseCount || use_stack, "command instruction with result requires stack usage");

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

mcc::CommandT mcc::CommandInstruction::GenerateInline(const bool use_stack) const
{
    Assert(!UseCount || use_stack, "command instruction with result requires stack usage");

    if (!UseCount)
        return Command;

    return std::format(
        "execute store result storage {} {} double 1 run {}",
        Location,
        GetStackPath(),
        Command);
}

mcc::Result mcc::CommandInstruction::GenerateResult(const bool stringify, const bool use_stack) const
{
    if (!UseCount)
        return {.Type = ResultType_None};

    Assert(use_stack, "command instruction with result requires stack usage");

    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = GetStackPath(),
    };
}

bool mcc::CommandInstruction::RequireStack() const
{
    return !!UseCount;
}
