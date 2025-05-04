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
}

static void _generic(
    mcc::CommandT &command,
    const mcc::ResourceLocation &,
    const mcc::CalleeE callee,
    const std::vector<mcc::ValuePtr> &arguments)
{
    command += mcc::ToString(callee);
    for (auto &argument: arguments)
        command += ' ' + argument->GenResult().Value;
}

static void _function(
    mcc::CommandT &command,
    const mcc::ResourceLocation &location,
    const mcc::CalleeE,
    const std::vector<mcc::ValuePtr> &arguments)
{
    const auto callee_ = arguments[0]->GenResult();
    const auto arguments_ = arguments[1]->GenResult();

    command += "function ";
    command += callee_.Value;
    command += ' ';

    switch (arguments_.Type)
    {
        case mcc::CommandResultType_Value:
            command += arguments_.Value;
            break;

        case mcc::CommandResultType_Storage:
            command += "with storage ";
            command += location.String();
            command += ' ';
            command += arguments_.Path;
            break;

        case mcc::CommandResultType_Score:
            mcc::Error("invalid argument type score");
    }
}


static void _tellraw(
    mcc::CommandT &command,
    const mcc::ResourceLocation &location,
    const mcc::CalleeE,
    const std::vector<mcc::ValuePtr> &arguments)
{
    const auto targets = arguments[0]->GenResult();
    auto [
        message_type_,
        message_value_,
        message_path_,
        message_player_,
        message_objective_
    ] = arguments[1]->GenResult();

    command += "tellraw ";
    command += targets.Value;
    command += ' ';

    switch (message_type_)
    {
        case mcc::CommandResultType_Value:
            command += message_value_;
            break;

        case mcc::CommandResultType_Storage:
            command += "{storage:";
            command += '"';
            command += location.String();
            command += '"';
            command += ",nbt:";
            command += '"';
            command += message_path_;
            command += '"';
            command += ",interpret:true}";
            break;

        case mcc::CommandResultType_Score:
            command += "{score:{name:";
            command += '"';
            command += message_player_;
            command += '"';
            command += ",objective:";
            command += '"';
            command += message_objective_;
            command += '"';
            command += "}}";
            break;
    }
}

using Generator = std::function<void(
    mcc::CommandT &command,
    const mcc::ResourceLocation &location,
    mcc::CalleeE callee,
    const std::vector<mcc::ValuePtr> &arguments)>;

static const std::map<mcc::CalleeE, std::pair<bool, Generator>> generator_map
{
    {mcc::Callee_Give, {true, _generic}},
    {mcc::Callee_SetBlock, {true, _generic}},

    {mcc::Callee_Function, {true, _function}},
    {mcc::Callee_TellRaw, {true, _tellraw}},
};

void mcc::CallInstruction::Gen(CommandVector &commands) const
{
    Assert(generator_map.contains(Callee), "no generator for call to {}", Callee);

    auto &[inlinable_, generator_] = generator_map.at(Callee);

    CommandT command;
    generator_(command, Location, Callee, Arguments);

    commands.Append(
        "execute store result storage {} stack[0].result.{} double 1 run {}",
        Location,
        GetResultID(),
        command);
}

mcc::CommandT mcc::CallInstruction::GenInline() const
{
    Assert(generator_map.contains(Callee), "no generator for call to {}", Callee);

    auto &[inlinable_, generator_] = generator_map.at(Callee);
    Assert(inlinable_, "cannot generate inline call to {}", Callee);

    CommandT command;
    generator_(command, Location, Callee, Arguments);

    return std::format(
        "execute store result storage {} stack[0].result.{} double 1 run {}",
        Location,
        GetResultID(),
        command);
}

mcc::CommandResult mcc::CallInstruction::GenResult(const bool stringify) const
{
    return {
        .Type = CommandResultType_Storage,
        .Path = "stack[0].result." + GetResultID(),
    };
}
