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

void mcc::CallInstruction::Gen(CommandVector &commands) const
{
    Command command;

    switch (Callee)
    {
        case Callee_Give:
        case Callee_SetBlock:
            command += ToString(Callee);
            for (auto &argument: Arguments)
                command += ' ' + argument->GenResult().Value;
            break;

        case Callee_Function:
        {
            auto callee = Arguments[0]->GenResult();
            auto arguments = Arguments[1]->GenResult();

            command += "function ";
            command += callee.Value;
            command += ' ';

            switch (arguments.Type)
            {
                case CommandResultType_Value:
                    command += arguments.Value;
                    break;

                case CommandResultType_Storage:
                    command += "with storage ";
                    command += Location.String();
                    command += ' ';
                    command += arguments.Path;
                    break;

                case CommandResultType_Score:
                    throw std::runtime_error("TODO");
            }

            break;
        }

        case Callee_TellRaw:
        {
            auto targets = Arguments[0]->GenResult();
            auto [
                message_type_,
                message_value_,
                message_path_,
                message_player_,
                message_objective_
            ] = Arguments[1]->GenResult();

            command += "tellraw ";
            command += targets.Value;
            command += ' ';

            switch (message_type_)
            {
                case CommandResultType_Value:
                    command += message_value_;
                    break;

                case CommandResultType_Storage:
                    command += "{storage:";
                    command += '"';
                    command += Location.String();
                    command += '"';
                    command += ",nbt:";
                    command += '"';
                    command += message_path_;
                    command += '"';
                    command += ",interpret:true}";
                    break;

                case CommandResultType_Score:
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

            break;
        }

        default:
            throw std::runtime_error("TODO");
    }

    commands.Append(
        "execute store result storage {} stack[0].result.{} double 1 run {}",
        Location,
        GetResultID(),
        command);
}

mcc::Command mcc::CallInstruction::GenInline() const
{
    Command command;

    switch (Callee)
    {
        case Callee_Give:
        case Callee_SetBlock:
            command += ToString(Callee);
            for (auto &argument: Arguments)
                command += ' ' + argument->GenResult().Value;
            break;

        case Callee_Function:
        {
            auto callee = Arguments[0]->GenResult();
            auto arguments = Arguments[1]->GenResult();

            command += "function ";
            command += callee.Value;
            command += ' ';

            switch (arguments.Type)
            {
                case CommandResultType_Value:
                    command += arguments.Value;
                    break;

                case CommandResultType_Storage:
                    command += "with storage ";
                    command += Location.String();
                    command += ' ';
                    command += arguments.Path;
                    break;

                case CommandResultType_Score:
                    throw std::runtime_error("TODO");
            }

            break;
        }

        case Callee_TellRaw:
        {
            auto targets = Arguments[0]->GenResult();
            auto [
                message_type_,
                message_value_,
                message_path_,
                message_player_,
                message_objective_
            ] = Arguments[1]->GenResult();

            command += "tellraw ";
            command += targets.Value;
            command += ' ';

            switch (message_type_)
            {
                case CommandResultType_Value:
                    command += message_value_;
                    break;

                case CommandResultType_Storage:
                    command += "{storage:";
                    command += '"';
                    command += Location.String();
                    command += '"';
                    command += ",nbt:";
                    command += '"';
                    command += message_path_;
                    command += '"';
                    command += ",interpret:true}";
                    break;

                case CommandResultType_Score:
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

            break;
        }

        default:
            throw std::runtime_error("TODO");
    }

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
