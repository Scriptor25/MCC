#include <mcc/error.hpp>
#include <mcc/value.hpp>

mcc::Value::Value(const SourceLocation &where, TypeContext &context, const TypePtr &type)
    : Where(where),
      Context(context),
      Type(type)
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
