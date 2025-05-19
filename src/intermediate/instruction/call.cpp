#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/instruction.hpp>

mcc::InstructionPtr mcc::CallInstruction::Create(
    const SourceLocation &where,
    const TypePtr &type,
    const ResourceLocation &location,
    const ResourceLocation &callee,
    const std::vector<std::pair<std::string, ValuePtr>> &arguments,
    const bool may_throw,
    const BlockPtr &landing_pad)
{
    return std::make_shared<CallInstruction>(
        where,
        type,
        location,
        callee,
        arguments,
        may_throw,
        landing_pad);
}

mcc::CallInstruction::CallInstruction(
    const SourceLocation &where,
    const TypePtr &type,
    const ResourceLocation &location,
    const ResourceLocation &callee,
    const std::vector<std::pair<std::string, ValuePtr>> &arguments,
    const bool may_throw,
    const BlockPtr &landing_pad)
    : Instruction(where, type),
      Location(location),
      Callee(callee),
      Arguments(arguments),
      MayThrow(may_throw),
      LandingPad(landing_pad)
{
    for (const auto &argument: Arguments | std::views::values)
        argument->Use();
    if (LandingPad)
        LandingPad->Use();
}

mcc::CallInstruction::~CallInstruction()
{
    for (const auto &argument: Arguments | std::views::values)
        argument->Drop();
    if (LandingPad)
        LandingPad->Drop();
}

void mcc::CallInstruction::Generate(CommandVector &commands, const bool stack) const
{
    auto stack_path = GetStackPath();
    auto tmp_name = GetTmpName();

    std::string argument_object;
    auto require_cleanup = false;
    if (!Arguments.empty())
    {
        const auto constant = !std::ranges::any_of(
            Arguments | std::views::values,
            [](auto &argument)
            {
                return !std::dynamic_pointer_cast<Constant>(argument);
            });

        if (constant)
        {
            argument_object += " {";

            for (unsigned i = 0; i < Arguments.size(); ++i)
            {
                if (i > 0)
                    argument_object += ',';

                auto [key, argument] = Arguments.at(i);

                auto value = argument->GenerateResult(false);
                argument_object += std::format("\"{}\":{}", key, value.Value);
            }

            argument_object += '}';
        }
        else
        {
            commands.Append("data modify storage {} {} set value {{}}", Location, stack_path);

            for (unsigned i = 0; i < Arguments.size(); ++i)
            {
                auto [key, argument] = Arguments.at(i);

                switch (auto value = argument->GenerateResult(false); value.Type)
                {
                    case ResultType_Value:
                        commands.Append(
                            "data modify storage {} {}.{} set value {}",
                            Location,
                            stack_path,
                            key,
                            value.Value);
                        break;

                    case ResultType_Storage:
                        commands.Append(
                            "data modify storage {} {}.{} set from storage {} {}",
                            Location,
                            stack_path,
                            key,
                            value.Location,
                            value.Path);
                        break;

                    case ResultType_Score:
                        commands.Append(
                            "execute store result storage {} {}.{} double 1 run scoreboard players get {} {}",
                            Location,
                            stack_path,
                            key,
                            value.Player,
                            value.Objective);
                        break;

                    default:
                        Error(
                            Where,
                            "value must be {}, {} or {}, but is {}",
                            ResultType_Value,
                            ResultType_Storage,
                            ResultType_Score,
                            value.Type);
                }
            }

            argument_object = std::format(" with storage {} {}", Location, stack_path);
            require_cleanup = true;
        }
    }

    if (MayThrow)
    {
        commands.Append(CreateTmpScore());
        commands.Append(
            "execute store result score %c {} run function {}{}",
            tmp_name,
            Callee,
            argument_object);
        commands.Append(
            "data remove storage {} {}",
            Location,
            stack_path);
        commands.Append(
            "execute unless score %c {} matches 0 run data modify storage {} {} set value 1",
            tmp_name,
            Location,
            stack_path);
        commands.Append(RemoveTmpScore());

        commands.Append(
            "data remove storage {} result",
            Location);
        commands.Append(
            "execute if data storage {0} {1} run data modify storage {0} result set from storage {2} result",
            Location,
            stack_path,
            Callee);

        if (LandingPad)
        {
            std::string prefix, arguments;
            LandingPad->Parent->ForwardArguments(prefix, arguments);

            commands.Append(
                "{}execute if data storage {} result run return run function {}{}",
                prefix,
                Location,
                LandingPad->Parent->GetLocation(LandingPad),
                arguments);
        }
        else
        {
            commands.Append(
                "execute if data storage {0} result run data remove storage {0} stack[0]",
                Location);
            commands.Append(
                "execute if data storage {} result run return 1",
                Location);
        }
    }
    else
    {
        commands.Append(
            "function {} {}",
            Callee,
            argument_object);
    }

    if (UseCount)
    {
        Assert(stack, Where, "call instruction with result requires stack");
        commands.Append(
            "data modify storage {} {} set from storage {} result",
            Location,
            stack_path,
            Callee);
    }
    else if (require_cleanup)
    {
        commands.Append("data remove storage {} {}", Location, stack_path);
    }
}

bool mcc::CallInstruction::RequireStack() const
{
    return UseCount
           || std::ranges::any_of(
               Arguments | std::views::values,
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
