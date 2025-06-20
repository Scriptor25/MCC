#include <utility>
#include <mcc/block.hpp>
#include <mcc/command.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::CallInstruction::Create(
    const SourceLocation &where,
    const ResourceLocation &location,
    const FunctionPtr &callee,
    const std::vector<std::pair<std::string, ValuePtr>> &arguments,
    const BlockPtr &landing_pad)
{
    return std::make_shared<CallInstruction>(where, location, callee, arguments, landing_pad);
}

mcc::CallInstruction::CallInstruction(
    const SourceLocation &where,
    ResourceLocation location,
    const FunctionPtr &callee,
    const std::vector<std::pair<std::string, ValuePtr>> &arguments,
    BlockPtr landing_pad)
    : Instruction(where, callee->ResultType, FieldType_ImmutableReference),
      Location(std::move(location)),
      Callee(callee),
      Arguments(arguments),
      LandingPad(std::move(landing_pad))
{
    for (const auto &argument : Arguments | std::views::values)
        argument->Use();
    if (LandingPad)
        LandingPad->Use();
}

mcc::CallInstruction::~CallInstruction()
{
    for (const auto &argument : Arguments | std::views::values)
        argument->Drop();
    if (LandingPad)
        LandingPad->Drop();
}

void mcc::CallInstruction::Generate(CommandVector &commands, const bool stack) const
{
    auto stack_path = GetStackPath();
    auto temp = GetTemp();

    std::string argument_prefix;
    std::string argument_object;
    auto require_cleanup = false;
    if (!Arguments.empty())
    {
        const auto constant = std::ranges::all_of(
            Arguments | std::views::values,
            [](auto &argument)
            {
                return std::dynamic_pointer_cast<Constant>(argument) ||
                       std::dynamic_pointer_cast<ArgumentValue>(argument);
            });

        if (constant)
        {
            argument_object += " {";

            for (unsigned i = 0; i < Arguments.size(); ++i)
            {
                if (i)
                    argument_object += ',';

                auto [key, argument] = Arguments[i];

                if (auto value = argument->GenerateResult(); value.Type == ResultType_Argument)
                {
                    argument_prefix = "$";
                    argument_object += std::format("\"{}\":{}", key, value.Name);
                }
                else
                    argument_object += std::format("\"{}\":{}", key, value.Value);
            }

            argument_object += '}';
        }
        else
        {
            commands.Append("data modify storage {} {} set value {{}}", Location, stack_path);

            for (unsigned i = 0; i < Arguments.size(); ++i)
            {
                auto &[key_, argument_] = Arguments[i];

                auto value = argument_->GenerateResult();
                auto prefix = value.WithArgument ? "$" : "";

                switch (value.Type)
                {
                case ResultType_Value:
                    commands.Append(
                        "{}data modify storage {} {}.{} set value {}",
                        prefix,
                        Location,
                        stack_path,
                        key_,
                        value.Value);
                    break;

                case ResultType_Reference:
                    if (Callee->Parameters[i].FieldType == FieldType_Value)
                        commands.Append(
                            "{}data modify storage {} {}.{} set from {} {} {}",
                            prefix,
                            Location,
                            stack_path,
                            key_,
                            value.ReferenceType,
                            value.Target,
                            value.Path);
                    else
                    {
                        commands.Append(
                            "{}data modify storage {} {}.{}_target set value \"{}\"",
                            prefix,
                            Location,
                            stack_path,
                            key_,
                            value.Target);
                        commands.Append(
                            "{}data modify storage {} {}.{}_path set value \"{}\"",
                            prefix,
                            Location,
                            stack_path,
                            key_,
                            value.Path);
                    }
                    break;

                case ResultType_Argument:
                    commands.Append(
                        "$data modify storage {} {}.{} set value {}",
                        Location,
                        stack_path,
                        key_,
                        value.Name);
                    break;

                default:
                    Error(
                        Where,
                        "value must be {}, {} or {}, but is {}",
                        ResultType_Value,
                        ResultType_Reference,
                        ResultType_Argument,
                        value.Type);
                }
            }

            argument_object = std::format(" with storage {} {}", Location, stack_path);
            require_cleanup = true;
        }
    }

    if (Callee->Throws)
    {
        commands.Append(CreateScore());
        commands.Append(
            "{}execute store result score %c {} run function {}{}",
            argument_prefix,
            temp,
            Callee->Location,
            argument_object);
        commands.Append("data remove storage {} {}", Location, stack_path);
        commands.Append(
            "execute unless score %c {} matches 0 run data modify storage {} {} set value 1",
            temp,
            Location,
            stack_path);
        commands.Append(RemoveScore());

        commands.Append("data remove storage {} result", Location);
        commands.Append(
            "execute if data storage {0} {1} run data modify storage {0} result set from storage {2} "
            "result",
            Location,
            stack_path,
            Callee->Location);

        if (LandingPad)
        {
            std::string prefix, arguments;
            LandingPad->Parent->ForwardArguments(prefix, arguments);

            commands.Append(
                "{}execute if data storage {} result run return run function {}{}",
                prefix,
                Location,
                LandingPad->GetLocation(),
                arguments);
        }
        else
        {
            commands.Append(
                "execute if data storage {0} result run data remove storage {0} stack[0]",
                Location);
            commands.Append("execute if data storage {} result run return 1", Location);
        }
    }
    else
        commands.Append("{}function {}{}", argument_prefix, Callee->Location, argument_object);

    if (UseCount)
    {
        Assert(stack, Where, "call instruction with result requires stack");
        commands.Append("data modify storage {} {} set from storage {} result", Location, stack_path, Callee->Location);
    }
    else if (require_cleanup)
        commands.Append("data remove storage {} {}", Location, stack_path);
}

bool mcc::CallInstruction::RequireStack() const
{
    return UseCount
           || std::ranges::any_of(
               Arguments | std::views::values,
               [](auto &argument)
               {
                   return argument->RequireStack();
               })
           || !std::ranges::all_of(
               Arguments | std::views::values,
               [](auto &argument)
               {
                   return std::dynamic_pointer_cast<Constant>(argument) ||
                          std::dynamic_pointer_cast<ArgumentValue>(argument);
               });
}

mcc::Result mcc::CallInstruction::GenerateResult() const
{
    return {
        .Type = ResultType_Reference,
        .ReferenceType = ReferenceType_Storage,
        .Target = Location.String(),
        .Path = GetStackPath(),
    };
}
