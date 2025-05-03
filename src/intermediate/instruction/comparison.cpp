#include <mcc/intermediate.hpp>

mcc::InstructionPtr mcc::ComparisonInstruction::Create(ComparatorE comparator, ValuePtr left, ValuePtr right)
{
    return std::make_shared<ComparisonInstruction>(comparator, left, right);
}

mcc::ComparisonInstruction::ComparisonInstruction(const ComparatorE comparator, ValuePtr left, ValuePtr right)
    : Comparator(comparator),
      Left(std::move(left)),
      Right(std::move(right))
{
}

void mcc::ComparisonInstruction::Gen(std::vector<Command> &commands) const
{
    throw std::runtime_error("TODO");
}

mcc::Command mcc::ComparisonInstruction::GenInline() const
{
    throw std::runtime_error("TODO");
}
