#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

void mcc::Value::Generate(const Builder &builder, CommandVector &commands, bool use_stack) const
{
    Error("mcc::Value::Generate");
}

mcc::CommandT mcc::Value::GenerateInline(const Builder &builder, bool use_stack) const
{
    Error("mcc::Value::GenerateInline");
}

mcc::Result mcc::Value::GenerateResult(const bool stringify, bool use_stack) const
{
    Error("mcc::Value::GenerateResult");
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

bool mcc::Constant::RequireStack() const
{
    return false;
}
