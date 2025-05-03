#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::IfUnlessInstruction::Create(bool unless, ValuePtr condition, ValuePtr then)
{
    return std::make_shared<IfUnlessInstruction>(unless, std::move(condition), std::move(then));
}

mcc::IfUnlessInstruction::IfUnlessInstruction(const bool unless, ValuePtr condition, ValuePtr then)
    : Unless(unless),
      Condition(std::move(condition)),
      Then(std::move(then))
{
}

void mcc::IfUnlessInstruction::Gen(std::vector<Command> &commands) const
{
    throw std::runtime_error("TODO");
}

mcc::Command mcc::IfUnlessInstruction::GenInline() const
{
    throw std::runtime_error("TODO");
}
