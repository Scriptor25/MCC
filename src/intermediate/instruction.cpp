#include <mcc/instruction.hpp>

mcc::Instruction::Instruction(const SourceLocation &where, const TypePtr &type)
    : Value(where, type)
{
}

bool mcc::Instruction::IsTerminator() const
{
    return false;
}

std::string mcc::Instruction::GetStackPath() const
{
    return std::format("stack[0].result.{}", reinterpret_cast<uintptr_t>(this));
}

std::string mcc::Instruction::GetTmpName() const
{
    return std::format("tmp.{}", reinterpret_cast<uintptr_t>(this));
}

std::string mcc::Instruction::CreateTmpScore() const
{
    return std::format("scoreboard objectives add tmp.{} dummy", reinterpret_cast<uintptr_t>(this));
}

std::string mcc::Instruction::RemoveTmpScore() const
{
    return std::format("scoreboard objectives remove tmp.{}", reinterpret_cast<uintptr_t>(this));
}
