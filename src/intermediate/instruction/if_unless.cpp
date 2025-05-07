#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::IfUnlessInstruction::Create(
    ResourceLocation location,
    bool unless,
    ValuePtr condition,
    ValuePtr then,
    ValuePtr else_)
{
    return std::make_shared<IfUnlessInstruction>(
        std::move(location),
        unless,
        std::move(condition),
        std::move(then),
        std::move(else_));
}

mcc::IfUnlessInstruction::IfUnlessInstruction(
    ResourceLocation location,
    const bool unless,
    ValuePtr condition,
    ValuePtr then,
    ValuePtr else_)
    : Location(std::move(location)),
      Unless(unless),
      Condition(std::move(condition)),
      Then(std::move(then)),
      Else(std::move(else_))
{
    Condition->Use();
    Then->Use();
    Else->Use();
}

mcc::IfUnlessInstruction::~IfUnlessInstruction()
{
    Condition->Drop();
    Then->Drop();
    Else->Drop();
}

void mcc::IfUnlessInstruction::Generate(CommandVector &commands, const bool use_stack) const
{
    Assert(!UseCount || use_stack, "if-unless instruction requires stack usage");

    auto condition = Condition->GenerateResult(false, use_stack);

    auto then = Then->GenerateInline(use_stack);
    auto else_ = Else->GenerateInline(use_stack);

    auto store_result = UseCount
                            ? std::format("run execute store result storage {} {} double 1 ", Location, GetStackPath())
                            : "";

    auto if_unless = Unless ? "if" : "unless";
    auto unless_if = Unless ? "unless" : "if";

    switch (condition.Type)
    {
        case ResultType_Value:
            if (Unless == (condition.Value == "false" || condition.Value == "0"))
                Then->Generate(commands, use_stack);
            else
                Else->Generate(commands, use_stack);
            break;

        case ResultType_Storage:
            commands.Append(CreateTmpScore());
            commands.Append(
                "execute store result score %c {} run data get storage {} {}",
                GetTmpName(),
                condition.Location,
                condition.Path);
            commands.Append(
                "execute {} score %c {} matches 0 {}run {}",
                if_unless,
                GetTmpName(),
                store_result,
                then);
            commands.Append(
                "execute {} score %c {} matches 0 {}run {}",
                unless_if,
                GetTmpName(),
                store_result,
                else_);
            commands.Append(RemoveTmpScore());
            break;

        case ResultType_Score:
            commands.Append(
                "execute {} score {} {} matches 0 {}run {}",
                if_unless,
                condition.Player,
                condition.Objective,
                store_result,
                then);
            commands.Append(
                "execute {} score {} {} matches 0 {}run {}",
                unless_if,
                condition.Player,
                condition.Objective,
                store_result,
                else_);
            break;

        default:
            Error(
                "condition must be {}, {} or {}, but is {}",
                ResultType_Value,
                ResultType_Storage,
                ResultType_Score,
                condition.Type);
    }
}

mcc::Result mcc::IfUnlessInstruction::GenerateResult(const bool stringify, const bool use_stack) const
{
    if (!UseCount)
        return {.Type = ResultType_None};

    Assert(use_stack, "if-unless instruction requires stack usage");

    if (auto condition = Condition->GenerateResult(false, use_stack);
        condition.Type == ResultType_Value)
    {
        if (Unless == (condition.Value == "false" || condition.Value == "0"))
            return Then->GenerateResult(stringify, use_stack);
        return Else->GenerateResult(stringify, use_stack);
    }

    return {
        .Type = ResultType_Storage,
        .Location = Location,
        .Path = GetStackPath(),
    };
}

bool mcc::IfUnlessInstruction::RequireStack() const
{
    return UseCount;
}
