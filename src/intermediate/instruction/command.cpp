#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::CommandInstruction::Create(
    ResourceLocation location,
    CommandT command,
    std::vector<ValuePtr> arguments)
{
    return std::make_shared<CommandInstruction>(std::move(location), std::move(command), std::move(arguments));
}

mcc::CommandInstruction::CommandInstruction(
    ResourceLocation location,
    CommandT command,
    std::vector<ValuePtr> arguments)
    : Location(std::move(location)),
      Command(std::move(command)),
      Arguments(std::move(arguments))
{
    for (const auto &argument: Arguments)
        argument->Use();
}

mcc::CommandInstruction::~CommandInstruction()
{
    for (const auto &argument: Arguments)
        argument->Drop();
}

void mcc::CommandInstruction::Generate(CommandVector &commands, const bool use_stack) const
{
    Assert(!UseCount || use_stack, "command instruction with result requires stack usage");

    auto command = Command;
    for (auto &argument: Arguments)
    {
        auto value = argument->GenerateResult(false, use_stack);
        Assert(
            value.Type == ResultType_Value,
            "argument must be {}, but is {}",
            ResultType_Value,
            value.Type);
        command += ' ' + value.Value;
    }

    if (!UseCount)
    {
        commands.Append(command);
        return;
    }

    commands.Append(
        "execute store result storage {} {} double 1 run {}",
        Location,
        GetStackPath(),
        command);
}

mcc::CommandT mcc::CommandInstruction::GenerateInline(const bool use_stack) const
{
    Assert(!UseCount || use_stack, "command instruction with result requires stack usage");

    auto command = Command;
    for (auto &argument: Arguments)
    {
        auto value = argument->GenerateResult(false, use_stack);
        Assert(
            value.Type == ResultType_Value,
            "argument must be {}, but is {}",
            ResultType_Value,
            value.Type);
        command += ' ' + value.Value;
    }

    if (!UseCount)
        return command;

    return std::format(
        "execute store result storage {} {} double 1 run {}",
        Location,
        GetStackPath(),
        command);
}

mcc::Result mcc::CommandInstruction::GenerateResult(const bool stringify, const bool use_stack) const
{
    if (!UseCount)
        return {.Type = ResultType_None};

    Assert(use_stack, "call instruction requires stack usage");

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
