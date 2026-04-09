#include <mcc/command.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>
#include <mcc/type.hpp>

#include <functional>

static void generate_macro_print(
        const mcc::MacroInstruction &self,
        mcc::CommandVector &commands)
{
    mcc::Assert(self.Arguments.size() == 2, self.Where, "argument count must be 2, but is {}", self.Arguments.size());

    const auto targets = std::dynamic_pointer_cast<mcc::ConstantString>(self.Arguments[0])->Value;
    const auto message = self.Arguments[1]->GenerateResult();

    std::string message_value, prefix;
    if (message.WithArgument)
        prefix = "$";

    switch (message.Type)
    {
    case mcc::ResultType_::Value:
        message_value = message.Value;
        break;

    case mcc::ResultType_::Reference:
        message_value = std::format(
                "{{\"{}\":\"{}\",\"nbt\":\"{}\",\"interpret\":true}}",
                message.ReferenceType,
                message.Target,
                message.Path);
        break;

    case mcc::ResultType_::Argument:
        message_value = message.Name;
        prefix        = "$";
        break;

    default:
        mcc::Error(
                self.Where,
                "message must be {} or {}, but is {}",
                mcc::ResultType_::Value,
                mcc::ResultType_::Reference,
                message.Type);
    }

    commands.Append("{}tellraw {} {}", prefix, targets, message_value);
}

static void generate_macro_swap(
        const mcc::MacroInstruction &self,
        mcc::CommandVector &commands)
{
    mcc::Assert(self.Arguments.size() == 2, self.Where, "argument count must be 2, but is {}", self.Arguments.size());

    auto value1 = self.Arguments[0]->GenerateResult();
    auto value2 = self.Arguments[1]->GenerateResult();

    mcc::Assert(
            value1.Type == mcc::ResultType_::Reference,
            self.Where,
            "first value must be {}, but is {}",
            mcc::ResultType_::Reference,
            value1.Type);
    mcc::Assert(
            value2.Type == mcc::ResultType_::Reference,
            self.Where,
            "second value must be {}, but is {}",
            mcc::ResultType_::Reference,
            value2.Type);

    auto tmp_name = self.GetTemp();

    std::string prefix1, prefix2, prefix3;
    if (value1.WithArgument)
        prefix1 = "$";
    if (value1.WithArgument || value2.WithArgument)
        prefix2 = "$";
    if (value2.WithArgument)
        prefix3 = "$";

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

static void generate_macro_data(
        const mcc::MacroInstruction &self,
        mcc::CommandVector &commands)
{
    mcc::Assert(self.Arguments.size() == 2, self.Where, "argument count must be 2, but is {}", self.Arguments.size());

    const auto dst = self.Arguments[0];
    const auto src = self.Arguments[1];

    mcc::Assert(dst->IsMutable(), self.Where, "dst must be mutable");

    auto dst_value = dst->GenerateResult();
    auto src_value = src->GenerateResultUnwrap();

    mcc::Assert(
            dst_value.Type == mcc::ResultType_::Reference,
            self.Where,
            "dst must be {}, but is {}",
            mcc::ResultType_::Reference,
            dst_value.Type);

    std::string prefix;
    if (dst_value.WithArgument || src_value.WithArgument)
        prefix = "$";

    commands.Append(
            "{}data modify {} {} {} set from {}",
            prefix,
            dst_value.ReferenceType,
            dst_value.Target,
            dst_value.Path,
            src_value.Value);
}

static void generate_macro_store(
        const mcc::MacroInstruction &self,
        mcc::CommandVector &commands)
{
    mcc::Assert(self.Arguments.size() == 2, self.Where, "argument count must be 2, but is {}", self.Arguments.size());

    const auto dst = self.Arguments[0];
    const auto src = self.Arguments[1];

    mcc::Assert(dst->IsMutable(), self.Where, "dst must be mutable");

    auto dst_value = dst->GenerateResult();
    auto src_value = src->GenerateResultUnwrap();

    mcc::Assert(
            dst_value.Type == mcc::ResultType_::Reference,
            self.Where,
            "dst must be {}, but is {}",
            mcc::ResultType_::Reference,
            dst_value.Type);

    std::string prefix;
    if (dst_value.WithArgument || src_value.WithArgument)
        prefix = "$";

    commands.Append(
            "{}execute store result {} {} {} double 1 run {}",
            prefix,
            dst_value.ReferenceType,
            dst_value.Target,
            dst_value.Path,
            src_value.Value);
}

using Generator = std::function<void(const mcc::MacroInstruction &, mcc::CommandVector &)>;

static const std::map<std::string_view, Generator> generators = {
    { "print", generate_macro_print },
    {  "swap",  generate_macro_swap },
    {  "data",  generate_macro_data },
    { "store", generate_macro_store },
};

mcc::InstructionPtr mcc::MacroInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        const ResourceLocation &location,
        const std::string &macro,
        const std::vector<ValuePtr> &arguments)
{
    auto self = std::make_shared<MacroInstruction>(where, name, context, location, macro, arguments);

    self->Self = self;
    for (const auto &argument : self->Arguments)
        argument->Use(self);

    return self;
}

mcc::MacroInstruction::MacroInstruction(
        const SourceLocation &where,
        const std::string &name,
        TypeContext &context,
        ResourceLocation location,
        std::string macro,
        const std::vector<ValuePtr> &arguments)
    : Instruction(
              where,
              name,
              context.GetVoid(),
              FieldType_::Value),
      Location(std::move(location)),
      Macro(std::move(macro)),
      Arguments(arguments)
{
}

mcc::MacroInstruction::~MacroInstruction()
{
    for (const auto &argument : Arguments)
        argument->Drop(Self);
}

void mcc::MacroInstruction::Generate(
        CommandVector &commands,
        const bool stack) const
{
    Assert(generators.contains(Macro), Where, "no generator for macro {}", Macro);
    generators.at(Macro)(*this, commands);
}

bool mcc::MacroInstruction::RequireStack() const
{
    return false;
}
