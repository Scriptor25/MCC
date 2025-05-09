#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

static void generate_function_call(
    const mcc::CallInstruction &self,
    mcc::CommandT &command)
{
    auto arguments = self.Arguments[0]->GenerateResult(false);

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
    mcc::CommandT &command)
{
    auto targets = self.Arguments[0]->GenerateResult(false);
    auto message = self.Arguments[1]->GenerateResult(false);

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

void mcc::CallInstruction::Generate(CommandVector &commands) const
{
    CommandT command;
    if (!Builtin)
        generate_function_call(*this, command);
    else if (Callee == "print")
        generate_builtin_print(*this, command);
    else
        Error("undefined builtin callee {}", Callee);

    if (UseCount)
        command = std::format("execute store result storage {} {} double 1 run {}", Location, GetStackPath(), command);

    commands.Append(std::move(command));
}

mcc::Result mcc::CallInstruction::GenerateResult(const bool stringify) const
{
    if (!UseCount)
        return {.Type = ResultType_None};

    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = GetStackPath(),
    };
}
