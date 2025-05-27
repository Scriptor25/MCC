#include <utility>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

static void generate_macro_print(const mcc::MacroInstruction &self, mcc::CommandVector &commands)
{
    mcc::Assert(self.Arguments.size() == 2, self.Where, "argument count must be 2, but is {}", self.Arguments.size());

    const auto targets = std::dynamic_pointer_cast<mcc::ConstantString>(self.Arguments.at(0))->Value;
    const auto message = self.Arguments.at(1)->GenerateResult();

    std::string message_value, prefix;
    if (message.WithArgument)
    {
        prefix = "$";
    }

    switch (message.Type)
    {
        case mcc::ResultType_Value:
            message_value = message.Value;
            break;

        case mcc::ResultType_Reference:
            message_value = std::format(
                "{{\"{}\":\"{}\",\"nbt\":\"{}\",\"interpret\":true}}",
                message.ReferenceType,
                message.Target,
                message.Path);
            break;

        case mcc::ResultType_Argument:
            message_value = message.Name;
            prefix = "$";
            break;

        default:
            mcc::Error(
                self.Where,
                "message must be {} or {}, but is {}",
                mcc::ResultType_Value,
                mcc::ResultType_Reference,
                message.Type);
    }

    commands.Append("{}tellraw {} {}", prefix, targets, message_value);
}

static void generate_macro_swap(const mcc::MacroInstruction &self, mcc::CommandVector &commands)
{
    mcc::Assert(self.Arguments.size() == 2, self.Where, "argument count must be 2, but is {}", self.Arguments.size());

    auto value1 = self.Arguments.at(0)->GenerateResult();
    auto value2 = self.Arguments.at(1)->GenerateResult();

    mcc::Assert(
        value1.Type == mcc::ResultType_Reference,
        self.Where,
        "first value must be {}, but is {}",
        mcc::ResultType_Reference,
        value1.Type);
    mcc::Assert(
        value2.Type == mcc::ResultType_Reference,
        self.Where,
        "second value must be {}, but is {}",
        mcc::ResultType_Reference,
        value2.Type);

    auto tmp_name = self.GetTmpName();

    std::string prefix1, prefix2, prefix3;
    if (value1.WithArgument)
    {
        prefix1 = "$";
    }
    if (value1.WithArgument || value2.WithArgument)
    {
        prefix2 = "$";
    }
    if (value2.WithArgument)
    {
        prefix3 = "$";
    }

    commands.Append(
        "{}data modify storage {} {} set from {} {} {}",
        prefix1,
        self.Location,
        tmp_name,
        value1.ReferenceType,
        value1.Target,
        value1.Path);
    commands.Append(
        "{}data modify {} {} {} set from {} {} {}",
        prefix2,
        value1.ReferenceType,
        value1.Target,
        value1.Path,
        value2.ReferenceType,
        value2.Target,
        value2.Path);
    commands.Append(
        "{}data modify {} {} {} set from storage {} {}",
        prefix3,
        value2.ReferenceType,
        value2.Target,
        value2.Path,
        self.Location,
        tmp_name);

    commands.Append("data remove storage {} {}", self.Location, tmp_name);
}

static void generate_macro_data(const mcc::MacroInstruction &self, mcc::CommandVector &commands)
{
    mcc::Assert(self.Arguments.size() == 2, self.Where, "argument count must be 2, but is {}", self.Arguments.size());

    const auto dst = self.Arguments.at(0);
    const auto src = self.Arguments.at(1);

    mcc::Assert(dst->IsMutable, self.Where, "dst must be mutable");

    auto dst_value = dst->GenerateResult();
    auto src_value = src->GenerateResultUnwrap();

    mcc::Assert(
        dst_value.Type == mcc::ResultType_Reference,
        self.Where,
        "dst must be {}, but is {}",
        mcc::ResultType_Reference,
        dst_value.Type);

    std::string prefix;
    if (dst_value.WithArgument || src_value.WithArgument)
    {
        prefix = "$";
    }

    commands.Append(
        "{}data modify {} {} {} set from {}",
        prefix,
        dst_value.ReferenceType,
        dst_value.Target,
        dst_value.Path,
        src_value.Value);
}

static void generate_macro_store(const mcc::MacroInstruction &self, mcc::CommandVector &commands)
{
    mcc::Assert(self.Arguments.size() == 2, self.Where, "argument count must be 2, but is {}", self.Arguments.size());

    const auto dst = self.Arguments.at(0);
    const auto src = self.Arguments.at(1);

    mcc::Assert(dst->IsMutable, self.Where, "dst must be mutable");

    auto dst_value = dst->GenerateResult();
    auto src_value = src->GenerateResultUnwrap();

    mcc::Assert(
        dst_value.Type == mcc::ResultType_Reference,
        self.Where,
        "dst must be {}, but is {}",
        mcc::ResultType_Reference,
        dst_value.Type);

    std::string prefix;
    if (dst_value.WithArgument || src_value.WithArgument)
    {
        prefix = "$";
    }

    commands.Append(
        "{}execute store result {} {} {} double 1 run {}",
        prefix,
        dst_value.ReferenceType,
        dst_value.Target,
        dst_value.Path,
        src_value.Value);
}

using Generator = std::function<void(const mcc::MacroInstruction &, mcc::CommandVector &)>;

static const std::map<std::string_view, Generator> generators
{
    {"print", generate_macro_print},
    {"swap", generate_macro_swap},
    {"data", generate_macro_data},
    {"store", generate_macro_store},
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
