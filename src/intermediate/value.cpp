#include <mcc/error.hpp>
#include <mcc/value.hpp>

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
