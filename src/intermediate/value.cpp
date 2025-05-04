#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

void mcc::Value::Generate(CommandVector &commands, bool use_stack) const
{
    Error("mcc::Value::Gen");
}

mcc::CommandT mcc::Value::GenerateInline(bool use_stack) const
{
    Error("mcc::Value::GenInline");
}

mcc::Result mcc::Value::GenResult(const bool stringify, bool use_stack) const
{
    Error("mcc::Value::GenResult");
}

void mcc::Value::Use()
{
    UseCount++;
}

void mcc::Value::Drop()
{
    UseCount--;
}

std::string mcc::Instruction::GetStackPath() const
{
    return "stack[0].result." + std::to_string(reinterpret_cast<uintptr_t>(this));
}

bool mcc::Constant::RequireStack() const
{
    return false;
}
