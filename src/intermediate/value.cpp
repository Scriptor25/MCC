#include <mcc/error.hpp>
#include <mcc/intermediate.hpp>

bool mcc::Result::operator==(const Result &result) const
{
    if (Type != result.Type)
        return false;

    switch (Type)
    {
        case ResultType_None:
            return true;
        case ResultType_Value:
            return Value == result.Value;
        case ResultType_Storage:
            return Location == result.Location && Path == result.Path;
        case ResultType_Score:
            return Player == result.Player && Objective == result.Objective;
    }

    return false;
}

void mcc::Value::Generate(CommandVector &commands, bool stack) const
{
    Error("mcc::Value::Generate");
}

bool mcc::Value::RequireStack() const
{
    Error("mcc::Value::RequireStack");
}

mcc::Result mcc::Value::GenerateResult(const bool stringify) const
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

bool mcc::Constant::RequireStack() const
{
    return false;
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
