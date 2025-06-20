#include <mcc/instruction.hpp>

mcc::Instruction::Instruction(const SourceLocation &where, const TypePtr &type, E_FieldType field_type)
    : Value(where, type, field_type)
{
}

bool mcc::Instruction::IsTerminator() const
{
    return false;
}

std::string mcc::Instruction::GetStackPath() const
{
    return std::format("stack[0].{}", reinterpret_cast<uintptr_t>(this));
}

std::string mcc::Instruction::GetTemp() const
{
    return std::format("{}", reinterpret_cast<uintptr_t>(this));
}

std::string mcc::Instruction::CreateScore() const
{
    return std::format("scoreboard objectives add {} dummy", reinterpret_cast<uintptr_t>(this));
}

std::string mcc::Instruction::RemoveScore() const
{
    return std::format("scoreboard objectives remove {}", reinterpret_cast<uintptr_t>(this));
}
