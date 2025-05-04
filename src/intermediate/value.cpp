#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

void mcc::Value::Gen(CommandVector &commands) const
{
    Error("mcc::Value::Gen");
}

mcc::CommandT mcc::Value::GenInline() const
{
    Error("mcc::Value::GenInline");
}

mcc::CommandResult mcc::Value::GenResult(const bool stringify) const
{
    Error("mcc::Value::GenResult");
}

std::string mcc::Instruction::GetResultID() const
{
    return std::to_string(reinterpret_cast<uintptr_t>(this));
}
