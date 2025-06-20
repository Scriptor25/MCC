#include <utility>
#include <mcc/error.hpp>
#include <mcc/value.hpp>

mcc::Value::Value(SourceLocation where, TypePtr type, const E_FieldType field_type)
    : Where(std::move(where)),
      Type(std::move(type)),
      FieldType(field_type)
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

mcc::Result mcc::Value::GenerateResult() const
{
    Error(Where, "mcc::Value::GenerateResult");
}

mcc::Result mcc::Value::GenerateResultUnwrap() const
{
    return GenerateResult();
}

void mcc::Value::Use()
{
    UseCount++;
}

void mcc::Value::Drop()
{
    UseCount--;
}

bool mcc::Value::IsMutable() const
{
    return FieldType == FieldType_MutableReference;
}
