#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::CallInstruction::Create(
    ResourceLocation location,
    CalleeE callee,
    std::vector<ValuePtr> arguments)
{
    return std::make_shared<CallInstruction>(std::move(location), callee, std::move(arguments));
}

mcc::CallInstruction::CallInstruction(ResourceLocation location, const CalleeE callee, std::vector<ValuePtr> arguments)
    : Location(std::move(location)),
      Callee(callee),
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

static void generate_generic(
    mcc::CommandT &command,
    const mcc::CalleeE callee,
    const std::vector<mcc::ValuePtr> &arguments,
    const bool use_stack)
{
    command += mcc::ToString(callee);
    for (auto &argument: arguments)
    {
        auto value = argument->GenerateResult(false, use_stack);
        mcc::Assert(
            value.Type == mcc::ResultType_Value,
            "argument must be {}, but is {}",
            mcc::ResultType_Value,
            value.Type);
        command += ' ' + value.Value;
    }
}

static void generate_function(
    mcc::CommandT &command,
    const mcc::CalleeE,
    const std::vector<mcc::ValuePtr> &arguments,
    const bool use_stack)
{
    auto callee_ = arguments[0]->GenerateResult(false, use_stack);
    auto arguments_ = arguments[1]->GenerateResult(false, use_stack);

    mcc::Assert(
        callee_.Type == mcc::ResultType_Value,
        "callee must be {}, but is {}",
        mcc::ResultType_Value,
        callee_.Type);

    command += std::format("function {} ", callee_.Value);

    switch (arguments_.Type)
    {
        case mcc::ResultType_Value:
            command += arguments_.Value;
            break;

        case mcc::ResultType_Storage:
            command += std::format("with storage {} {}", arguments_.Location, arguments_.Path);
            break;

        default:
            mcc::Error(
                "arguments must be {} or {}, but is {}",
                mcc::ResultType_Value,
                mcc::ResultType_Storage,
                arguments_.Type);
    }
}

static void generate_tellraw(
    mcc::CommandT &command,
    const mcc::CalleeE,
    const std::vector<mcc::ValuePtr> &arguments,
    const bool use_stack)
{
    auto targets = arguments[0]->GenerateResult(false, use_stack);
    auto message = arguments[1]->GenerateResult(false, use_stack);

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

using GeneratorT = std::function<void(
    mcc::CommandT &command,
    mcc::CalleeE callee,
    const std::vector<mcc::ValuePtr> &arguments,
    bool use_stack)>;

struct GeneratorInfo
{
    GeneratorT Generator;
    bool RequireStack;
    bool Inlinable;
};

static const std::map<mcc::CalleeE, GeneratorInfo> generator_map
{
    {mcc::Callee_Give, {generate_generic, false, true}},
    {mcc::Callee_SetBlock, {generate_generic, false, true}},

    {mcc::Callee_Function, {generate_function, false, true}},
    {mcc::Callee_TellRaw, {generate_tellraw, false, true}},
};

void mcc::CallInstruction::Generate(CommandVector &commands, const bool use_stack) const
{
    Assert(!UseCount || use_stack, "call instruction requires stack usage");

    Assert(generator_map.contains(Callee), "no generator for call to {}", Callee);
    auto &[generator_, require_stack_, inlinable_] = generator_map.at(Callee);
    Assert(use_stack || !require_stack_, "cannot generate stackless call to {}", Callee);

    CommandT command;
    generator_(command, Callee, Arguments, use_stack);

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

mcc::CommandT mcc::CallInstruction::GenerateInline(const bool use_stack) const
{
    Assert(!UseCount || use_stack, "call instruction requires stack usage");

    Assert(generator_map.contains(Callee), "no generator for call to {}", Callee);
    auto &[generator_, require_stack_, inlinable_] = generator_map.at(Callee);
    Assert(use_stack || !require_stack_, "cannot generate stackless call to {}", Callee);
    Assert(inlinable_, "cannot generate inline call to {}", Callee);

    CommandT command;
    generator_(command, Callee, Arguments, use_stack);

    if (!UseCount)
        return command;

    return std::format(
        "execute store result storage {} {} double 1 run {}",
        Location,
        GetStackPath(),
        command);
}

mcc::Result mcc::CallInstruction::GenerateResult(const bool stringify, const bool use_stack) const
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

bool mcc::CallInstruction::RequireStack() const
{
    if (UseCount)
        return true;

    Assert(generator_map.contains(Callee), "no generator for call to {}", Callee);
    auto &[generator_, require_stack_, inlinable_] = generator_map.at(Callee);
    return require_stack_;
}
