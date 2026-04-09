#include <mcc/error.hpp>
#include <mcc/value.hpp>

auto mcc::operator<=>(
        const WeakValuePtr &lhs,
        const WeakValuePtr &rhs)
{
    if (lhs.expired() && rhs.expired())
        return std::strong_ordering::equivalent;
    if (lhs.expired())
        return std::strong_ordering::less;
    if (rhs.expired())
        return std::strong_ordering::greater;
    return lhs.lock() <=> rhs.lock();
}

static uintptr_t global_stack = 0;

mcc::ValueBase::ValueBase(
        SourceLocation where,
        std::string name,
        TypePtr type,
        const FieldType_ field_type)
    : Where(std::move(where)),
      Name(std::move(name)),
      Type(std::move(type)),
      FieldType(field_type),
      StackId(global_stack++)
{
}

void mcc::ValueBase::Generate(
        CommandVector &commands,
        bool stack) const
{
    Error(Where, "mcc::ValueBase::Generate");
}

bool mcc::ValueBase::RequireStack() const
{
    Error(Where, "mcc::ValueBase::RequireStack");
}

mcc::Result mcc::ValueBase::GenerateResult() const
{
    Error(Where, "mcc::ValueBase::GenerateResult");
}

mcc::Result mcc::ValueBase::GenerateResultUnwrap() const
{
    return GenerateResult();
}

void mcc::ValueBase::Replace(
        ValuePtr value,
        ValuePtr replacement)
{
    Error(Where, "mcc::ValueBase::Replace");
}

void mcc::ValueBase::Use(WeakValuePtr user)
{
    Uses.insert(user);
}

void mcc::ValueBase::Drop(WeakValuePtr user)
{
    Uses.erase(user);
}

bool mcc::ValueBase::IsMutable() const
{
    return FieldType == FieldType_::MutableReference;
}
