#include <mcc/command.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::CommandInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        const ResourceLocation &location,
        const CommandT &command)
{
    auto self  = std::make_shared<CommandInstruction>(where, name, type, location, command);
    self->Self = self;
    return self;
}

mcc::CommandInstruction::CommandInstruction(
        const SourceLocation &where,
        const std::string &name,
        const TypePtr &type,
        ResourceLocation location,
        CommandT command)
    : Instruction(
              where,
              name,
              type,
              FieldType_::Value),
      Location(std::move(location)),
      Command(std::move(command))
{
}

void mcc::CommandInstruction::Generate(
        CommandVector &commands,
        const bool stack) const
{
    if (Uses.empty())
    {
        commands.Append(Command);
        return;
    }

    auto command     = Command;
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
    return !Uses.empty();
}

mcc::Result mcc::CommandInstruction::GenerateResult() const
{
    return {
        .Type          = ResultType_::Reference,
        .ReferenceType = ReferenceType_::Storage,
        .Target        = Location.String(),
        .Path          = GetStackPath(),
    };
}
