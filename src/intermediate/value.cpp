#include <utility>
#include <mcc/error.hpp>
#include <mcc/value.hpp>

mcc::Value::Value(SourceLocation where, TypeContext &context, TypePtr type)
    : Where(std::move(where)),
      Context(context),
      Type(std::move(type))
{
}

void mcc::Value::Generate(CommandVector &commands, bool stack) const
{
    Error(Where, "mcc::Value::Generate");
}

bool mcc::Value::RequireStack() const
{
    Error(Where, "mcc::Value::RequireStack");
}

mcc::Result mcc::Value::GenerateResult(const bool stringify) const
{
    Error(Where, "mcc::Value::GenerateResult");
}

void mcc::Value::Use()
{
    UseCount++;
}

void mcc::Value::Drop()
{
    UseCount--;
}
