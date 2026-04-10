#include <mcc/block.hpp>
#include <mcc/command.hpp>
#include <mcc/constant.hpp>
#include <mcc/error.hpp>
#include <mcc/function.hpp>
#include <mcc/instruction.hpp>

#include <algorithm>
#include <ranges>

mcc::InstructionPtr mcc::CallInstruction::Create(
        const SourceLocation &where,
        const std::string &name,
        const FunctionPtr &parent,
        const FunctionPtr &callee,
        const std::vector<std::pair<
                std::string,
                ValuePtr
        >> &arguments,
        const BlockPtr &landing_pad)
{
    auto self = std::make_shared<CallInstruction>(where, name, parent, callee, arguments, landing_pad);

    self->Self = self;
    for (const auto &argument : self->Arguments | std::views::values)
        argument->Use(self);
    if (self->LandingPad)
        self->LandingPad->Use(self);

    return self;
}

mcc::CallInstruction::CallInstruction(
        const SourceLocation &where,
        const std::string &name,
        FunctionPtr parent,
        const FunctionPtr &callee,
        const std::vector<std::pair<
                std::string,
                ValuePtr
        >> &arguments,
        BlockPtr landing_pad)
    : Instruction(
              where,
              name,
              callee->ResultType,
              FieldType_::ImmutableReference),
      Parent(std::move(parent)),
      Callee(callee),
      Arguments(arguments),
      LandingPad(std::move(landing_pad))
{
}

mcc::CallInstruction::~CallInstruction()
{
    for (const auto &argument : Arguments | std::views::values)
        argument->Drop(Self);
    if (LandingPad)
        LandingPad->Drop(Self);
}

void mcc::CallInstruction::Generate(
        CommandVector &commands,
        const bool stack) const
{
    auto location = Parent->Mangle();

    auto stack_path = GetStackPath();
    auto temp       = GetTemp();

    std::string argument_prefix;
    std::string argument_object;
    auto require_cleanup = false;
    if (!Arguments.empty())
    {
        const auto constant = std::ranges::all_of(
                Arguments | std::views::values,
                [](auto &argument)
                {
                    return std::dynamic_pointer_cast<Constant>(argument)
                           || std::dynamic_pointer_cast<ArgumentValue>(argument);
                });

        if (constant)
        {
            argument_object += " {";

            for (unsigned i = 0; i < Arguments.size(); ++i)
            {
                if (i)
                    argument_object += ',';

                auto [key, argument] = Arguments[i];

                if (auto value = argument->GenerateResult(); value.Type == ResultType_::Argument)
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
            commands.Append("data modify storage {} {} set value {{}}", location, stack_path);

            for (unsigned i = 0; i < Arguments.size(); ++i)
            {
                auto &[key_, argument_] = Arguments[i];

                auto value  = argument_->GenerateResult();
                auto prefix = value.WithArgument ? "$" : "";

                switch (value.Type)
                {
                case ResultType_::Value:
                    commands.Append(
                            "{}data modify storage {} {}.{} set value {}",
                            prefix,
                            location,
                            stack_path,
                            key_,
                            value.Value);
                    break;

                case ResultType_::Reference:
                    if (Callee->Parameters[i].FieldType == FieldType_::Value)
                        commands.Append(
                                "{}data modify storage {} {}.{} set from {} {} {}",
                                prefix,
                                location,
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
                                location,
                                stack_path,
                                key_,
                                value.Target);
                        commands.Append(
                                "{}data modify storage {} {}.{}_path set value \"{}\"",
                                prefix,
                                location,
                                stack_path,
                                key_,
                                value.Path);
                    }
                    break;

                case ResultType_::Argument:
                    commands.Append(
                            "$data modify storage {} {}.{} set value {}",
                            location,
                            stack_path,
                            key_,
                            value.Name);
                    break;

                default:
                    Error(Where,
                          "value must be {}, {} or {}, but is {}",
                          ResultType_::Value,
                          ResultType_::Reference,
                          ResultType_::Argument,
                          value.Type);
                }
            }

            argument_object = std::format(" with storage {} {}", location, stack_path);
            require_cleanup = true;
        }
    }

    auto callee = Callee->Mangle();

    if (Callee->Throws)
    {
        commands.Append(CreateScore());
        commands.Append(
                "{}execute store result score %c {} run function {}{}",
                argument_prefix,
                temp,
                callee,
                argument_object);
        commands.Append("data remove storage {} {}", location, stack_path);
        commands.Append(
                "execute unless score %c {} matches 0 run data modify storage {} {} set value 1",
                temp,
                location,
                stack_path);
        commands.Append(RemoveScore());

        commands.Append("data remove storage {} result", location);
        commands.Append(
                "execute if data storage {0} {1} run data modify storage {0} result set from storage {2} "
                "result",
                location,
                stack_path,
                callee);

        if (LandingPad)
        {
            std::string prefix, arguments;
            LandingPad->Parent->ForwardArguments(prefix, arguments);

            commands.Append(
                    "{}execute if data storage {} result run return run function {}{}",
                    prefix,
                    location,
                    LandingPad->GetLocation(),
                    arguments);
        }
        else
        {
            commands.Append("execute if data storage {0} result run data remove storage {0} stack[0]", location);
            commands.Append("execute if data storage {} result run return 1", location);
        }
    }
    else
        commands.Append("{}function {}{}", argument_prefix, callee, argument_object);

    if (!Uses.empty())
    {
        Assert(stack, Where, "call instruction with result requires stack");
        commands.Append("data modify storage {} {} set from storage {} result", location, stack_path, callee);
    }
    else if (require_cleanup)
        commands.Append("data remove storage {} {}", location, stack_path);
}

bool mcc::CallInstruction::RequireStack() const
{
    return !Uses.empty()
           || std::ranges::any_of(
                   Arguments | std::views::values,
                   [](auto &argument) { return argument->RequireStack(); })
           || !std::ranges::all_of(
                   Arguments | std::views::values,
                   [](auto &argument)
                   {
                       return std::dynamic_pointer_cast<Constant>(argument)
                              || std::dynamic_pointer_cast<ArgumentValue>(argument);
                   });
}

mcc::Result mcc::CallInstruction::GenerateResult() const
{
    return {
        .Type          = ResultType_::Reference,
        .ReferenceType = ReferenceType_::Storage,
        .Target        = Parent->Mangle().String(),
        .Path          = GetStackPath(),
    };
}
