#include <set>
#include <mcc/builder.hpp>
#include <mcc/context.hpp>
#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

using namespace std::string_view_literals;

using GeneratorT = std::function<void(const mcc::CommandInstruction &self, mcc::CommandT &command, bool use_stack)>;

struct GeneratorInfo
{
    GeneratorT Generator;
    bool RequireStack;
    bool Inlinable;
};

static void generate_generic(
    const mcc::CommandInstruction &self,
    mcc::CommandT &command,
    const bool use_stack)
{
    command += self.Path.front();
    for (auto &argument: self.Arguments)
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
    const mcc::CommandInstruction &self,
    mcc::CommandT &command,
    const bool use_stack)
{
    auto callee = self.Arguments[0]->GenerateResult(false, use_stack);
    auto arguments = self.Arguments[1]->GenerateResult(false, use_stack);

    mcc::Assert(
        callee.Type == mcc::ResultType_Value,
        "callee must be {}, but is {}",
        mcc::ResultType_Value,
        callee.Type);

    command += std::format("function {} ", callee.Value);

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

static void generate_tellraw(
    const mcc::CommandInstruction &self,
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

static const std::set generic_set
{
    "give"sv,
    "setblock"sv,
};

static const std::map<std::string_view, GeneratorInfo> generator_map
{
    {"function"sv, {generate_function, false, true}},
    {"tellraw"sv, {generate_tellraw, false, true}},
};

mcc::InstructionPtr mcc::CommandInstruction::Create(
    ResourceLocation location,
    std::vector<std::string> path,
    std::vector<ValuePtr> arguments)
{
    return std::make_shared<CommandInstruction>(std::move(location), std::move(path), std::move(arguments));
}

mcc::CommandInstruction::CommandInstruction(
    ResourceLocation location,
    std::vector<std::string> path,
    std::vector<ValuePtr> arguments)
    : Location(std::move(location)),
      Path(std::move(path)),
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

static const GeneratorInfo &get_generator(const bool inline_, const bool use_stack, const std::string &base)
{
    mcc::Assert(generator_map.contains(base), "no generator for call to {}", base);

    auto &generator = generator_map.at(base);

    mcc::Assert(use_stack || !generator.RequireStack, "cannot generate stackless call to {}", base);
    mcc::Assert(!inline_ || generator.Inlinable, "cannot generate inline call to {}", base);

    return generator;
}

void mcc::CommandInstruction::Generate(const Builder &builder, CommandVector &commands, const bool use_stack) const
{
    Assert(!UseCount || use_stack, "call instruction requires stack usage");

    auto &base = Path.front();
    auto &definitions = builder.GetContext().Definitions;
    Assert(definitions.contains(base), "undefined command base {}", base);

    auto &definition = definitions.at(base);

    CommandT command;
    if (Path.size() == 1 && generic_set.contains(base))
        generate_generic(*this, command, use_stack);
    else
        get_generator(false, use_stack, base).Generator(*this, command, use_stack);

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

mcc::CommandT mcc::CommandInstruction::GenerateInline(const Builder &builder, const bool use_stack) const
{
    Assert(!UseCount || use_stack, "call instruction requires stack usage");

    auto &base = Path.front();

    CommandT command;
    if (Path.size() == 1 && generic_set.contains(base))
        generate_generic(*this, command, use_stack);
    else
        get_generator(true, use_stack, base).Generator(*this, command, use_stack);

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
    if (UseCount)
        return true;

    if (std::ranges::any_of(
        Arguments,
        [](const ValuePtr &argument)
        {
            return argument->RequireStack();
        }))
        return true;

    auto &base = Path.front();

    if (Path.size() == 1 && generic_set.contains(base))
        return false;

    return get_generator(false, true, base).RequireStack;
}
