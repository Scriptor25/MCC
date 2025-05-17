#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

static void generate_function_call(const mcc::CallInstruction &self, mcc::CommandVector &commands)
{
    auto arguments = self.Arguments[0]->GenerateResult(false);

    std::string arguments_value;
    switch (arguments.Type)
    {
        case mcc::ResultType_Value:
            arguments_value = arguments.Value;
            break;

        case mcc::ResultType_Storage:
            arguments_value = std::format("with storage {} {}", arguments.Location, arguments.Path);
            break;

        default:
            mcc::Error(
                "arguments must be {} or {}, but is {}",
                mcc::ResultType_Value,
                mcc::ResultType_Storage,
                arguments.Type);
    }

    if (!self.UseCount)
    {
        commands.Append("function {} {}", self.Callee, arguments_value);
        return;
    }

    commands.Append(
        "execute store result storage {} {} double 1 run function {} {}",
        self.Location,
        self.GetStackPath(),
        self.Callee,
        arguments_value);
}

static void generate_builtin_print(const mcc::CallInstruction &self, mcc::CommandVector &commands)
{
    auto targets = self.Arguments[0]->GenerateResult(false);
    auto message = self.Arguments[1]->GenerateResult(false);

    std::string message_value;
    switch (message.Type)
    {
        case mcc::ResultType_Value:
            message_value = message.Value;
            break;

        case mcc::ResultType_Storage:
            message_value = std::format(
                "{{storage:\"{}\",nbt:\"{}\",interpret:true}}",
                message.Location,
                message.Path);
            break;

        case mcc::ResultType_Score:
            message_value = std::format(
                "{{score:{{name:\"{}\",objective:\"{}\"}}}}",
                message.Player,
                message.Objective);
            break;

        default:
            mcc::Error(
                "message must be {}, {} or {}, but is {}",
                mcc::ResultType_Value,
                mcc::ResultType_Storage,
                mcc::ResultType_Score,
                message.Type);
    }

    if (!self.UseCount)
    {
        commands.Append("tellraw {} {}", targets.Value, message_value);
        return;
    }

    commands.Append(
        "execute store result storage {} {} double 1 run tellraw {} {}",
        self.Location,
        self.GetStackPath(),
        targets.Value,
        message_value);
}

static void generate_builtin_swap(const mcc::CallInstruction &self, mcc::CommandVector &commands)
{
    auto value1 = self.Arguments[0]->GenerateResult(false);
    auto value2 = self.Arguments[1]->GenerateResult(false);

    // store value 1 in tmp
    // store value 2 in value 1
    // store tmp in value 2

    auto tmp = self.GetTmpName();

    switch (value1.Type)
    {
        case mcc::ResultType_Storage:
            commands.Append(
                "data modify storage {} {} set from storage {} {}",
                self.Location,
                tmp,
                value1.Location,
                value1.Path);
            switch (value2.Type)
            {
                case mcc::ResultType_Storage:
                    commands.Append(
                        "data modify storage {} {} set from storage {} {}",
                        value1.Location,
                        value1.Path,
                        value2.Location,
                        value2.Path);
                    break;

                case mcc::ResultType_Score:
                    commands.Append(
                        "execute store result storage {} {} double 1 run scoreboard players get {} {}",
                        value1.Location,
                        value1.Path,
                        value2.Player,
                        value2.Objective);
                    break;

                default:
                    mcc::Error(
                        "value 2 must be {} or {}, but is {}",
                        mcc::ResultType_Storage,
                        mcc::ResultType_Score,
                        value2.Type);
            }
            break;

        case mcc::ResultType_Score:
            commands.Append(
                "execute store result storage {} {} double 1 run scoreboard players get {} {}",
                self.Location,
                tmp,
                value1.Player,
                value1.Objective);
            switch (value2.Type)
            {
                case mcc::ResultType_Storage:
                    commands.Append(
                        "execute store result score {} {} run data get storage {} {}",
                        value1.Player,
                        value1.Objective,
                        value2.Location,
                        value2.Path);
                    break;

                case mcc::ResultType_Score:
                    commands.Append(
                        "scoreboard players operation {} {} = {} {}",
                        value1.Player,
                        value1.Objective,
                        value2.Player,
                        value2.Objective);
                    break;

                default:
                    mcc::Error(
                        "value 2 must be {} or {}, but is {}",
                        mcc::ResultType_Storage,
                        mcc::ResultType_Score,
                        value2.Type);
            }
            break;

        default:
            mcc::Error(
                "value 1 must be {} or {}, but is {}",
                mcc::ResultType_Storage,
                mcc::ResultType_Score,
                value1.Type);
    }

    switch (value2.Type)
    {
        case mcc::ResultType_Storage:
            commands.Append(
                "data modify storage {} {} set from storage {} {}",
                value2.Location,
                value2.Path,
                self.Location,
                tmp);
            break;

        case mcc::ResultType_Score:
            commands.Append(
                "execute store result score {} {} run data get storage {} {}",
                value2.Player,
                value2.Objective,
                self.Location,
                tmp);
            break;

        default:
            mcc::Error(
                "value 2 must be {} or {}, but is {}",
                mcc::ResultType_Storage,
                mcc::ResultType_Score,
                value2.Type);
    }

    commands.Append("data remove storage {} {}", self.Location, tmp);

    if (!self.UseCount)
        return;

    commands.Append("data modify storage {} {} set value 0", self.Location, self.GetStackPath());
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

void mcc::CallInstruction::Generate(CommandVector &commands, const bool stack) const
{
    Assert(!UseCount || stack, "call instruction with result requires stack");

    if (!Builtin)
        generate_function_call(*this, commands);
    else if (Callee == "print")
        generate_builtin_print(*this, commands);
    else if (Callee == "swap")
        generate_builtin_swap(*this, commands);
    else
        Error("undefined builtin callee {}", Callee);
}

bool mcc::CallInstruction::RequireStack() const
{
    return UseCount
           || std::ranges::any_of(
               Arguments,
               [](auto &argument)
               {
                   return argument->RequireStack();
               });
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
