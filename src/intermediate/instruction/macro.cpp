#include <utility>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

static void generate_macro_print(const mcc::MacroInstruction &self, mcc::CommandVector &commands)
{
    const auto targets = std::dynamic_pointer_cast<mcc::ConstantString>(self.Arguments.at(0));

    std::string message_value;
    switch (auto message = self.Arguments.at(1)->GenerateResult(); message.Type)
    {
        case mcc::ResultType_Value:
            message_value = message.Value;
            break;

        case mcc::ResultType_Storage:
            message_value = std::format(
                "{{\"storage\":\"{}\",\"nbt\":\"{}\",\"interpret\":true}}",
                message.Location,
                message.Path);
            break;

        default:
            mcc::Error(
                self.Where,
                "message must be {} or {}, but is {}",
                mcc::ResultType_Value,
                mcc::ResultType_Storage,
                message.Type);
    }

    commands.Append("tellraw {} {}", targets->Value, message_value);
}

static void generate_macro_swap(const mcc::MacroInstruction &self, mcc::CommandVector &commands)
{
    auto value1 = self.Arguments.at(0)->GenerateResult();
    auto value2 = self.Arguments.at(1)->GenerateResult();

    mcc::Assert(
        value1.Type == mcc::ResultType_Storage,
        self.Where,
        "first value must be {}, but is {}",
        mcc::ResultType_Storage,
        value1.Type);
    mcc::Assert(
        value2.Type == mcc::ResultType_Storage,
        self.Where,
        "second value must be {}, but is {}",
        mcc::ResultType_Storage,
        value2.Type);

    auto tmp_name = self.GetTmpName();

    commands.Append(
        "data modify storage {} {} set from storage {} {}",
        self.Location,
        tmp_name,
        value1.Location,
        value1.Path);
    commands.Append(
        "data modify storage {} {} set from storage {} {}",
        value1.Location,
        value1.Path,
        value2.Location,
        value2.Path);
    commands.Append(
        "data modify storage {} {} set from storage {} {}",
        value2.Location,
        value2.Path,
        self.Location,
        tmp_name);

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
    ResourceLocation location,
    std::string name,
    const std::vector<ValuePtr> &arguments)
    : Instruction(where, context.GetVoid(), false),
      Location(std::move(location)),
      Name(std::move(name)),
      Arguments(arguments)
{
    for (const auto &argument: Arguments)
    {
        argument->Use();
    }
}

mcc::MacroInstruction::~MacroInstruction()
{
    for (const auto &argument: Arguments)
    {
        argument->Drop();
    }
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
