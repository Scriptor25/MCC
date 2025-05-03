#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::OperationInstruction::Create(OperatorE operator_, ValuePtr left, ValuePtr right)
{
    return std::make_shared<OperationInstruction>(operator_, left, right);
}

mcc::OperationInstruction::OperationInstruction(const OperatorE operator_, ValuePtr left, ValuePtr right)
    : Operator(operator_),
      Left(std::move(left)),
      Right(std::move(right))
{
}

void mcc::OperationInstruction::Gen(std::vector<Command> &commands) const
{
    throw std::runtime_error("TODO");
}

mcc::Command mcc::OperationInstruction::GenInline() const
{
    throw std::runtime_error("TODO");
}
