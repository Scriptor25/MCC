#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::CallInstruction::Create(CalleeE callee, std::vector<ValuePtr> arguments)
{
    return std::make_shared<CallInstruction>(callee, std::move(arguments));
}

mcc::CallInstruction::CallInstruction(const CalleeE callee, std::vector<ValuePtr> arguments)
    : Callee(callee),
      Arguments(std::move(arguments))
{
}

void mcc::CallInstruction::Gen(std::vector<Command> &commands) const
{
    Command command;

    command += ToString(Callee);
    for (auto &argument: Arguments)
        command += ' ' + argument->GenInline();

    commands.emplace_back(std::move(command));
}

mcc::Command mcc::CallInstruction::GenInline() const
{
    throw std::runtime_error("TODO");
}
