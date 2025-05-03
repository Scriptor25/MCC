#include <mcc/intermediate.hpp>

void mcc::Value::Gen(CommandVector &commands) const
{
    throw std::runtime_error("only inlinable");
}

mcc::Command mcc::Value::GenInline() const
{
    throw std::runtime_error("not inlinable");
}

mcc::CommandResult mcc::Value::GenResult(const bool stringify) const
{
    throw std::runtime_error("no result");
}

std::string mcc::Instruction::GetResultID() const
{
    return std::to_string(reinterpret_cast<uintptr_t>(this));
}
