#include <mcc/instruction.hpp>

mcc::Instruction::Instruction(
        const SourceLocation &where,
        const TypePtr &type,
        E_FieldType field_type)
    : Value(where,
            type,
            field_type)
{
}

bool mcc::Instruction::IsTerminator() const
{
    return false;
}

std::string mcc::Instruction::GetStackPath() const
{
    return std::format("stack[0].x{}", StackId);
}

std::string mcc::Instruction::GetTemp() const
{
    return std::format("x{}", StackId);
}

std::string mcc::Instruction::CreateScore() const
{
    return std::format("scoreboard objectives add x{} dummy", StackId);
}

std::string mcc::Instruction::RemoveScore() const
{
    return std::format("scoreboard objectives remove x{}", StackId);
}
