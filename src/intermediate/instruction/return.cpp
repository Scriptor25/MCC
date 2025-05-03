#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::ReturnInstruction::Create(ValuePtr value)
{
    return std::make_shared<ReturnInstruction>(std::move(value));
}

mcc::ReturnInstruction::ReturnInstruction(ValuePtr value)
    : Value(std::move(value))
{
}

void mcc::ReturnInstruction::Gen(std::vector<Command> &commands) const
{
    throw std::runtime_error("TODO");
}

mcc::Command mcc::ReturnInstruction::GenInline() const
{
    throw std::runtime_error("TODO");
}
