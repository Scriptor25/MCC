#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

static void generate_function_call(
    const mcc::CallInstruction &self,
    mcc::CommandT &command,
    const bool use_stack)
{
    auto arguments = self.Arguments[0]->GenerateResult(false, use_stack);

    command += std::format("function {} ", self.Callee);

    switch (arguments.Type)
    {
        case mcc::ResultType_Value:
            command += arguments.Value;
            break;

        case mcc::ResultType_Storage:
            command += std::format("with storage {} {}", arguments.Location, arguments.Path);
            break;

        default:
            mcc::Error(
                "arguments must be {} or {}, but is {}",
                mcc::ResultType_Value,
                mcc::ResultType_Storage,
                arguments.Type);
    }
}

static void generate_builtin_print(
    const mcc::CallInstruction &self,
    mcc::CommandT &command,
    const bool use_stack)
{
    auto targets = self.Arguments[0]->GenerateResult(false, use_stack);
    auto message = self.Arguments[1]->GenerateResult(false, use_stack);

    command += std::format("tellraw {} ", targets.Value);

    switch (message.Type)
    {
        case mcc::ResultType_Value:
            command += message.Value;
            break;

        case mcc::ResultType_Storage:
            command += std::format("{{storage:\"{}\",nbt:\"{}\",interpret:true}}", message.Location, message.Path);
            break;

        case mcc::ResultType_Score:
            command += std::format("{{score:{{name:\"{}\",objective:\"{}\"}}}}", message.Player, message.Objective);
            break;

        default:
            mcc::Error(
                "message must be {}, {} or {}, but is {}",
                mcc::ResultType_Value,
                mcc::ResultType_Storage,
                mcc::ResultType_Score,
                message.Type);
    }
}

mcc::InstructionPtr mcc::CallInstruction::Create(
    ResourceLocation location,
    std::string callee,
    const bool builtin,
    std::vector<ValuePtr> arguments)
{
    return std::make_shared<CallInstruction>(std::move(location), std::move(callee), builtin, std::move(arguments));
}

mcc::CallInstruction::CallInstruction(
    ResourceLocation location,
    std::string callee,
    const bool builtin,
    std::vector<ValuePtr> arguments)
    : Location(std::move(location)),
      Callee(std::move(callee)),
      Builtin(builtin),
      Arguments(std::move(arguments))
{
    for (const auto &argument: Arguments)
        argument->Use();
}

mcc::CallInstruction::~CallInstruction()
{
    for (const auto &argument: Arguments)
        argument->Drop();
}

void mcc::CallInstruction::Generate(CommandVector &commands, const bool use_stack) const
{
    Assert(!UseCount || use_stack, "call instruction with result requires stack usage");

    CommandT command;
    if (!Builtin)
        generate_function_call(*this, command, use_stack);
    else if (Callee == "print")
        generate_builtin_print(*this, command, use_stack);
    else
        Error("undefined builtin callee {}", Callee);

    if (UseCount)
        command = std::format("execute store result storage {} {} double 1 run {}", Location, GetStackPath(), command);

    commands.Append(std::move(command));
}

mcc::CommandT mcc::CallInstruction::GenerateInline(const bool use_stack) const
{
    Assert(!UseCount || use_stack, "call instruction with result requires stack usage");

    CommandT command;
    if (!Builtin)
        generate_function_call(*this, command, use_stack);
    else if (Callee == "print")
        generate_builtin_print(*this, command, use_stack);
    else
        Error("undefined builtin callee {}", Callee);

    if (!UseCount)
        return command;

    return std::format("execute store result storage {} {} double 1 run {}", Location, GetStackPath(), command);
}

mcc::Result mcc::CallInstruction::GenerateResult(const bool stringify, const bool use_stack) const
{
    if (!UseCount)
        return {.Type = ResultType_None};

    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = GetStackPath(),
    };
}

bool mcc::CallInstruction::RequireStack() const
{
    if (UseCount)
        return true;

    for (auto &argument: Arguments)
        if (argument->RequireStack())
            return true;

    return false;
}
