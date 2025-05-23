#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::CommandInstruction::Create(
    const SourceLocation &where,
    TypeContext &context,
    const TypePtr &type,
    const ResourceLocation &location,
    const CommandT &command)
{
    return std::make_shared<CommandInstruction>(where, context, type, location, command);
}

mcc::CommandInstruction::CommandInstruction(
    const SourceLocation &where,
    TypeContext &context,
    const TypePtr &type,
    const ResourceLocation &location,
    const CommandT &command)
    : Instruction(where, context, type),
      Location(location),
      Command(command)
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
        "{}execute store result storage {} {} double 1 run {}",
        macro ? "$" : "",
        Location,
        GetStackPath(),
        command);
}

bool mcc::CommandInstruction::RequireStack() const
{
    return UseCount;
}

mcc::Result mcc::CommandInstruction::GenerateResult(const bool stringify) const
{
    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = GetStackPath(),
    };
}
