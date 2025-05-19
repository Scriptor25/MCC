#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

static void generate_macro_print(const mcc::MacroInstruction &self, mcc::CommandVector &commands)
{
    auto targets = self.Arguments[0]->GenerateResult(false);

    std::string message_value;
    switch (auto message = self.Arguments[1]->GenerateResult(false); message.Type)
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
                self.Where,
                "message must be {}, {} or {}, but is {}",
                mcc::ResultType_Value,
                mcc::ResultType_Storage,
                mcc::ResultType_Score,
                message.Type);
    }

    if (!self.UseCount)
    {
        commands.Append(
            "tellraw {} {}",
            targets.Value,
            message_value);
        return;
    }

    commands.Append("tellraw {} {}", targets.Value, message_value);
}

static void generate_macro_swap(const mcc::MacroInstruction &self, mcc::CommandVector &commands)
{
    auto value1 = self.Arguments[0]->GenerateResult(false);
    auto value2 = self.Arguments[1]->GenerateResult(false);

    auto tmp_name = self.GetTmpName();

    switch (value1.Type)
    {
        case mcc::ResultType_Storage:
            commands.Append(
                "data modify storage {} {} set from storage {} {}",
                self.Location,
                tmp_name,
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
                        self.Where,
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
                tmp_name,
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
                        self.Where,
                        "value 2 must be {} or {}, but is {}",
                        mcc::ResultType_Storage,
                        mcc::ResultType_Score,
                        value2.Type);
            }
            break;

        default:
            mcc::Error(
                self.Where,
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
                tmp_name);
            break;

        case mcc::ResultType_Score:
            commands.Append(
                "execute store result score {} {} run data get storage {} {}",
                value2.Player,
                value2.Objective,
                self.Location,
                tmp_name);
            break;

        default:
            mcc::Error(
                self.Where,
                "value 2 must be {} or {}, but is {}",
                mcc::ResultType_Storage,
                mcc::ResultType_Score,
                value2.Type);
    }

    commands.Append("data remove storage {} {}", self.Location, tmp_name);
}

using Generator = std::function<void(const mcc::MacroInstruction &, mcc::CommandVector &)>;

static const std::map<std::string_view, Generator> generators
{
    {"print", generate_macro_print},
    {"swap", generate_macro_swap},
};

mcc::InstructionPtr mcc::MacroInstruction::Create(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const std::string &name,
    const std::vector<ValuePtr> &arguments)
{
    return std::make_shared<MacroInstruction>(where, context, location, name, arguments);
}

mcc::MacroInstruction::MacroInstruction(
    const SourceLocation &where,
    TypeContext &context,
    const ResourceLocation &location,
    const std::string &name,
    const std::vector<ValuePtr> &arguments)
    : Instruction(where, context, context.GetVoid()),
      Location(location),
      Name(name),
      Arguments(arguments)
{
    for (const auto &argument: Arguments)
        argument->Use();
}

mcc::MacroInstruction::~MacroInstruction()
{
    for (const auto &argument: Arguments)
        argument->Drop();
}

void mcc::MacroInstruction::Generate(CommandVector &commands, const bool stack) const
{
    Assert(generators.contains(Name), Where, "no generator for macro {}", Name);
    generators.at(Name)(*this, commands);
}

bool mcc::MacroInstruction::RequireStack() const
{
    return false;
}
